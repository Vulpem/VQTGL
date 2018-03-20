#version 330 core

in vec4 vertexOCS;
in vec3 normalOCS;
in vec3 fmatamb;
in vec3 fmatdiff;
in vec3 fmatspec;
in float fmatshin;

uniform vec4 lightPos;
uniform vec3 lightCol;

out vec4 FragColor;

void main()
{
  FragColor = vec4(0.0, 1.0, 0.0, 1.0);
}
