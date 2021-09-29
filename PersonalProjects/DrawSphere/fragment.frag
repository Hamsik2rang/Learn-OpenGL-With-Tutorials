#version 330 core

out vec4 fragColor;

in vec3 ourColor;
in vec2 ourTexCoord;

uniform sampler2D ourTexture;

void main()
{
	fragColor = texture(ourTexture, ourTexCoord) * vec4(ourColor, 1.0);
	//fragColor = vec4(ourColor, 1.0);
}