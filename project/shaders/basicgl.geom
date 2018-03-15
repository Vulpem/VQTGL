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
	vec4 pos;
	mat4 trans = projTransform * viewTransform * sceneTransform;
	color = vertexColor[0];
    pos = gl_in[0].gl_Position + vec4(0.0, 0.0, 0.0, 0.0);
	gl_Position = trans * pos;
    EmitVertex();

	color = vertexColor[1];
    pos = gl_in[1].gl_Position + vec4(0.0, 0.0, 0.0, 0.0);
	gl_Position = trans * pos;
    EmitVertex();

	color = vertexColor[2];
	pos = gl_in[2].gl_Position + vec4(0.0, 0.0, 0.0, 0.0);
	gl_Position = trans * pos;
    EmitVertex();

    EndPrimitive();
}
