#version 330 core

uniform samplerCube cubeTex;

in Vertex {
	vec3 viewDir;
} IN;

out vec4 fragColour;

void main(void) {
	// Sampling the cube map texture from the view direction, calculated in skybox vertex shader
	fragColour = texture(cubeTex, normalize(IN.viewDir));
}