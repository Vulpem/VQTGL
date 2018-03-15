#version 330 core

layout (triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vec3 vertexColor[];

out vec3 color;

void main()
{
	color = vertexColor[0];

    gl_Position = gl_in[0].gl_Position + vec4(0.0, 0.0, 0.0, 0.0);
    EmitVertex();

	color = vertexColor[1];
    gl_Position = gl_in[1].gl_Position + vec4(0.0, 0.0, 0.0, 0.0);
    EmitVertex();

	color = vertexColor[2];
	gl_Position = gl_in[2].gl_Position + vec4(0.0, 0.0, 0.0, 0.0);
    EmitVertex();

    EndPrimitive();
}
