#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

uniform mat4 cModel;
uniform mat4 cViewProj;

out vec2 texCoord;

void main()
{
	texCoord = aTexCoord;
	gl_Position = cViewProj * cModel * vec4(aPos, 1.0);
}
