#version 450 core

in vec3 fragNormal;
in vec2 fragTexCoord;
in vec3 fragPos;
in vec4 fragPosLightSpace;

out vec4 color;

// Texture
uniform sampler2D Texture;
uniform sampler2D ShadowMap;

// Light Variables
uniform vec3 LightColour;
uniform vec3 LightPosition;

// Specular Variables
uniform float SpecularStrength;
uniform float Shininess;

// Ambient Variables
uniform float AmbientStrength;
uniform vec3 AmbientColour;

// Camera Position
uniform vec3 CameraPosition;

// Linear Fog
uniform vec4 fogColour = vec4(1.0f, 0.5f, 0.5f, 1.0f);
uniform float fogStart = 40.0f;
uniform float fogLength = 10.0f;

float ShadowCalculation()
{
	vec3 ndcSpace = fragPosLightSpace.xyz / fragPosLightSpace.w;
	vec3 texCoordSpace = 0.5f * ndcSpace + 0.5f;

	float currentDepth = texCoordSpace.z - 0.005f;

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

void main(void)
{
	// Ambient
	vec3 ambient = AmbientStrength * AmbientColour;

	// Light Direction
	vec3 norm = normalize(fragNormal); // Norm get from normal map?
	vec3 lightDir = normalize(fragPos - LightPosition);

	// Diffuse Coloring
	float diffuseStr = max(dot(norm, -lightDir), 0.0f);
	vec3 diffuse = diffuseStr * LightColour;

	// Specular Highlight
	vec3 negViewDir = normalize(CameraPosition - fragPos);
	vec3 reflectDir = reflect(lightDir, norm);
	float spec = pow(max(dot(negViewDir, reflectDir), 0.0f), Shininess);
	vec3 specular = SpecularStrength * spec * LightColour;

	// Calculate Colour
	vec3 totalColour = ambient + ((diffuse + specular) * ShadowCalculation());
	color = vec4(totalColour, 1.0f) * texture(Texture, fragTexCoord);

	// Fog colour
	float lerp = (distance(fragPos.xyz, CameraPosition) - fogStart) / fogLength;
	lerp = clamp(lerp, 0.0f, 1.0f);
	color = mix(color, fogColour, lerp);
}