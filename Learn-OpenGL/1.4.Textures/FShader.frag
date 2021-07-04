#version 330 core

in vec3 ourColor;
in vec2 texCoord;

out vec4 fragColor;

uniform sampler2D ourTexture;

void main()
{
    fragColor = texture(ourTexture, texCoord) * vec4(ourColor, 1.0);
}