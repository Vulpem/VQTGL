#version 330 core

layout (triangles) in;
layout(triangle_strip, max_vertices = 3) out;

uniform mat4 projTransform;
uniform mat4 viewTransform;
uniform mat4 sceneTransform;

in vec3 normalOCS[];
in vec2 UVOCS[];

in vec3 fmatamb[];
in vec3 fmatdiff[];
in vec3 fmatspec[];
in float fmatshin[];

out vec3 vertex;
out vec3 normal;
out vec2 UV;

out vec3 matamb;
out vec3 matdiff;
out vec3 matspec;
out float matshin;

void main()
{
	mat4 trans = viewTransform * sceneTransform;

	for(int n = 0; n < 3; n++)
	{
		normal = normalOCS[n];
		UV = UVOCS[n];
		matamb = fmatamb[n];
		matdiff = fmatdiff[n];
		matspec = fmatspec[n];
		matshin = fmatshin[n];
		vertex = vec4(trans * gl_in[n].gl_Position).xyz;
		gl_Position = projTransform * vec4(vertex,1);
		EmitVertex();
	}

    EndPrimitive();
}
