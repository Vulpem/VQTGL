#version 330 core

layout (triangles) in;
layout(triangle_strip, max_vertices = 3) out;

uniform mat4 projTransform;
uniform mat4 viewTransform;
uniform mat4 sceneTransform;

in vec3 vertexColor[];

out vec3 color;

void main()
{
	mat4 trans = projTransform * viewTransform * sceneTransform;

	color = vertexColor[0];
	gl_Position = trans * gl_in[0].gl_Position;
    EmitVertex();

	color = vertexColor[1];
	gl_Position = trans * gl_in[1].gl_Position;
    EmitVertex();

	color = vertexColor[2];
	gl_Position = trans * gl_in[2].gl_Position;
    EmitVertex();

    EndPrimitive();
}
