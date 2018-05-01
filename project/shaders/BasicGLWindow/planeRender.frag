#version 330 core

#define r_finalImage 0
#define r_simpleRender 1
#define r_depth 2
#define r_normals 3
#define r_ambientOcclusion 4
#define r_position 5

in vec2 UVs;

uniform int whatToDraw;
uniform float farPlane;
uniform float nearPlane;

uniform mat4 projectionMat;

uniform sampler2D diffuseTex;
uniform sampler2D depthTex;
uniform sampler2D normalsTex;
uniform sampler2D randomTex;

uniform vec3 kernel[32];

layout (location = 0) out vec4 FragColor;

#define kernelSize 32

float CalculateSSAO()
{
     // http://john-chapman-graphics.blogspot.com.es/2013/01/ssao-tutorial.html

     const float radius = 0.1f;
     const float bias = 0.025f;

    vec4 rawNormal = texture2D(normalsTex, UVs);
    vec3 normal = vec3(rawNormal.x * 2.f - 1.f, rawNormal.y * 2.f - 1.f, rawNormal.z * 2.f - 1.f);
    normal = normalize(normal);

    vec3 position = texture2D(depthTex, UVs).xyz;


    vec3 rvec = texture(randomTex, UVs /** uNoiseScale*/ ).xyz * 2.0 - 1.0;
    vec3 tangent = normalize(rvec - normal * dot(rvec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 tbn = mat3(tangent, bitangent, normal);

    float occlusion = 0.0;
    for (int i = 0; i < kernelSize; ++i)
    {
        // get sample position:
        vec3 sample = tbn * kernel[i];
        sample = sample * radius + position;
        
        // project sample position:
        vec4 offset = vec4(sample, 1.0);
        offset = projectionMat * offset;
        offset.xyz /= offset.w;
        offset.xyz = offset.xyz * 0.5 + 0.5;
        
        // get sample depth:
        float sampleDepth = texture(depthTex, offset.xy).z;

        // range check & accumulate:
        float rangeCheck = mix(0.f, 1.f, radius / abs(position.z - sampleDepth));
        occlusion += (sampleDepth >= sample.z + bias ? 1.0 : 0.0) * rangeCheck;
    }

     occlusion = 1.0 - (occlusion / kernelSize);

     return max(min(occlusion, 1.f), 0.1f);
}

void main()
{
    if(whatToDraw == r_simpleRender)
    {
        FragColor = texture2D(diffuseTex, UVs);
    }
    else if (whatToDraw == r_depth)
    {
        float depth = texture2D(depthTex, UVs).z;
        depth = depth / ((farPlane - nearPlane));
        FragColor = vec4(depth,depth,depth,1);
    }
    else if (whatToDraw == r_position)
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
