#version 330 core
// Texturing uniforms
uniform sampler2D diffuseTexDirt;
uniform sampler2D diffuseTexRock;
uniform sampler2D bumpTexDirt;
uniform sampler2D bumpTexRock;

// Lighting uniforms
uniform vec3 	cameraPos;
uniform vec4 	lightColour;
uniform vec3	lightPos;
uniform float 	lightRadius;

in Vertex {
	vec2 texCoord;
	vec3 normal;
	vec3 tangent;
	vec3 binormal;
	vec3 worldPos;
} IN;

out vec4 fragColour;

void main(void) {
	// ==== Texturing ====
	vec3 colorDirt = texture(diffuseTexDirt, IN.texCoord).rgb;
	vec3 colorRock = texture(diffuseTexRock, IN.texCoord).rgb;
	vec3 bumpDirt = texture(bumpTexDirt, IN.texCoord).rgb;
	vec3 bumpRock = texture(bumpTexRock, IN.texCoord).rgb;

	// if height < -1, dirt
	//if height > 1, rock
	// else, blend/mix in middle

	// Calculating diffuse
	float a = smoothstep(-10.0, 250.0, IN.worldPos.y);
	vec3 final_diffuse_texture = mix(colorDirt, colorRock, a);
	//fragColour = vec4(final_texture, 1);
	vec4 diffuse = vec4(final_diffuse_texture, 1);

	// Calculating bump map using a from diffuse (as blend has already been decided)
	vec3 bumpNormal = mix(bumpDirt, bumpRock, a); // with normal maps, we dont use alpha, so vec3

	// ==== Lighting ====
	vec3 incident = normalize(lightPos - IN.worldPos);
	vec3 viewDir = normalize(cameraPos - IN.worldPos);
	vec3 halfDir = normalize(incident + viewDir);

	mat3 TBN = mat3(normalize(IN.tangent), normalize(IN.binormal), normalize(IN.normal));

	bumpNormal = normalize(TBN * normalize(bumpNormal * 2.0 - 1.0));

	float lambert = max(dot(incident , bumpNormal), 0.0f);
	float distance = length(lightPos - IN.worldPos );
	float attenuation = 1.0f - clamp(distance / lightRadius ,0.0 ,1.0);

	float specFactor = clamp(dot(halfDir, bumpNormal) ,0.0 ,1.0);
	specFactor = pow(specFactor , 60.0 );

	vec3 surface = (diffuse.rgb * lightColour.rgb);
	fragColour.rgb = surface * lambert * attenuation;
	fragColour.rgb += (lightColour.rgb * specFactor) * attenuation *0.33;
	fragColour.rgb += surface * 0.1f;
	fragColour.a = diffuse.a;
}