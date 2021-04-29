#version 330 core

uniform vec3 cColor;

out vec4 FragColor;

void main()
{
    FragColor = vec4(cColor, 1.0);
}
