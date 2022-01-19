#pragma once
#include "../nclgl/OGLRenderer.h"
#include "../nclgl/SceneNode.h"
#include "../nclgl/Frustum.h"
#include "../nclgl/MeshMaterial.h"
#include <algorithm>
#include <vector>

class HeightMap;
class Camera;
class OGLRenderer;

struct ShadowObject {
	ShadowObject() {
		this->mesh = NULL;
		this->transform = Matrix4::Scale(Vector3(1.0f, 1.0f, 1.0f));
	}
	ShadowObject(Mesh* m, Matrix4 t) {
		this->mesh = m;
		this->transform = t;
	}
	// mesh memory is freed in renderer destructor
	Mesh* mesh;
	Matrix4 transform;
};

class Renderer : public OGLRenderer {
public:
	Renderer(Window& parent);
	~Renderer(void);
	void RenderScene() override;
	void UpdateScene(float dt) override;

	// Auxilliary methods
	Camera* GetCamera() { return camera; }
	float GetFrameTime() { return frameTime; }
	void IncrementFrameTime(float t) { frameTime += t; }

protected:
	// Auxiliary method
	SceneNode* LoadMeshAndMaterial(std::string meshFilename, std::string materialFilename, std::string animationFilename = "");

	// Rendering functions
	void DrawShadowScene();
	void DrawShadowObjects();
	void DrawSkybox();
	void DrawWater();

	// Post processing functions
	void DrawPostProcess();
	void PresentScene();

	// Scene Node functions 
	void BuildNodeLists(SceneNode* from);
	void SortNodeLists();
	void ClearNodeLists();
	void DrawNodes();
	void DrawNode(SceneNode* n);

	// Lighting
	Light* light;

	// Scene Management
	SceneNode* root;
	Frustum frameFrustum;
	std::vector <SceneNode*> nodeList;

	// Skybox
	GLuint cubeMap;
	Shader* skyboxShader;
	Mesh* quad;

	// Water
	Mesh* waterPlane;
	GLuint waterTexDiffuse;
	GLuint waterTexBump;
	Shader* waterShader;
	float sceneTime;

	// Shadows
	GLuint shadowFBO;
	GLuint shadowTex;
	Shader* shadowShader;
	std::vector<ShadowObject> shadowObjects;

	// Palm Trees
	Shader* ptShader;
	MeshMaterial* ptMat;
	std::vector<GLuint> ptMatTextures;

	// Animation
	float frameTime;
	float characterMoveSpeed = 50;
	SceneNode* animCharacter;

	// Post processing
	GLuint bufferFBO;
	GLuint processFBO;
	GLuint bufferColourTex[2];
	GLuint bufferDepthTex;
	Shader* processShader;
	Shader* sceneShader;
	bool showPostProcessing;

	// Misc.
	Camera* camera;
};