#version 330 core

in vec3 normal;
in vec2 UV;
in vec3 color;

out vec4 FragColor;

uniform int tex1Loaded;
uniform int tex2Loaded;
uniform sampler2D tex1Texture;
uniform sampler2D tex2Texture;

void main()
{
   if (tex1Loaded == 1 && tex2Loaded == 1)
    {
	    vec4 texColor1 = texture2D(tex1Texture, UV);
	    vec4 texColor2 = texture2D(tex2Texture, UV);
	    FragColor = texColor1 * (2.f / 3.f) + texColor2 * (1.f / 3.f);
    }
    else if (tex1Loaded == 1)
    {
      FragColor = texture2D(tex1Texture, UV);
    }
    else if (tex2Loaded == 1)
    {
      FragColor = texture2D(tex2Texture, UV);
    }
    else
    {
      FragColor = vec4(color, 1.0);
    }
}
