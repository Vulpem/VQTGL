#version 330 core

in vec2 UVs;

uniform sampler2D diffuseTex;
uniform sampler2D depthTex;
uniform sampler2D normalsTex;

layout (location = 0) out vec4 FragColor;

void main()
{
	FragColor = texture2D(diffuseTex, UVs);
}
