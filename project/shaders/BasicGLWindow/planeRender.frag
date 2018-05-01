#version 330 core

#define r_finalImage 0
#define r_simpleRender 1
#define r_depth 2
#define r_normals 3
#define r_ambientOcclusion 4

in vec2 UVs;

uniform int whatToDraw;
uniform float farPlane;
uniform float nearPlane;

uniform mat4 projectionMat;

uniform sampler2D diffuseTex;
uniform sampler2D depthTex;
uniform sampler2D normalsTex;
uniform sampler2D randomTex;

layout (location = 0) out vec4 FragColor;

/*vec3 PositionFromDepth_DarkPhoton(in float depth)
{
  vec2 ndc;             // Reconstructed NDC-space position
  vec3 eye;             // Reconstructed EYE-space position
 
  eye.z = near * far / ((depth * (far - near)) - far);
 
  ndc.x = ((gl_FragCoord.x * widthInv) - 0.5) * 2.0;
  ndc.y = ((gl_FragCoord.y * heightInv) - 0.5) * 2.0;
 
  eye.x = (-ndc.x * eye.z) * right/near;
  eye.y = (-ndc.y * eye.z) * top/near;
 
  return eye;
}*/

float CalculateSSAO()
{
     // -------------------------------------------------------------------------------------------------------------
     // http://john-chapman-graphics.blogspot.com.es/2013/01/ssao-tutorial.html

     const int kernelSize = 64;
     const float radius = 1.f;
     const float bias = 0.025f;

     vec3 kernel[kernelSize];

     for (int i = 0; i < kernelSize; ++i)
     {
     //Generate the random vector
        vec3 randomValue = texture2D(randomTex, UVs * (i)).xyz;
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

    vec4 rawNormal = texture2D(normalsTex, UVs);
    vec3 normal = vec3(rawNormal.x * 2.f - 1.f, rawNormal.y * 2.f - 1.f, rawNormal.z * 2.f - 1.f);
    normal = normalize(normal);

    //float depth = texture2D(depthTex, UVs).z;
    //vec3 origin = vec3(0, 0, depth /** (farPlane - nearPlane) + nearPlane*/);
    vec3 origin = texture2D(depthTex, UVs).xyz;

    vec3 rvec = texture(randomTex, UVs /** uNoiseScale*/ ).xyz * 2.0 - 1.0;
    vec3 tangent = normalize(rvec - normal * dot(rvec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 tbn = mat3(tangent, bitangent, normal);

    float occlusion = 0.0;
    for (int i = 0; i < kernelSize; ++i)
    {
        // get sample position:
        vec3 sample = tbn * kernel[i];
        sample = sample * radius + origin;
        
        // project sample position:
        vec4 offset = vec4(sample, 1.0);
        offset = projectionMat * offset;
        offset.xy /= offset.w;
        offset.xy = offset.xy * 0.5 + 0.5;
        
        // get sample depth:
        float sampleDepth = texture(depthTex, offset.xy).z;

        // range check & accumulate:
        //float rangeCheck= abs(origin.z - sampleDepth) < radius ? 1.0 : 0.0;
        //occlusion += (sampleDepth <= sample.z ? 1.0 : 0.0) * rangeCheck;

        float rangeCheck = mix(0.f, 1.f, radius / abs(origin.z - sampleDepth));
        occlusion += (sampleDepth >= sample.z + bias ? 1.0 : 0.0) * rangeCheck;
    }

    occlusion = 1.0 - (occlusion / kernelSize);
     // -------------------------------------------------------------------------------------------------------------

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
    //TODO draw position or depth
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
