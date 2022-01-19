#pragma once

#include "Matrix4.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Mesh.h"
#include "MeshAnimation.h"
#include "MeshMaterial.h"
#include <vector>

class Mesh;
class Light;
class Shader;
class OGLRenderer;
class Renderer;

struct NodeTexture {
	NodeTexture(GLchar* n, GLuint t) {
		this->name = n;
		this->tex = t;
	}

	GLchar* name;
	GLuint tex;
};

class SceneNode {
public:
	SceneNode(Mesh* m = NULL, Shader* shader = NULL, Vector4 colour = Vector4(1, 1, 1, 1), float boundingSize = 1);
	~SceneNode();
	
	void SetTransform(const Matrix4 & matrix) { transform = matrix; }
	const Matrix4 & GetTransform() const { return transform; }
	Matrix4 GetWorldTransform() const { return worldTransform; }
	
	Vector4 GetColour() const { return colour; }
	void SetColour(Vector4 c) { colour = c; }

	Vector3 GetModelScale() const { return modelScale; }
	void SetModelScale(Vector3 s) { modelScale = s; }

	Mesh * GetMesh() const { return mesh; }
	void SetMesh(Mesh * m) { mesh = m; }

	Shader* GetShader()  const { return shader; }
	void SetShader(Shader* s) { shader = s; }

	Light* GetLight()  const { return light; }
	void SetLight(Light* l) { light = l; }
	
	MeshAnimation* GetAnimation()  const { return anim; }
	void SetAnimation(MeshAnimation* a) { this->anim = a; }

	void AddTexture(GLchar* name, GLuint tex) { textures.push_back(NodeTexture(name, tex)); }

	void SetRequireCamera(bool b) { this->requireCamera = b; }

	void AddChild(SceneNode * s);
	
	virtual void Update(float dt);
	virtual void Draw(Renderer& r);

	std::vector <SceneNode*>::const_iterator GetChildIteratorStart() {
		return children.begin();
	}
	std::vector <SceneNode*>::const_iterator GetChildIteratorEnd() {
		return children.end();
	}

	float GetBoundingRadius() const { return boundingRadius; }
	void SetBoundingRadius(float f) { boundingRadius = f; }

	float GetCameraDistance() const { return distanceFromCamera; }
	void SetCameraDistance(float f) { distanceFromCamera = f; }

	void SetTexture(GLuint tex) { texture = tex; }
	GLuint GetTexture() const { return texture; }

	static bool CompareByCameraDistance(SceneNode* a, SceneNode* b) {
		return (a->distanceFromCamera < b->distanceFromCamera) ? true : false;
	}
	bool animated = false;
protected:
	SceneNode* parent;
	Mesh* mesh;
	Matrix4 worldTransform;
	Matrix4 transform;
	Vector3 modelScale;
	Vector4 colour;
	std::vector <SceneNode*> children;

	float distanceFromCamera;
	float boundingRadius;
	GLuint texture;
	Shader* shader;
	Light* light = NULL;
	std::vector<NodeTexture> textures;

	// Animations
	MeshAnimation* anim;
	MeshMaterial* material;
	int currentFrame;
	

	bool requireCamera;

};