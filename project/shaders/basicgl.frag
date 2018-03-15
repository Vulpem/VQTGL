#version 330 core

in vec4 vertexOCS;
in vec3 normalOCS;
in vec2 UVOCS;
in vec3 color;

out vec4 FragColor;

void main()
{
   FragColor = vec4(color,1);
}
