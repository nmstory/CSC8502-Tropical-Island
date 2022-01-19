#version 450 core

uniform mat4 modelMatrix;

in vec3 position;

out vec3 worldPos;

void main(void) {
	vec4 worldPosition = (modelMatrix * vec4(position, 1));
	worldPos = worldPosition.xyz;
	
	gl_Position = vec4(position , 1.0);
}