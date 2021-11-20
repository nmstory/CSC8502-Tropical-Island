#version 430 core

layout(vertices = 4) out; // producing 4 vertices of output

void main(void) {
	if (gl_InvocationID == 0) // to not do same stuff 4 times
		{
			// calculate A, B, C, D,E, F - based on your algorithm, or just set constants
			// set 2 innner tess. levels
			gl_TessLevelInner[0] = A;
			gl_TessLevelInner[1] = B;
			// set 4 outer tess. levels (for each edge)
			gl_TessLevelOuter[0] = C;
			gl_TessLevelOuter[1] = D;
			gl_TessLevelOuter[2] = E;
			gl_TessLevelOuter[3] = F;
		}
		gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
	}
}