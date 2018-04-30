#version 330 core

#define r_finalImage 0
#define r_simpleRender 1
#define r_depth 2
#define r_normals 3
#define r_ambientOcclusion 4

in vec2 UVs;

uniform int whatToDraw;

uniform sampler2D diffuseTex;
uniform sampler2D depthTex;
uniform sampler2D normalsTex;

layout (location = 0) out vec4 FragColor;

float CalculateSSAO()
{
     vec4 rawNormal = texture2D(normalsTex, UVs);
     vec4 rawDepth = texture2D(depthTex, UVs);

     vec3 normal = vec3(rawNormal.x * 2.f - 1.f, rawNormal.y * 2.f - 1.f, rawNormal.z * 2.f - 1.f);
     float depth = rawDepth.x;

     return 1.f;
}

void main()
{
    if(whatToDraw == r_simpleRender)
    {
        FragColor = texture2D(diffuseTex, UVs);
    }
    else if (whatToDraw == r_depth)
    {
        FragColor = texture2D(depthTex, UVs);
    }
    else if (whatToDraw == r_normals)
    {
        FragColor = texture2D(normalsTex, UVs);
    }
    else
    {
        float SSAO = CalculateSSAO();

        if(whatToDraw == r_finalImage)
        {
	        FragColor = texture2D(diffuseTex, UVs) * SSAO;
        }
        else if (whatToDraw == r_ambientOcclusion)
        {
            FragColor = vec4(SSAO,SSAO,SSAO,1.f);
        }
    }
}
