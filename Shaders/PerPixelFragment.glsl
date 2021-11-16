#version 330 core

uniform sampler2D diffuseTex;
uniform vec3 cameraPos;
uniform vec4 lightColour;
uniform vec3 lightPos;
uniform float lightRadius;

in Vertex {
	vec3 colour;
	vec2 texCoord;
	vec3 normal;
	vec3 worldPos;
} IN;

out vec4 fragColour;

void main(void)	{
	// Most light models need the incident, view and half-angle vectors (see notes on why we calculate these why)
	vec3 incident = normalize(lightPos - IN.worldPos);
	vec3 viewDir = normalize(cameraPos - IN.worldPos);
	vec3 halfDir = normalize(incident + viewDir);

	// Diffuse simply needs the colour of the surface of the object, representing via a texture, so this is sampled
	vec4 diffuse = texture(diffuseTex , IN.texCoord);

	// Lamberts cosine law gives us the amount of light hitting a surface - clamped between 0 and 1 so surfaces facing away from light don't get neg amounts of light!
	float lambert = max(dot(incident , IN.normal), 0.0f);

	// Attenuation (here we're using linear attenuation, cheap and chearful) of the light source at the current fragment
	float distance = length(lightPos - IN.worldPos );
	float attenuation = 1.0 - clamp(distance / lightRadius , 0.0, 1.0);

	// Calculating specularity of the fragment
	float specFactor = clamp(dot(halfDir , IN.normal ) ,0.0 ,1.0);
	specFactor = pow(specFactor , 60.0 );

	vec3 surface = (diffuse.rgb * lightColour.rgb);
	fragColour.rgb = surface * lambert * attenuation; // calulating diffuse
	fragColour.rgb += (lightColour.rgb * specFactor )* attenuation *0.33; // adding specularity ( * 0.33 - slightly lowering brightness) 
	fragColour.rgb += surface * 0.1f; // ambient!
	fragColour.a = diffuse.a; 

}