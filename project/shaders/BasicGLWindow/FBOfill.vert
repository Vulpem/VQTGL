#version 330 core

in vec3 vertex;
in vec3 normal;

// Observer Coordinate System
out vec3 normalOCS;

void main()
{
    normalOCS = normal;

	gl_Position = vec4(vertex, 1.0f);
}
