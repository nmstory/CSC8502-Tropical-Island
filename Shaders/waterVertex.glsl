#version 450 core

uniform mat4 projMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

in vec3 position;

void main(void) {
	//gl_Position = (projMatrix * viewMatrix * modelMatrix) * vec4(position , 1.0);
	gl_Position = vec4(position, 1.0);
}