#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormals;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 cViewProj;
uniform mat4 cModel;
uniform mat4 cModelInv;

out vec2 vTexCoord;
out vec3 vNormal;
out vec3 vFragPos;

void main()
{
	vTexCoord = aTexCoord;
	vNormal = mat3(cModelInv) * aNormals;
	vFragPos = (cModel * vec4(aPos, 1.0)).xyz;
	gl_Position = cViewProj * cModel * vec4(aPos, 1.0);
}
