#include "Renderer.h"
#include "../nclgl/camera.h"
#include "../nclgl/HeightMap.h"

Renderer::Renderer(Window& parent) : OGLRenderer(parent) {
	//heightMap = new HeightMap(TEXTUREDIR"noise.png");
	quad = Mesh::GenerateQuad();

	heightMap = new HeightMap(TEXTUREDIR"tenerifeINVERT.png");
	
	
	cubeMap = SOIL_load_OGL_cubemap(
		TEXTUREDIR"sky_west.jpg", TEXTUREDIR"sky_east.jpg",
		TEXTUREDIR"sky_up.jpg", TEXTUREDIR"sky_down.jpg",
		TEXTUREDIR"sky_south.jpg", TEXTUREDIR"sky_north.jpg",
		SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);
	/*
	cubeMap = SOIL_load_OGL_cubemap(
		TEXTUREDIR"sky2-west.png", TEXTUREDIR"sky2-east.png",
		TEXTUREDIR"sky2-up.png", TEXTUREDIR"sky2-down.png",
		TEXTUREDIR"sky2-south.png", TEXTUREDIR"sky2-north.png",
		SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);*/

	

	if (!heightMap || !cubeMap) {
		return;
	}

	Vector3 heightmapSize = heightMap->GetHeightmapSize();
	light = new Light(heightmapSize * Vector3(0.5f, 2.5f, 0.5f), Vector4(0.9, 0.9, 0.9, 1), heightmapSize.x);

	std::queue<Vector3> cameraTrack; // temp!!
	cameraTrack.emplace(Vector3(0, 1, 1));
	//camera = new Camera(-40, 270, heightmapSize * Vector3(0.5, 2, 0.5), cameraTrack);
	camera = new Camera(-40, 270, Vector3(0,0,0), cameraTrack);

	heightMapShader = new Shader("heightmapVertex.glsl", "heightmapFragment.glsl");
	skyboxShader = new Shader("skyboxVertex.glsl", "skyboxFragment.glsl");
	waterShader = new Shader("WaterVertex.glsl", "WaterFrag.glsl", /*"GeometryShader.glsl"*/ "", "TCS.glsl", "TES.glsl");

	if (!heightMapShader->LoadSuccess() || !skyboxShader->LoadSuccess() || !waterShader->LoadSuccess()) {
		return;
	}

	//terrainTex = SOIL_load_OGL_texture(TEXTUREDIR"Barren Reds.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	terrainTexDirt = SOIL_load_OGL_texture(TEXTUREDIR"Dirt_DIFF.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	terrainTexRock = SOIL_load_OGL_texture(TEXTUREDIR"Rock_04_DIFF.PNG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	bumpMapDirt = SOIL_load_OGL_texture(TEXTUREDIR"Dirt_NRM.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	bumpMapRock = SOIL_load_OGL_texture(TEXTUREDIR"Rock_04_NRM.PNG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	if (!terrainTexDirt || !terrainTexRock || !bumpMapDirt || !bumpMapRock) {
		return;
	}

	SetTextureRepeating(terrainTexDirt, true);
	SetTextureRepeating(terrainTexRock, true);
	SetTextureRepeating(bumpMapDirt, true);
	SetTextureRepeating(bumpMapRock, true);

	projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);

	// SceneNode
	//root = new SceneNode();
	//root->AddChild(heightMap);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	// Which to remove?
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	waterRotate = 0.0f;
	waterCycle = 0.0f;

	init = true;
}

Renderer::~Renderer(void) {
	// check we definitely delete everything!
	delete heightMap;
	delete camera;
	delete heightMapShader;
	delete quad;
	delete light;
}

void Renderer::UpdateScene(float dt) {
	camera->UpdateCamera(dt);
	viewMatrix = camera->BuildViewMatrix();
	waterRotate += dt * 2.0f; //2 degrees a second
	waterCycle += dt * 0.25f; //10 units a second
}

void Renderer::BuildNodeLists(SceneNode* from) {
	if (frameFrustum.InsideFrustum(*from)) {
		Vector3 dir = from->GetWorldTransform().GetPositionVector() - camera->GetPosition();
		from->SetCameraDistance(Vector3::Dot(dir, dir));

		if (from->GetColour().w < 1.0f) {
			transparentNodeList.push_back(from);
		}
		else {
			nodeList.push_back(from);
		}
	}

	for (std::vector<SceneNode*>::const_iterator i = from->GetChildIteratorStart(); i != from->GetChildIteratorEnd(); ++i) {
		BuildNodeLists(*i);
	}
}

void Renderer::SortNodeLists() {
	// Once the node lists have been built, they now need to be sort

	std::sort(transparentNodeList.rbegin(), transparentNodeList.rend(), SceneNode::CompareByCameraDistance); // rbegin and rend means reverse
	std::sort(nodeList.begin(), nodeList.end(), SceneNode::CompareByCameraDistance);
}

void Renderer::ClearNodeLists() {
	transparentNodeList.clear();
	nodeList.clear();
}

/*
	Drawing scene graph using two for loops - opaque nodes first, then transparent nodes.
	Since we used the reverse iterators to sort the transprent nodes, they'll be drawn from furthers to closest just by ranging over the vector, same as the opaque nodes.

*/
void Renderer::DrawNodes() {
	for (const auto& i : nodeList) {
		DrawNode(i);
	}

	for (const auto& i : transparentNodeList) {
		DrawNode(i);
	}
}

void Renderer::DrawNode(SceneNode* n) {
	BindShader(n->GetShader());
	if (n->GetMesh()) {
		if (n->GetLight()) SetShaderLight(*n->GetLight());
		UpdateShaderMatrices();

		n->Draw(*this);

		/*
		Matrix4 model = n->GetWorldTransform() * Matrix4::Scale(n->GetModelScale());
		glUniformMatrix4fv(glGetUniformLocation(heightMapShader->GetProgram(), "modelMatrix"), 1, false, model.values);

		glUniform4fv(glGetUniformLocation(heightMapShader->GetProgram(), "nodeColour"), 1, (float*)&n->GetColour());

		GLuint texture = n->GetTexture();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);

		glUniform1i(glGetUniformLocation(heightMapShader->GetProgram(), "useTexture"), texture);

		n->Draw(*this);*/
	}
}

void Renderer::RenderScene() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//DrawSkybox();
	//DrawHeightmap();
	DrawWater();
}

void Renderer::DrawSkybox() {
	glDepthMask(GL_FALSE);

	BindShader(skyboxShader);
	UpdateShaderMatrices();

	quad->Draw();

	glDepthMask(GL_TRUE);
}

void Renderer::DrawHeightmap() {
	// Heightmap
	BindShader(heightMapShader);
	
	glUniform1i(glGetUniformLocation(heightMapShader->GetProgram(), "diffuseTexDirt"), 0);
	glUniform1i(glGetUniformLocation(heightMapShader->GetProgram(), "diffuseTexRock"), 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, terrainTexDirt);
	glGenerateMipmap(GL_TEXTURE_2D); // could do this without binding by calling glGenerateTextureMipMap (maybe in renderer constructor?) https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glGenerateMipmap.xhtml
	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, terrainTexRock);

	// Lighting
	glUniform1i(glGetUniformLocation(heightMapShader->GetProgram(), "bumpTexDirt"), 2);
	glUniform1i(glGetUniformLocation(heightMapShader->GetProgram(), "bumpTexRock"), 3);
	glActiveTexture(GL_TEXTURE0 + 2);
	glBindTexture(GL_TEXTURE_2D, bumpMapDirt);
	glActiveTexture(GL_TEXTURE0 + 3);
	glBindTexture(GL_TEXTURE_2D, bumpMapRock);

	glUniform3fv(glGetUniformLocation(heightMapShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());
	
	// Extra
	SetShaderLight(*light);
	UpdateShaderMatrices();
	heightMap->Draw();
}

void Renderer::DrawWater() {
	/*
	GLint MaxPatchVertices = 0;
	glGetIntegerv(GL_MAX_PATCH_VERTICES, &MaxPatchVertices);
	std::cout << "The maximum supported patch vertices: " << MaxPatchVertices << std::endl;
	*/
	BindShader(waterShader);

	//glPatchParameteri(GL_PATCH_VERTICES, 4);
	//glDrawArrays(GL_PATCHES, 0, 8);
	//glDrawArraysInstanced(GL_PATCHES, 0, 6, 64 * 64);
	glPatchParameteri(GL_PATCH_VERTICES, 4);

	UpdateShaderMatrices();
	quad->Draw();


	/*
	BindShader(reflectShader);

	glUniform3fv(glGetUniformLocation(reflectShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());

	glUniform1i(glGetUniformLocation(reflectShader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(reflectShader->GetProgram(), "cubeTex"), 2);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, waterTex);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);

	Vector3 hSize = heightMap->GetHeightmapSize();

	modelMatrix = Matrix4::Translation(hSize * 0.5f) * Matrix4::Scale(hSize * 0.5f) * Matrix4::Rotation(90, Vector3(1, 0, 0));

	textureMatrix = Matrix4::Translation(Vector3(waterCycle, 0.0f, waterCycle)) * Matrix4::Scale(Vector3(10, 10, 10)) * Matrix4::Rotation(waterRotate, Vector3(0, 0, 1));

	UpdateShaderMatrices();
	quad->Draw();
	*/
}
	

// SetShaderLight (* light); //No lighting in this shader!