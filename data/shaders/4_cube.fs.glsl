#version 330 core

uniform sampler2D sTexture;

in vec2 texCoord;

void main()
{
    gl_FragColor = texture(sTexture, texCoord);
}
