#version 400

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

in vec3 position;
in vec2 texCoord;
in vec4 jointWeights;
in ivec4 jointIndices; // ivec4 - vec4 of integers

uniform mat4 joints[128];

out Vertex {
	vec2 texCoord;
} OUT;

void main(void) {
	vec4 localPos = vec4(position, 1.0f);
	vec4 skelPos = vec4(0, 0, 0, 0);

	// Getting joint index and weighting, then using that to determine the skeletons position
	for(int i = 0; i < 4; ++i) {
		int jointIndex = jointIndices[i];
		float jointWeight = jointWeights[i];
		skelPos += joints[jointIndex] * localPos * jointWeight;
	}

	// Still in local space, so returning final clip space position here
	mat4 mvp = projMatrix * viewMatrix * modelMatrix;
	gl_Position = mvp * vec4(skelPos.xyz , 1.0);

	OUT.texCoord = texCoord;
}