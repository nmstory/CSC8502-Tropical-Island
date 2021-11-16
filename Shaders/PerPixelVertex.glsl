#version 330 core
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

in vec3 position;
in vec4 colour;
in vec3 normal; //New Attribute!
in vec2 texCoord;

out Vertex {
	vec4 colour;
	vec2 texCoord;
	vec3 normal;
	vec3 worldPos;
} OUT;

// Vertex shader is predominantly calculating the direction in world space the normal is pointing

void main(void) {
	OUT.colour = colour;
	OUT.texCoord = texCoord;

	// Using inverse transpose model matrix for normal direction in world space
	mat3 normalMatrix = transpose(inverse(mat3(modelMatrix )));
	OUT.normal = normalize(normalMatrix * normalize(normal ));

	// World space pos of the processed vertex, calculated using modelMatrix
	// Stored as we need to pass it to the frag shader for lighting, and it's also part of the standard vertex processing (for going to clip space)
	vec4 worldPos = (modelMatrix * vec4(position ,1));

	OUT.worldPos = worldPos.xyz;

	gl_Position = (projMatrix * viewMatrix) * worldPos;
} 