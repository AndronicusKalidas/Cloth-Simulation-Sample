#version 450 core

in vec3 fragPos;
in vec3 fragNormal;
in vec2 fragTexCoord;

out vec4 color;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_normal1;

uniform float	ambientStr		= 0.25f;
uniform vec3	ambientColor	= vec3(1.0f, 1.0f, 1.0f);

uniform vec3	lightColor		= vec3(1.0f, 1.0f, 1.0f);
uniform vec3	lightPos		= vec3(-2.0f, 6.0f, 3.0f);

uniform float	lightSpecStr	= 1.0f;
uniform float	shininess		= 1.0f;
uniform vec3	camPos			= vec3(1.0f, 1.0f, 2.0f);

uniform vec3	baseColour		= vec3(0.3f, 0.3f, 0.3f);

void main(void)
{
	// Ambient
	vec3 ambient = ambientStr * ambientColor;

	// Light Direction
	vec3 texNormal = vec3(texture(texture_normal1, fragTexCoord));
	vec3 norm = (length(texNormal) == 0.0f ? normalize(fragNormal) : normalize(vec3(texNormal.xy + fragNormal.xy, fragNormal.z)));
	//vec3 norm = normalize(fragNormal);
	//vec3 norm = texNormal;
	vec3 lightDir = normalize(fragPos - lightPos);

	// Diffuse Coloring
	float diffuseStr = max(dot(norm, -lightDir), 0.0f);
	vec3 diffuse = diffuseStr * lightColor;

	// Specular Highlight
	vec3 negViewDir = normalize(camPos - fragPos);
	vec3 reflectDir = reflect(lightDir, norm);
	float spec = pow(max(dot(negViewDir, reflectDir), 0.0f), max(texture(texture_specular1, fragTexCoord).x, shininess)); // Shininess get from specular map?
	vec3 specular = lightSpecStr * spec * lightColor;

	color = vec4(ambient + diffuse + specular, 1.0f) * max(texture(texture_diffuse1, fragTexCoord), vec4(baseColour, 1.0f));
	//color = vec4(ambient + diffuse + specular, 1.0f) * vec4(baseColour, 1.0f);
}