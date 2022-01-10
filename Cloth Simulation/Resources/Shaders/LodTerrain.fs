#version 430 core

in vec3 fragPos;
in vec3 fragNormal;
in vec4 fragPosLightSpace;

out vec4 color;


uniform float	ambientStr		= 0.1f;
uniform vec3	ambientColor	= vec3(1.0f, 1.0f, 1.0f);

uniform vec3	lightColor		= vec3(1.0f, 1.0f, 1.0f);
uniform vec3	lightPos		= vec3(-2.0f, 3.0f, 3.0f);

uniform float	lightSpecStr	= 0.01f;
uniform float	shininess		= 10.0f;
uniform vec3	camPos			= vec3(1.0f, 1.0f, 2.0f);

uniform vec4 fogColour = vec4(0.5f, 0.5f, 0.5f, 1.0f);
uniform float fogStart = 10.0f;
uniform float fogLength = 1.0f;

uniform sampler2D ColourGradient;
uniform sampler2D ShadowMap;

float ShadowCalculation()
{
	vec3 ndcSpace = fragPosLightSpace.xyz / fragPosLightSpace.w;
	vec3 texCoordSpace = 0.5f * ndcSpace + 0.5f;

	float currentDepth = texCoordSpace.z - 0.002f;
	
	vec2 texelSize = 1.0f / textureSize(ShadowMap, 0);
	float shadow = 0.0f;

	for(int x = -1; x <= 1; ++x)
	{
		for(int y = -1; y <= 1; ++y)
		{
			float pcfDepth = texture(ShadowMap, texCoordSpace.xy + vec2(x, y) * texelSize).x;
			shadow += currentDepth < pcfDepth ? 1.0f : 0.0f;
		}
	}

	shadow /= 9.0;
	return shadow;
}

void main()
{
	// Ambient
	vec3 ambient = ambientStr * ambientColor;

	// Light Direction
	vec3 norm = fragNormal;
	vec3 lightDir = normalize(fragPos - lightPos);

	// Diffuse Coloring
	float diffuseStr = max(dot(norm, -lightDir), 0.0f);
	vec3 diffuse = diffuseStr * lightColor;

	// Specular Highlight
	vec3 negViewDir = normalize(camPos - fragPos);
	vec3 reflectDir = reflect(lightDir, norm);
	float spec = pow(max(dot(negViewDir, reflectDir), 0.0f), shininess);
	vec3 specular = lightSpecStr * spec * lightColor;

	// Colour
	vec4 col = texture(ColourGradient, vec2(clamp(fragNormal.y * 50.0f - 49.0f, 0.0f, 1.0f), 0.5f));
	col = mix(col, vec4(0.0f, 0.0f, 1.0f, 1.0f), clamp((0.6f - fragPos.y) * 5.0f, 0.0f, 1.0f));

	color = vec4(ambient + ((diffuse + specular) * ShadowCalculation()), 1.0f) * col;
	
	// Fog
	float lerp = (distance(fragPos, camPos) - fogStart) / fogLength;
	lerp = clamp(lerp, 0.0f, 1.0f);
	color = mix(color, fogColour, lerp);
}