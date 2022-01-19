#version 450 core

layout (vertices = 4) out;

in vec3 worldPos [];
out vec3 worldPosition [];

void main(void)
{
	if(gl_InvocationID == 0) {
		gl_TessLevelInner[0] = 20000.0;
		gl_TessLevelInner[1] = 20000.0;
		gl_TessLevelOuter[0] = 10000.0;
		gl_TessLevelOuter[1] = 10000.0;
		gl_TessLevelOuter[2] = 10000.0;
		gl_TessLevelOuter[3] = 10000.0;
	}
	
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
	
	worldPosition[gl_InvocationID] = worldPos[gl_InvocationID];
}
