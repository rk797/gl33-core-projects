#version 330 core
layout (location = 0) in vec3 positionIn;
layout (location = 1) in vec2 uvIn;

out vec2 uvOut;

void main() {
	uvOut = uvIn;
	gl_Position = vec4(positionIn, 1.0);
}
