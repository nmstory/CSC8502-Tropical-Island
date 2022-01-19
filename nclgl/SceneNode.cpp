#include "SceneNode.h"
#include "../Blank Project/Renderer.h"


SceneNode::SceneNode(Mesh* mesh, Shader* shader, Vector4 colour, float boundingSize) {
	this->mesh = mesh;
	this->colour = colour;
	this->shader = shader;
	this->boundingRadius = boundingSize;
	parent = NULL;
	modelScale = Vector3(1, 1, 1);

	boundingRadius = 1.0f;
	distanceFromCamera = 0.0f;
	texture = 0;
}

SceneNode::~SceneNode() {
	delete mesh;
	delete shader;
	delete material;
	delete anim;
	
	for (unsigned int i = 0; i < children.size(); ++i) {
		delete children[i];
	}
}

void SceneNode::AddChild(SceneNode* s){
	children.push_back(s);
	s->parent = this;
}

void SceneNode::Draw(Renderer& r) {

	// BindShader done in Renderer

	for (int i = 0; i < textures.size(); ++i) {
		glUniform1i(glGetUniformLocation(shader->GetProgram(), textures[i].name), i);
		glActiveTexture(GL_TEXTURE0 + i);
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, textures[i].tex);
	}

	if (animated) { // If this scene node is animated
		if (r.GetFrameTime() < 0.0f) {
			currentFrame = (currentFrame + 1) % anim->GetFrameCount();
			r.IncrementFrameTime(1.0f / anim->GetFrameRate());
		}
		else {
			std::vector <Matrix4> frameMatrices; // Temporary vector of matrices to send as joints to vertex shaders

			const Matrix4* invBindPose = mesh->GetInverseBindPose();
			const Matrix4* frameData = anim->GetJointData(currentFrame);

			for (unsigned int i = 0; i < mesh->GetJointCount(); ++i) {
				frameMatrices.emplace_back(frameData[i] * invBindPose[i]);
			}

			// Sending array of joint data, so the vertices can be correctly skinned
			int j = glGetUniformLocation(shader->GetProgram(), "joints");
			glUniformMatrix4fv(j, frameMatrices.size(), false, (float*)frameMatrices.data());
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textures[0].tex);
		}
	}

	if(requireCamera) glUniform3fv(glGetUniformLocation(shader->GetProgram(), "cameraPos"), 1, (float*)r.GetCamera());

	if (mesh) mesh->Draw();
}

void SceneNode::Update(float dt) {
	if (parent) { // If this node has a parent...
		worldTransform = parent->worldTransform * transform;
	}
	else { // Root node, world transform will be the local transform!
		worldTransform = transform;
	}
	for (std::vector<SceneNode*>::iterator i = children.begin(); i != children.end(); ++i) {
		(*i)->Update(dt);
	}
}