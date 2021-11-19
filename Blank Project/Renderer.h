#pragma once
#include "../nclgl/OGLRenderer.h"
#include "../nclgl/SceneNode.h"
#include "../nclgl/Frustum.h"
#include <algorithm>
#include <vector>

class HeightMap;
class Camera;

class Renderer : public OGLRenderer {
public:
	Renderer(Window& parent);
	~Renderer(void);
	void RenderScene() override;
	void UpdateScene(float dt) override;

protected:
	// Rendering functions
	void DrawSkybox();
	void DrawHeightmap();
	void DrawWater();

	// Scene Node functions 
	void BuildNodeLists(SceneNode* from);
	void SortNodeLists();
	void ClearNodeLists();
	void DrawNodes();
	void DrawNode(SceneNode* n);

	// Lighting
	Light* light;
	Shader* lightShader;
	Shader* reflectShader;

	// Scene Management
	SceneNode* root;
	Frustum frameFrustum;
	std::vector <SceneNode*> transparentNodeList;
	std::vector <SceneNode*> nodeList;

	// Heightmap
	HeightMap* heightMap;
	Shader* heightMapShader;
	GLuint	terrainTexRock;
	GLuint	terrainTexDirt;

	// Skybox
	GLuint cubeMap;
	Shader* skyboxShader;
	Mesh* quad;

	// Water
	GLuint waterTex;
	float waterRotate;
	float waterCycle;

	// Misc.
	Camera* camera;




};