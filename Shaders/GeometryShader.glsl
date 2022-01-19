#version 450 core

layout(triangles) in;
layout(triangle_strip) out;
layout(max_vertices = 3) out;

in vec3 worldPos[];

uniform mat4 modelMatrix;

out gs_fs {
	vec3 normal;
	vec3 tangent;
	vec3 binormal;
	vec3 worldPosition;
} OUT;

void main(void) {
	vec3 ab = worldPos[1] - worldPos[0];
	vec3 ac = worldPos[2] - worldPos[0];
	vec3 n = normalize(cross(ab, ac));
	
	// tangent
	vec3 t = normalize(ab);

	// binormal
	vec3 bn = normalize(cross(t, n));

	OUT.normal = n;
	OUT.tangent = t;
	OUT.binormal = bn;

	// Passing along worldPosition to fragment shader
	for(int i = 0; i < gl_in.length(); i++) {
		OUT.worldPosition = worldPos[i];
		gl_Position = gl_in[i].gl_Position;
		EmitVertex();	
	}
	EndPrimitive();
}