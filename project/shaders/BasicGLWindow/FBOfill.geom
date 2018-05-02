#version 330 core

layout (triangles) in;
layout(triangle_strip, max_vertices = 3) out;

uniform mat4 projTransform;
uniform mat4 viewTransform;
uniform mat4 sceneTransform;

in vec3 normalOCS[];

out vec3 vertex;
out vec3 normal;

void main()
{
	mat4 trans = viewTransform * sceneTransform;
	mat3 normalMatrix = inverse(transpose(mat3 (viewTransform * sceneTransform)));

	for(int n = 0; n < 3; n++)
	{
		normal = normalize(vec3(normalMatrix * normalOCS[n]));
		vertex = vec4(trans * gl_in[n].gl_Position).xyz;
		gl_Position = projTransform * vec4(vertex,1);
		EmitVertex();
	}

    EndPrimitive();
}
