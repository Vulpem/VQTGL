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
uniform sampler2D randomTex;

layout (location = 0) out vec4 FragColor;


float CalculateSSAO()
{
     vec4 rawNormal = texture2D(normalsTex, UVs);
     vec3 normal = vec3(rawNormal.x * 2.f - 1.f, rawNormal.y * 2.f - 1.f, rawNormal.z * 2.f - 1.f);

     float depth = texture2D(depthTex, UVs).x;

     float output = 1.f;
     // -------------------------------------------------------------------------------------------------------------
     // http://john-chapman-graphics.blogspot.com.es/2013/01/ssao-tutorial.html

     const int kernelSize = 16;
     vec3 kernel[kernelSize];

     for (int i = 0; i < kernelSize; ++i)
     {
     //Generate the random vector
        vec3 randomValue = texture2D(randomTex, UVs).xyz;
        kernel[i] = vec3(
        randomValue.x * 2.f - 1.f,
        randomValue.y * 2.f - 1.f,
        randomValue.z
        );

        //Reescale it, so points are more focused near the center
        float scale = float(i) / float(kernelSize);
        scale = mix(0.1f, 1.0f, scale * scale);
        normalize(kernel[i]);
        kernel[i] *= scale;
    }

     // -------------------------------------------------------------------------------------------------------------

     return output;
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
