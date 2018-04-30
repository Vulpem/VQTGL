#version 330 core

in vec3 vertex;
in vec2 UV;

out vec2 UVs;

void main()
{
    UVs = UV;
	gl_Position = vec4(vertex, 1.0f);
}
