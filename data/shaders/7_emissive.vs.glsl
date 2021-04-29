#version 330 core

layout (location = 0) in vec3 aPos;

uniform mat4 cModel;
uniform mat4 cViewProj;

void main()
{
	gl_Position = cViewProj * cModel * vec4(aPos, 1.0);
}
