#version 450 core

in vec3 fragPos;
in vec3 fragNormal;
in vec2 fragTexCoord;

out vec4 color;

uniform sampler2D tex;
uniform vec3 camPos;

uniform float	ambientStr		= 0.1f;
uniform vec3	ambientColor	= vec3(1.0f, 1.0f, 1.0f);

uniform vec3	lightColor		= vec3(1.0f, 1.0f, 1.0f);
uniform vec3	lightPos		= vec3(-2.0f, 3.0f, 3.0f);

uniform float	lightSpecStr	= 1.0f;
uniform float	shininess		= 32.0f;

uniform vec4 fogColour = vec4(1.0f, 0.5f, 0.5f, 1.0f);
uniform float fogStart = 40.0f;
uniform float fogLength = 10.0f;

void main(void)
{
	// Ambient
	vec3 ambient = ambientStr * ambientColor;

	// Light Direction
	vec3 norm = normalize(fragNormal);
	vec3 lightDir = normalize(fragPos - lightPos);

	// Diffuse Coloring
	float diffuseStr = max(dot(norm, -lightDir), 0.0f);
	vec3 diffuse = diffuseStr * lightColor;

	// Specular Highlight
	vec3 negViewDir = normalize(camPos - fragPos);
	vec3 reflectDir = reflect(lightDir, norm);
	float spec = pow(max(dot(negViewDir, reflectDir), 0.0f), shininess);
	vec3 specular = lightSpecStr * spec * lightColor;

	// Lighting colour
	vec4 litColor = vec4(ambient + diffuse + specular, 1.0f) * texture(tex, fragTexCoord);
	
	// Fog colour
	float lerp = (distance(fragPos.xyz, camPos) - fogStart) / fogLength;
	lerp = clamp(lerp, 0.0f, 1.0f);
	color = mix(litColor, fogColour, lerp);
}