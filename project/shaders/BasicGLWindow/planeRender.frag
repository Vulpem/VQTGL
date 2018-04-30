#version 330 core

in vec2 UVs;

uniform sampler2D diffuseTex;
uniform sampler2D depthTex;
uniform sampler2D normalsTex;

layout (location = 0) out vec4 FragColor;

void main()
{
    vec4 rawNormal = texture2D(normalsTex, UVs);
    vec4 rawDepth = texture2D(depthTex, UVs);

    vec3 normal = vec3(rawNormal.x * 2.f - 1.f, rawNormal.y * 2.f - 1.f, rawNormal.z * 2.f - 1.f);
    float depth = rawDepth.x;

    float SSAO = 1.f;


	FragColor = texture2D(diffuseTex, UVs) * SSAO;
}
