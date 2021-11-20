#version 450 core

layout (vertices = 4) out;

void main(void)
{
	if(gl_InvocationID == 0) {
		gl_TessLevelInner[0] = 50.0;
		gl_TessLevelInner[1] = 50.0;
		gl_TessLevelOuter[0] = 25.0;
		gl_TessLevelOuter[1] = 25.0;
		gl_TessLevelOuter[2] = 25.0;
		gl_TessLevelOuter[3] = 25.0;
	}
	
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}
