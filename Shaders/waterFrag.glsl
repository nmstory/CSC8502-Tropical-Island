#version 450 core

// Texture uniforms
uniform sampler2D diffuseWater;
uniform sampler2D bumpWater;

// Lighting uniforms
uniform vec3 	cameraPos;
uniform vec4 	lightColour;
uniform vec3	lightPos;
uniform float 	lightRadius;

// View and projection matrices
uniform mat4 projMatrix;
uniform mat4 viewMatrix;

in gs_fs {
	vec3 normal;
	vec3 tangent;
	vec3 binormal;
	vec3 worldPosition;
} IN;

out vec4 fragColour;

void main(void)
{
	// Scaling the water texture when sampling, given the size of the water plane
	vec3 diffuse = texture(diffuseWater, (IN.worldPosition.xz)/500.0f).rgb;
	
	fragColour.rgb = diffuse.rgb;
	fragColour.a = 0.3f;

	// This is what used to be a part of the water's fragment shader. However, given the change of the winding order
	// in tesselation shaders at different stages of the generation, the following functionality didn't interact with
	// the light exactly as planned:

	//vec4 diffuse = vec4(texture(diffuseWater, IN.worldPosition.xz).rgb, 1);
	//vec3 bumpNormal = texture(bumpWater, IN.worldPosition.xz).rgb;

	//vec3 incident = normalize(lightPos - IN.worldPosition);
	//vec3 viewDir = normalize(cameraPos - IN.worldPosition);
	//vec3 halfDir = normalize(incident + viewDir);

	//mat3 TBN = mat3(normalize(IN.tangent), normalize(IN.binormal), normalize(IN.normal));

	//bumpNormal = normalize(TBN * normalize(bumpNormal * 2.0 - 1.0));

	//float lambert = max(dot(incident , bumpNormal), 0.0f);
	//float distance = length(lightPos - IN.worldPosition);
	//float attenuation = 1.0f - clamp(distance / lightRadius, 0.0, 1.0);

	//float specFactor = clamp(dot(halfDir, bumpNormal), 0.0, 1.0);
	//specFactor = pow(specFactor , 60.0 );

	//vec3 surface = (diffuse.rgb * lightColour.rgb);
	//fragColour.rgb = surface * lambert * attenuation;
	//fragColour.rgb += (lightColour.rgb * specFactor) * attenuation * 0.33;
	//fragColour.rgb += surface * 0.1f;
	//fragColour.a = diffuse.a;
}