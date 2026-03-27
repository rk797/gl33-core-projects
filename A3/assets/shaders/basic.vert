#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 color;

out vec3 fragColor;

uniform mat4 view;
uniform mat4 projection;

void main() 
{
	// projection * view * model
    gl_Position = projection * view * vec4(pos, 1.0);
    fragColor = color;
}