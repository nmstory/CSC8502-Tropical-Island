#version 450 core

uniform mat4 modelMatrix;
uniform mat4 projMatrix;
uniform mat4 viewMatrix;

uniform float time;

in vec3 worldPosition [];
out vec3 worldPos;

layout (quads) in;

void main (void)
{
	vec4 p1 = mix(gl_in[0].gl_Position, gl_in[1].gl_Position, gl_TessCoord.x);
	vec4 p2 = mix(gl_in[2].gl_Position, gl_in[3].gl_Position, gl_TessCoord.x);
	vec4 pos = mix(p1, p2, gl_TessCoord.y);
	
	// Generating waves (where amplitude = 7, speed = 5)
	pos.y += 7 * sin(pos.x - (5 * time));
	gl_Position = (projMatrix * viewMatrix) * vec4(pos.xyz, 1);

	// Passing world position to the geometry shader
	worldPos = pos.xyz;
}
