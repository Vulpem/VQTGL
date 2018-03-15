#version 330 core

layout (location = 1) in vec3 vertex;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec2 UV;
layout (location = 4) in vec3 color;

uniform mat4 projTransform;
uniform mat4 viewTransform;
uniform mat4 sceneTransform;

// Observer Coordinate System
out vec4 vertexOCS;
out vec3 normalOCS;
out vec2 UVOCS;
out vec3 vertexColor;

void main()
{
    normalOCS = normal;
    UVOCS = UV;
    vertexOCS = viewTransform * sceneTransform * vec4(vertex, 1);
    vertexColor = color;
    gl_Position = projTransform * vertexOCS;

}
