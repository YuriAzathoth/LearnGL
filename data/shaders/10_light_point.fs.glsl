#version 330 core

struct Material
{
	sampler2D sDiffuse;
	sampler2D sSpecular;
	float shininess;
};

struct LightPoint
{
	vec3 position;
	vec3 diffuse;
	vec3 specular;
	float constant;
	float linear;
	float quadratic;
};

uniform Material cMaterial;
uniform LightPoint cLight;
uniform vec3 cAmbientColor;
uniform vec3 cViewPos;

in vec2 vTexCoord;
in vec3 vNormal;
in vec3 vFragPos;

out vec4 vFragColor;

void main()
{
	vec4 diffuseInput = texture(cMaterial.sDiffuse, vTexCoord);
	vec4 specularInput = texture(cMaterial.sSpecular, vTexCoord);
	
	vec3 ambient = cAmbientColor * diffuseInput.rgb;

	float distance = length(cLight.position - vFragPos);
	float attenuation = 1.0 / (cLight.constant + cLight.linear * distance + cLight.quadratic * (distance * distance));
	
	vec3 normal = normalize(vNormal);
	vec3 lightDir = normalize(cLight.position - vFragPos);
	float lightFactor = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = cLight.diffuse * (lightFactor * diffuseInput.rgb) * attenuation;

	vec3 viewDir = normalize(cViewPos - vFragPos);
	vec3 reflectDir = reflect(-lightDir, normal);
	float specularFactor = pow(max(dot(viewDir, reflectDir), 0.0), cMaterial.shininess);
	vec3 specular = cLight.diffuse * (specularFactor * specularInput.rgb) * attenuation;

	vFragColor.rgb = ambient + diffuse + specular;
	vFragColor.a = 1.0;
}
