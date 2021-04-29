#version 330 core

uniform sampler2D sTexture;

uniform vec3 cAmbientColor;
uniform vec3 cLightPos;
uniform vec3 cLightColor;
uniform vec3 cViewPos;

in vec2 vTexCoord;
in vec3 vNormal;
in vec3 vFragPos;

out vec4 vFragColor;

const float SPECULAR_STRENGTH = 0.5f;

void main()
{
	vec4 textureDiffuse = texture(sTexture, vTexCoord);
	
	vec3 normal = normalize(vNormal);
	vec3 lightDir = normalize(cLightPos - vFragPos);
	float lightFactor = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = cLightColor * lightFactor;
	
	vec3 viewDir = normalize(cViewPos - vFragPos);
	vec3 reflectDir = reflect(-lightDir, normal);
	float specularFactor = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3 specular = SPECULAR_STRENGTH * specularFactor * cLightColor;

    vFragColor = textureDiffuse * vec4(cAmbientColor + diffuse + specular, 1.0);
}
