#version 330 core

uniform vec2 screenResolution;

uniform mat4 projectionMat;

uniform sampler2D posTex;
uniform sampler2D normalsTex;
uniform sampler2D randomTex;
uniform float sampleRadius;

uniform vec3 kernel[32];

out vec4 SSAO;

#define kernelSize 32

vec2 texCoord = vec2(gl_FragCoord.x / screenResolution.x, gl_FragCoord.y / screenResolution.y);

float CalculateSSAO()
{
     // http://john-chapman-graphics.blogspot.com.es/2013/01/ssao-tutorial.html
     const float bias = 0.025f;
     vec4 position = texture2D(posTex, texCoord);

     if(position.w >= 0.999999)
     { return 1.0f; }

    vec4 rawNormal = texture2D(normalsTex, texCoord);
    vec3 normal = vec3(rawNormal.x * 2.f - 1.f, rawNormal.y * 2.f - 1.f, rawNormal.z * 2.f - 1.f);
    normal = normalize(normal);



    vec2 noiseScale = vec2(screenResolution.x / 64.f, screenResolution.y / 64.f);
    vec3 rvec = normalize(texture(randomTex, vec2(texCoord.x * noiseScale.x, texCoord.y * noiseScale.y))).xyz * 2.0 - 1.0;

    vec3 tangent = normalize(rvec - normal * dot(rvec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 tbn = mat3(tangent, bitangent, normal);

    float occlusion = 0.0;
    for (int i = 0; i < kernelSize; ++i)
    {
        // get sample position:
        vec3 sample = tbn * kernel[i];
        sample = sample * sampleRadius + position.xyz;
        
        // project sample position:
        vec4 offset = vec4(sample, 1.0);
        offset = projectionMat * offset;
        offset.xy /= offset.w;
        offset.xy = offset.xy * 0.5 + 0.5;
        
        // get sample depth:
        float sampleDepth = texture(posTex, offset.xy).z;

        // range check & accumulate:
        float rangeCheck = smoothstep(0.f, 1.f, sampleRadius / abs(position.z - sampleDepth));
        occlusion += (sampleDepth >= sample.z + bias ? 1.0 : 0.0) * rangeCheck;
    }

     occlusion = 1- (occlusion / kernelSize);

     return occlusion;
}

void main()
{
    float val = CalculateSSAO();
    SSAO = vec4(val,val,val,1.f);
}
