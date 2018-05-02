#version 330 core

#define r_finalImage 0
#define r_simpleRender 1
#define r_depth 2
#define r_normals 3
#define r_ambientOcclusion 4
#define r_position 5
#define r_blurredAO 6

uniform int whatToDraw;
uniform float farPlane;
uniform float nearPlane;
uniform vec2 screenResolution;

uniform sampler2D diffuseTex;
uniform sampler2D posTex;
uniform sampler2D normalsTex;
uniform sampler2D randomTex;
uniform sampler2D SSAOTex;

layout (location = 0) out vec4 FragColor;

vec2 texCoord = vec2(gl_FragCoord.x / screenResolution.x, gl_FragCoord.y / screenResolution.y);

float SmoothSSAO()
{
    vec2 ssaoUV = vec2(gl_FragCoord.x / screenResolution.x, gl_FragCoord.y / screenResolution.y);
    vec2 texelSize = 1.0 / screenResolution;
    float result = 0.0;
    for (int x = -2; x <= 2; ++x) 
    {
        for (int y = -2; y <= 2; ++y) 
        {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            result += texture2D(SSAOTex, ssaoUV + offset).r;
        }
    }
    return result / (5.0 * 5.0);
}

void main()
{
    if(whatToDraw == r_simpleRender)
    {
        FragColor = texture2D(diffuseTex, texCoord);
    }
    else if (whatToDraw == r_depth)
    {
        float depth = texture2D(posTex, texCoord).a / (farPlane - nearPlane);
        FragColor = vec4(depth,depth,depth,1);
    }
    else if (whatToDraw == r_position)
    {
        FragColor = texture2D(posTex, texCoord);
    }
    else if (whatToDraw == r_normals)
    {
        FragColor = texture2D(normalsTex, texCoord);
    }
    else if (whatToDraw == r_ambientOcclusion)
    {
        float SSAO = texture2D(SSAOTex, texCoord).x;
        FragColor = vec4(SSAO,SSAO,SSAO,1.f);
    }
    else if(whatToDraw == r_finalImage)
    {
        FragColor = texture2D(diffuseTex, texCoord);// * SSAO;
    }
    else if (whatToDraw == r_blurredAO)
    {
        float SSAO = SmoothSSAO();
        FragColor = vec4(SSAO,SSAO,SSAO,1.f);
    }
}
