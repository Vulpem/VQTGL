#version 330 core

layout (triangles) in;
layout(triangle_strip, max_vertices = 3) out;

uniform mat4 projTransform;
uniform mat4 viewTransform;
uniform mat4 sceneTransform;

in vec3 vertexColor[];
in vec3 normalOCS[];
in vec2 UVOCS[];

out vec3 color;
out vec3 normal;
out vec2 UV;

void main()
{
	mat4 trans = projTransform * viewTransform * sceneTransform;

	color = vertexColor[0];
    normal = normalOCS[0];
    UV = UVOCS[0];
	gl_Position = trans * gl_in[0].gl_Position;
    EmitVertex();

	color = vertexColor[1];
    normal = normalOCS[1];
    UV = UVOCS[1];
	gl_Position = trans * gl_in[1].gl_Position;
    EmitVertex();

	color = vertexColor[2];
    normal = normalOCS[2];
    UV = UVOCS[2];
	gl_Position = trans * gl_in[2].gl_Position;
    EmitVertex();

    EndPrimitive();
}
