#version 330 core

out vec4 fragColor;

in vec2 texCoord;

uniform sampler2D ourTexture1;
uniform sampler2D ourTexture2;

void main()
{
    fragColor = texture(ourTexture1, texCoord);
}