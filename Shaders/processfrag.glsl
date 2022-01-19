#version 330 core

uniform sampler2D sceneTex;

uniform int isVertical;

in Vertex {
	vec2 texCoord;
} IN;

out vec4 fragColor;

// Weightings that each texel is multiplied by, to determine it's weighting in the final fragment shader
const float scaleFactors [7] = float [](0.01, 0.07, 0.242, 0.383, 0.245, 0.061, 0.002); // sum > 1 for bright colour grading

void main(void) {
	fragColor = vec4 (0, 0, 0, 1);
	vec2 delta = vec2 (0, 0);

	// Getting partial derivatives of the incoming interpolated texture coordinates
	if (isVertical == 1) {
		delta = dFdy(IN.texCoord );
	}
	else {
		delta = dFdx(IN.texCoord );
	}

	// Calculating this texels weighting, in accordance to the scale factors
	for (int i = 0; i < 7; i++) {
		vec2 offset = delta * (i - 3);
		vec4 tmp = texture2D(sceneTex , IN.texCoord.xy + offset);
		fragColor += tmp * scaleFactors[i];
	}
}