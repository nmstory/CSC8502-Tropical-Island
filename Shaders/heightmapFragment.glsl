#version 330 core
// Texturing uniforms
uniform sampler2D diffuseTexDirt;
uniform sampler2D diffuseTexRock;

// Lighting uniforms
uniform vec3 	cameraPos;
uniform vec4 	lightColour;
uniform vec3	lightPos;
uniform float 	lightRadius;

in Vertex {
	vec2 texCoord;
	vec3 normal;
	vec3 tangent;
	vec3 binormal;
	vec3 worldPos;
} IN;

out vec4 fragColour;

void main(void) {
	//fragColour = texture(diffuseTexRock, IN.texCoord);

	// ==== Texturing ====
	vec3 colorDirt = texture(diffuseTexDirt, IN.texCoord).rgb;
	vec3 colorRock = texture(diffuseTexRock, IN.texCoord).rgb;

	// if height < -1, dirt
	//if height > 1, rock
	// else, blend/mix in middle

	float a = smoothstep(-10.0, 250.0, IN.worldPos.y);
	vec3 final_texture = mix(colorDirt, colorRock, a);
	fragColour = vec4(final_texture, 1);

	// ==== Lighting ====

// had here the main function from bumpFragment.glsl, but will need to modify for the two textures?
	
}