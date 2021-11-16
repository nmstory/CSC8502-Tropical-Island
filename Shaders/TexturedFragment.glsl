#version 330 core
uniform sampler2D diffuseTexDirt;
uniform sampler2D diffuseTexRock;

in Vertex {
	vec2 texCoord;
	vec3 worldPos;
} IN;

out vec4 fragColour;

void main(void) {
	//fragColour = texture(diffuseTexRock, IN.texCoord);

	vec3 colorDirt = texture(diffuseTexDirt, IN.texCoord).rgb;
	vec3 colorRock = texture(diffuseTexRock, IN.texCoord).rgb;

	// if height < -1, dirt
	//if height > 1, rock
	// else, blend/mix in middle

	float a = smoothstep(-10.0, 190.0, IN.worldPos.y);
	vec3 final_texture = mix(colorDirt, colorRock, a);
	fragColour = vec4(final_texture, 1);
	

	
}