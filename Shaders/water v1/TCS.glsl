#version 400 core
layout ( vertices = 4) out; // num vertices in output patch

in Vertex {
	vec2 texCoord; // From Vertex Shader
} IN []; // Equal to size of the draw call vertex count

out Vertex {
	vec2 texCoord ; // To Evaluation Shader
} OUT []; // Equal to the size of the layout vertex count

void main(void)
{
    if (gl_InvocationID == 0) // to not do same stuff 4 times
    {
        // calculate A, B, C, D,E, F - based on your algorithm, or just set constants
        // set 2 innner tess. levels
        gl_TessLevelInner[0] = 9.0;
        gl_TessLevelInner[1] = 7.0;
        // set 4 outer tess. levels (for each edge)
        gl_TessLevelOuter[0] = 3.0;
        gl_TessLevelOuter[1] = 5.0;
        gl_TessLevelOuter[2] = 3.0;
        gl_TessLevelOuter[3] = 5.0;
    }
    // FROM OLD: gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
	
	OUT [gl_InvocationID].texCoord = IN[gl_InvocationID].texCoord;
	
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}