#version 330 core

out vec4 color;

in vec2 uvOut;

uniform sampler2D sampler;

void main() {
	vec4 sampledColor = texture(sampler, uvOut);
	if(sampledColor.a < 1e-4)
	{
		discard;// If the texture is transparent, don't draw the fragment
	}
	color = sampledColor;
}
