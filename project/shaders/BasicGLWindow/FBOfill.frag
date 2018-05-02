#version 330 core

in vec3 vertex;
in vec3 normal;

uniform mat4 projTransform;

layout (location = 0) out vec4 FragColorDepth;
layout (location = 1) out vec4 FragColorNormals;

void main()
{
	vec3 n = normalize(normal);

	float depthColor = (gl_FragCoord.z / gl_FragCoord.w);// / (farPlane - nearPlane);
	FragColorDepth = vec4(vertex.xy, depthColor, 1.f);

    vec3 norm = n * mat3(projTransform);
	FragColorNormals = vec4(norm.x / 2.0 + 0.5f, norm.y / 2.0 + 0.5f, norm.z / 2.0 + 0.5f, 1);
}
