#version 330 core
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat4 textureMatrix;

in vec3 position;
in vec4 colour;
in vec3 normal;
in vec4 tangent;
in vec2 texCoord;

out Vertex {
	vec2 texCoord;
	vec3 normal;
	vec3 tangent;
	vec3 binormal;
	vec3 worldPos;
} OUT;

void main(void) {
	mat4 mvp = projMatrix * viewMatrix * modelMatrix;
	gl_Position = mvp * vec4(position, 1.0);
	OUT.texCoord = (textureMatrix * vec4(texCoord , 0.0, 1.0)).xy;

	// Passing worldPos to frag
	vec4 worldPos = (modelMatrix * vec4(position, 1));
	OUT.worldPos = worldPos.xyz;

	// ==== Lighting ====
	
	// Like normals, tangents are transformed by the normal matrix
	mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));

	vec3 wNormal = normalize(normalMatrix * normalize(normal));
	vec3 wTangent = normalize(normalMatrix * normalize(tangent.xyz));

	// Normal and tangent carried over to the ouput interface block, whilst both are used to calculate binormal 
	OUT.normal = wNormal;
	OUT.tangent = wTangent;
	OUT.binormal = cross(wTangent , wNormal) * tangent.w;	
}