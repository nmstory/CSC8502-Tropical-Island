#pragma once
#include "Mesh.h"

class Water : public Mesh {
public:
	Water(const GLuint tex, Shader* s);
	~Water();

	void Draw(Matrix4& modelMatrix, Matrix4& textureMatrix) final;
private:
	GLuint waterTex;
	Shader* shader;
};