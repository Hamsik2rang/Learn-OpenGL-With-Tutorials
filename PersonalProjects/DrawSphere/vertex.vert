#version 330 core

out vec3 ourColor;
out vec2 ourTexCoord;

layout (location = 0) 
in vec3 aPos;

layout (location = 1)
in vec2 aTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0);

	ourTexCoord = aTexCoord;
	// White
	ourColor = vec3(1.0, 1.0, 1.0);
}