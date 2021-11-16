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
	// Scene Node functions 
	void BuildNodeLists(SceneNode* from);
	void SortNodeLists();
	void ClearNodeLists();
	void DrawNodes();
	void DrawNode(SceneNode* n);

	HeightMap* heightMap;
	SceneNode* root;

	Frustum frameFrustum;

	Camera* camera;
	Shader* shader;
	GLuint	terrainTexRock;
	GLuint	terrainTexDirt;

	std::vector <SceneNode*> transparentNodeList;
	std::vector <SceneNode*> nodeList;
};