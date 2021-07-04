#version 330 core

out vec2 texCoord;

layout (location = 0) in vec3 aPos;

layout (location = 1) in vec2 aTexCoord;

uniform mat4 transform;

void main()
{
    gl_Position = transform * vec4(aPos, 1.0);
    texCoord = aTexCoord;
}