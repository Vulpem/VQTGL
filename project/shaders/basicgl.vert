#version 330 core

layout (location = 1) in vec3 vertex;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec2 UV;
layout (location = 4) in vec3 color;

// Observer Coordinate System
out vec3 normalOCS;
out vec2 UVOCS;
out vec3 vertexColor;

void main()
{
    normalOCS = normal;
    UVOCS = UV;
    vertexColor = color;
    gl_Position = vec4(vertex, 1.0f);
}
