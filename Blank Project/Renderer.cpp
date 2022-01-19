#include "Renderer.h"
#include "../nclgl/camera.h"
#include "../nclgl/HeightMap.h"

#define SHADOWSIZE 2048
const int POST_PASSES = 10;

Renderer::Renderer(Window& parent) : OGLRenderer(parent) {

	// Loading shaders
	Shader* heightMapShader = new Shader("heightmapVertex.glsl", "heightmapFragment.glsl");
	Shader* animationShader = new Shader("SkinningVertex.glsl", "texturedFragment.glsl");
	skyboxShader = new Shader("skyboxVertex.glsl", "skyboxFragment.glsl");
	waterShader = new Shader("WaterVertex.glsl", "WaterFrag.glsl", "GeometryShader.glsl", "TCS.glsl", "TES.glsl");
	shadowShader = new Shader("shadowVert.glsl", "shadowFrag.glsl");
	ptShader = new Shader("PalmTreeVertex.glsl", "PalmTreeFragment.glsl");
	sceneShader = new Shader("TexturedVertex.glsl", "TexturedFragment.glsl");
	processShader = new Shader("TexturedVertex.glsl", "processfrag.glsl");

	// Verifying shaders
	if (!heightMapShader->LoadSuccess() || !animationShader->LoadSuccess() || !skyboxShader->LoadSuccess() || !waterShader->LoadSuccess() || !shadowShader->LoadSuccess() || !ptShader->LoadSuccess()
		|| !sceneShader->LoadSuccess() || !processShader->LoadSuccess()) {
		return;
	}

	// Generating and loading textures
	quad = Mesh::GenerateQuad();
	HeightMap* heightMap = new HeightMap(TEXTUREDIR"tenerife.png");
	cubeMap = SOIL_load_OGL_cubemap(
		TEXTUREDIR"SKY_WEST.BMP", TEXTUREDIR"SKY_EAST.BMP",
		TEXTUREDIR"SKY_UP.BMP", TEXTUREDIR"SKY_DOWN.BMP",
		TEXTUREDIR"SKY_SOUTH.BMP", TEXTUREDIR"SKY_NORTH.BMP",
		SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);

	if (!heightMap || !cubeMap) {
		return;
	}

	// Generating light, water and the camera track
	Vector3 heightmapSize = heightMap->GetHeightmapSize();
	light = new Light(heightmapSize * Vector3(0.5f, 5.0f, 0.5f), Vector4(1, 0.98431373, 0.91372549, 0.9), 50000);
	waterPlane = waterPlane->GenerateWaterPlane(heightmapSize * Vector3(0.5, 0.2, 0.5), 10000);

	std::queue<Vector3> cameraTrack;
	cameraTrack.emplace(Vector3(9000, 600, 5000));
	cameraTrack.emplace(Vector3(8000, 600, 8000));
	cameraTrack.emplace(Vector3(6000, 600, 8000));
	camera = new Camera(-40, 270, heightmapSize * Vector3(0.3, 2, 0.3), cameraTrack);

	// Loading textures
	GLuint terrainTexDirt = SOIL_load_OGL_texture(TEXTUREDIR"Dirt_DIFF.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	GLuint terrainTexRock = SOIL_load_OGL_texture(TEXTUREDIR"Rock_04_DIFF.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	GLuint bumpMapDirt = SOIL_load_OGL_texture(TEXTUREDIR"Dirt_NRM.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	GLuint bumpMapRock = SOIL_load_OGL_texture(TEXTUREDIR"Rock_04_NRM.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	
	waterTexDiffuse = SOIL_load_OGL_texture(TEXTUREDIR"water.TGA", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	waterTexBump = SOIL_load_OGL_texture(TEXTUREDIR"waterbump.PNG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	if (!terrainTexDirt || !terrainTexRock || !bumpMapDirt || !bumpMapRock || !waterTexDiffuse || !waterTexBump) {
		return;
	}

	SetTextureRepeating(terrainTexDirt, true);
	SetTextureRepeating(terrainTexRock, true);
	SetTextureRepeating(bumpMapDirt, true);
	SetTextureRepeating(bumpMapRock, true);
	SetTextureRepeating(waterTexDiffuse, true);
	SetTextureRepeating(waterTexBump, true);

	// Generating heightmap
	root = new SceneNode();
	SceneNode* heightMapNode = new SceneNode();
	heightMapNode->SetShader(heightMapShader);
	heightMapNode->SetMesh(heightMap);
	heightMapNode->AddTexture("diffuseTexDirt", terrainTexDirt);
	heightMapNode->AddTexture("diffuseTexRock", terrainTexRock);
	heightMapNode->AddTexture("bumpTexDirt", bumpMapDirt);
	heightMapNode->AddTexture("bumpTexRock", bumpMapRock);
	heightMapNode->SetLight(light);
	heightMapNode->SetBoundingRadius(1000);
	root->AddChild(heightMapNode);

	// Loading in treasure chest
	SceneNode* treasureChest = LoadMeshAndMaterial("treasure_chest.msh", "treasure_chest.mat");
	treasureChest->SetShader(sceneShader);
	treasureChest->SetTransform(Matrix4::Translation(Vector3(8000.0f, 300.0f, 8000.0f)) * Matrix4::Rotation(180, Vector3(0, 1, 0)) * Matrix4::Scale(Vector3(1, 1, 1)));
	heightMapNode->AddChild(treasureChest);

	// Loading in animation character
	animCharacter = LoadMeshAndMaterial("Ch34_nonPBR@Swimming.msh", "Ch34_nonPBR@Swimming.mat", "Ch34_nonPBR@Swimming.anm");
	animCharacter->SetShader(animationShader);
	animCharacter->SetTransform(Matrix4::Translation(Vector3(9000, -20, 3000)) * Matrix4::Scale(Vector3(100, 100, 100)) * Matrix4::Rotation(-70, Vector3(0, 1, 0)) * Matrix4::Rotation(-90, Vector3(1,0,0)));
	heightMapNode->AddChild(animCharacter);

	// Generating shadow FBOs
	glGenTextures(1, &shadowTex);
	glBindTexture(GL_TEXTURE_2D, shadowTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	// 32 bits of depth precision for depth buffer (GL_DEPTH_COMPONENT)
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOWSIZE, SHADOWSIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Code to create an FBO and it's depth attachment
	glGenFramebuffers(1, &shadowFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowTex, 0);
	glDrawBuffer(GL_NONE); // GL_NONE as this is for the shadow map pass - a colour attachment isn't required
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Generating Palm Tree shadow objects
	Mesh* palmTreeMesh = Mesh::LoadFromMeshFile("palm_tree.msh");
	MeshMaterial* material = new MeshMaterial("palm_tree.mat");
	const MeshMaterialEntry* matEntry = material->GetMaterialForLayer(0);

	// Generating and placing palm trees
	const std::string* filename = nullptr;
	matEntry->GetEntry("Diffuse", &filename);
	std::string path = TEXTUREDIR + *filename;
	GLuint texID = SOIL_load_OGL_texture(path.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
	ptMatTextures.emplace_back(texID);
	filename = nullptr;
	matEntry->GetEntry("Bump", &filename);
	path = TEXTUREDIR + *filename;
	texID = SOIL_load_OGL_texture(path.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
	ptMatTextures.emplace_back(texID);
	
	for (int i = 0; i < 15; ++i) {
		int randomValx = rand() % 1500 + 0;
		int randomValz = rand() % 1600 + 0;
		shadowObjects.push_back(ShadowObject(palmTreeMesh, Matrix4::Translation(Vector3(8000 + randomValx, 200, 8000 + randomValz))));
	}
	for (int i = 15; i < 30; ++i) {
		int randomValx = rand() % 1500 + 0;
		int randomValz = rand() % 2000 + 0;
		shadowObjects.push_back(ShadowObject(palmTreeMesh, Matrix4::Translation(Vector3(9000 + randomValx, 200, 5000 + randomValz))));
	}
	for (int i = 30; i < 45; ++i) {
		int randomValx = rand() % 2000 + 0;
		int randomValz = rand() % 1300 + 0;
		shadowObjects.push_back(ShadowObject(palmTreeMesh, Matrix4::Translation(Vector3(6000 + randomValx, 200, 8000 + randomValz))));
	}

	// Post processing
	glGenTextures(1, &bufferDepthTex);
	glBindTexture(GL_TEXTURE_2D, bufferDepthTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	// 24 bits for depth, 8 bits for stencil buffers (GL_DEPTH24_STENCIL8)
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);

	// Generating colour FBO components
	for (int i = 0; i < 2; ++i) {
		glGenTextures(1, &bufferColourTex[i]);
		glBindTexture(GL_TEXTURE_2D, bufferColourTex[i]);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	}

	// Generating two new frame buffers
	glGenFramebuffers(1, &bufferFBO); // One for rendering the scene
	glGenFramebuffers(1, &processFBO);// One for processing the post process effects

	// Bind an FBO
	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);

	// Attach textures to it to render into
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, bufferDepthTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, bufferDepthTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[0], 0);

	// Checking FBO attachment success
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE || !bufferDepthTex || !bufferColourTex[0]) return;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);
	sceneTime = 0;
	frameTime = 0;
	init = true;
	showPostProcessing = false;
}

Renderer::~Renderer(void) {
	delete camera;
	delete quad;
	delete light;
	delete waterPlane;
	delete waterShader;
	delete shadowShader;
	delete ptShader;
	delete processShader;
	delete sceneShader;
	delete shadowObjects[0].mesh;
}

void Renderer::UpdateScene(float dt) {
	camera->UpdateCamera(dt);
	viewMatrix = camera->BuildViewMatrix();

	sceneTime += dt; // For water
	frameTime -= dt; // For animation

	// Enabling/disabling post processing effects
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_P)) {
		showPostProcessing = !showPostProcessing;
	}

	// Updating transform of the swimming animated character
	animCharacter->SetTransform(Matrix4::Translation(Vector3(animCharacter->GetTransform().GetPositionVector().x - (characterMoveSpeed * dt), 
		animCharacter->GetTransform().GetPositionVector().y,
		animCharacter->GetTransform().GetPositionVector().z)) * Matrix4::Scale(Vector3(100, 100, 100)) * Matrix4::Rotation(-90, Vector3(0,1,0)));
}

void Renderer::BuildNodeLists(SceneNode* from) {
	for (std::vector<SceneNode*>::const_iterator i = from->GetChildIteratorStart(); i != from->GetChildIteratorEnd(); ++i) {
		nodeList.push_back(*i);
		BuildNodeLists(*i);
	}
}

void Renderer::SortNodeLists() {
	// Sorting node list now it's been built
	std::sort(nodeList.begin(), nodeList.end(), SceneNode::CompareByCameraDistance);
}

void Renderer::ClearNodeLists() {
	nodeList.clear();
}

void Renderer::DrawNodes() {
	for (const auto& i : nodeList) {
		DrawNode(i);
	}
}

void Renderer::DrawNode(SceneNode* n) {
	BindShader(n->GetShader());
	if (n->GetMesh()) {
		viewMatrix = camera->BuildViewMatrix();
		projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);

		if (n->GetLight()) SetShaderLight(*n->GetLight());

		glUniform1i(glGetUniformLocation(n->GetShader()->GetProgram(), "shadowTex"), 2);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, shadowTex);

		Matrix4 tempmm = modelMatrix;
		modelMatrix = n->GetTransform();
		UpdateShaderMatrices();
		modelMatrix = tempmm;

		n->Draw(*this);
	}
}

void Renderer::RenderScene() {
	if (showPostProcessing) {
		glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO); // Setting rendering target to the new FBO
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
		DrawSkybox();

		DrawShadowObjects();

		BuildNodeLists(root);
		DrawNodes();
		ClearNodeLists();
	
		DrawWater();
		glBindFramebuffer(GL_FRAMEBUFFER, 0); // Resetting rendering target

		DrawPostProcess();
		PresentScene();
	}
	else { // No post processing, show standard scene
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		DrawSkybox(); // Need to draw skybox first!

		DrawShadowScene();
		DrawShadowObjects();

		BuildNodeLists(root);
		DrawNodes();
		ClearNodeLists();

		DrawWater();
	}
}

void Renderer::DrawSkybox() {
	glDepthMask(GL_FALSE);

	BindShader(skyboxShader);
	UpdateShaderMatrices();

	quad->Draw();

	glDepthMask(GL_TRUE);
}

void Renderer::DrawWater() {

	BindShader(waterShader);

	SetShaderLight(*light);
	glUniform1f(glGetUniformLocation(waterShader->GetProgram(), "time"), sceneTime);

	glUniform1i(glGetUniformLocation(waterShader->GetProgram(), "diffuseWater"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, waterTexDiffuse);

	glUniform1i(glGetUniformLocation(waterShader->GetProgram(), "bumpWater"), 1);
	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, waterTexBump);

	glPatchParameteri(GL_PATCH_VERTICES, 4);

	UpdateShaderMatrices();
	waterPlane->Draw();

}

void Renderer::DrawShadowScene() {
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glClear(GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, SHADOWSIZE, SHADOWSIZE);

	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	BindShader(shadowShader);

	viewMatrix = Matrix4::BuildViewMatrix(light->GetPosition(), Vector3(0, 0, 0));
	projMatrix = Matrix4::Perspective(1, 100, 1, 45);
	shadowMatrix = projMatrix * viewMatrix; //used later

	Matrix4 tempmm = modelMatrix;
	for (int i = 0; i < shadowObjects.size(); ++i) {
		modelMatrix = shadowObjects[i].transform;
		UpdateShaderMatrices();
		shadowObjects[i].mesh->Draw();
	}
	modelMatrix = tempmm;

	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glViewport(0, 0, width, height);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::DrawShadowObjects() {
	BindShader(ptShader);
	SetShaderLight(*light);
	viewMatrix = camera->BuildViewMatrix();
	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);

	glUniform1i(glGetUniformLocation(ptShader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(ptShader->GetProgram(), "bumpTex"), 1);
	glUniform1i(glGetUniformLocation(ptShader->GetProgram(), "shadowTex"), 2);

	glUniform3fv(glGetUniformLocation(ptShader->GetProgram(), "cameraPos"), 3, (float*)&camera->GetPosition());

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, ptMatTextures[0]);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, ptMatTextures[1]);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, shadowTex);

	Matrix4 tempmm = modelMatrix;
	for (int i = 0; i < shadowObjects.size(); ++i) {
		modelMatrix = shadowObjects[i].transform;
		UpdateShaderMatrices();
		shadowObjects[i].mesh->Draw();
	}
	modelMatrix = tempmm;
}

void Renderer::DrawPostProcess() {
	glBindFramebuffer(GL_FRAMEBUFFER, processFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[1], 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	BindShader(processShader);
	modelMatrix.ToIdentity();
	viewMatrix.ToIdentity();
	projMatrix.ToIdentity();
	UpdateShaderMatrices();

	glDisable(GL_DEPTH_TEST);

	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(processShader->GetProgram(), "sceneTex"), 0);

	for (int i = 0; i < POST_PASSES; ++i) {
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[1], 0);
		glUniform1i(glGetUniformLocation(processShader->GetProgram(), "isVertical"), 0);

		glBindTexture(GL_TEXTURE_2D, bufferColourTex[0]);
		quad->Draw();

		//Now to swap the colour buffers, and do the second blur pass
		glUniform1i(glGetUniformLocation(processShader->GetProgram(), "isVertical"), 1);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[0], 0);
		glBindTexture(GL_TEXTURE_2D, bufferColourTex[1]);
		quad->Draw();
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glEnable(GL_DEPTH_TEST);
}

void Renderer::PresentScene() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	BindShader(sceneShader);
	modelMatrix.ToIdentity();
	viewMatrix.ToIdentity();
	projMatrix.ToIdentity();
	UpdateShaderMatrices();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, bufferColourTex[0]);
	glUniform1i(glGetUniformLocation(sceneShader->GetProgram(), "diffuseTex"), 0);
	quad->Draw();
}

SceneNode* Renderer::LoadMeshAndMaterial(std::string meshFilename, std::string materialFilename, std::string animationFilename) {
	SceneNode* sn = new SceneNode();

	sn->SetMesh(Mesh::LoadFromMeshFile(meshFilename));

	MeshMaterial* mm = new MeshMaterial(materialFilename);

	const MeshMaterialEntry* matEntry = mm->GetMaterialForLayer(0);

	const std::string* filename = nullptr;
	matEntry->GetEntry("Diffuse", &filename);

	std::string path = TEXTUREDIR + *filename;
	GLuint texID = SOIL_load_OGL_texture(path.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
	sn->AddTexture("diffuseTex", texID);

	filename = nullptr;
	matEntry->GetEntry("Bump", &filename);

	path = TEXTUREDIR + *filename;
	texID = SOIL_load_OGL_texture(path.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
	sn->AddTexture("bumpTex", texID);

	sn->SetLight(light);

	if(animationFilename.length() != 0) {
		sn->SetAnimation(new MeshAnimation(animationFilename));
		sn->animated = true;
	}

	return sn;
}