#version 330 core

in vec4 vertexOCS;
in vec3 normalOCS;
in vec4 vertexColor;
in vec2 vertexTexCoords;

out vec4 FragColor;

uniform int tex1Loaded;
uniform int tex2Loaded;
uniform sampler2D tex1Texture;
uniform sampler2D tex2Texture;
uniform vec4 lightPos;
uniform vec3 lightCol;

void main()
{
    if (tex1Loaded == 1 && tex2Loaded == 1)
    {
	vec4 texColor1 = texture2D(tex1Texture, vertexTexCoords);
	vec4 texColor2 = texture2D(tex2Texture, vertexTexCoords);
	FragColor = texColor1 * (2.f / 3.f) + texColor2 * (1.f / 3.f);
    }
    else if (tex1Loaded == 1)
      FragColor = texture2D(tex1Texture, vertexTexCoords);
    else if (tex2Loaded == 1)
      FragColor = texture2D(tex2Texture, vertexTexCoords);
    else
      FragColor = vertexColor;
}
