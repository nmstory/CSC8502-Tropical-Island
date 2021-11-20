#version 450 core

uniform mat4 projMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

in vec3 position;

void main(void) {
	//gl_Position = (projMatrix * viewMatrix * modelMatrix) * vec4(position , 1.0);
	vec3 pos = position;
	pos.y = sin(pos.x);
	gl_Position = (projMatrix * viewMatrix * modelMatrix) * vec4(pos , 1.0);
}