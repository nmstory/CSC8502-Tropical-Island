#version 450 core

//layout (quads, equal_spacing, ccw) in;

uniform mat4 modelMatrix;
uniform mat4 projMatrix;
uniform mat4 viewMatrix;

layout (quads) in;

void main (void)
{
	vec4 p1 = mix(gl_in[0].gl_Position,
					gl_in[1].gl_Position,
				  gl_TessCoord.x);
	
	vec4 p2 = mix(gl_in[2].gl_Position,
				  gl_in[3].gl_Position,
				  gl_TessCoord.x);
	
	//gl_Position = mix(p1, p2, gl_TessCoord.y);
	vec4 pos = mix(p1, p2, gl_TessCoord.y);
	
	pos.y += sin(pos.x);

	gl_Position = pos;
}
