#version 330 core

in vec3 vertex;
in vec3 normal;
in vec2 UV;

in vec3 matamb;
in vec3 matdiff;
in vec3 matspec;
in float matshin;

// Observer Coordinate System
out vec3 normalOCS;
out vec2 UVOCS;

out vec3 fmatamb;
out vec3 fmatdiff;
out vec3 fmatspec;
out float fmatshin;

void main()
{
    normalOCS = normal;
    UVOCS = UV;

	fmatamb = matamb;
    fmatdiff = matdiff;
    fmatspec = matspec;
    fmatshin = matshin;

	gl_Position = vec4(vertex, 1.0f);
}
