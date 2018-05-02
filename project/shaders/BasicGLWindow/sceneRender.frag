#version 330 core

in vec3 vertex;
in vec3 normal;
in vec2 UV;

in vec3 matamb;
in vec3 matdiff;
in vec3 matspec;
in float matshin;

uniform mat4 projTransform;
uniform vec2 screenResolution;

uniform int tex1Loaded;
uniform int tex2Loaded;
uniform sampler2D tex1Texture;
uniform sampler2D tex2Texture;
uniform sampler2D SSAOTex;

uniform int useSSAO;

uniform vec4 lightPos;
uniform vec3 lightCol;

out vec4 OutColor;

vec3 ambientLight = vec3(0.3, 0.3, 0.3);

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

vec3 Lambert (vec3 normal, vec3 L)
{
  // We assume that vectors are normalized

  // Color initialization with the ambient color
  vec3 resultCol = ambientLight * matamb;

  // Add the diffuse component
  if (dot (L, normal) > 0)
    resultCol += lightCol * matdiff * dot (L, normal);

  return (resultCol);
}

vec3 Phong (vec3 normal, vec3 L, vec3 vertOCS)
{
  // We assume that vectors are normalized

  // Color initialization with the Lambert color
  vec3 resultCol = Lambert (normal, L);

  // R and V computation
  if (dot(normal, L) < 0)
    // There is no specular component
    return resultCol;

  vec3 R = reflect(-L, normal); // equivalent to: normalize (2.0*dot(normal,L)*normal - L);
  vec3 V = normalize(-vertOCS);

  if ((dot(R, V) < 0) || (matshin == 0))
    // There is no specular component
    return resultCol;

  // We add the specular component
  float shine = pow(max(0.0, dot(R, V)), matshin);
  return (resultCol + matspec * lightCol * shine);
}

vec4 BlendTextures ()
{
    if (tex1Loaded == 1 && tex2Loaded == 1)
    {
	    vec4 texColor1 = texture2D(tex1Texture, UV);
	    vec4 texColor2 = texture2D(tex2Texture, UV);
	    return texColor1 * (2.f / 3.f) + texColor2 * (1.f / 3.f);
    }
    else if (tex1Loaded == 1)
    {
      return texture2D(tex1Texture, UV);
    }
    else if (tex2Loaded == 1)
    {
      return texture2D(tex2Texture, UV);
    }
    else
    {
      return vec4(1.0, 1.0, 1.0, 1.0);
    }
}

void main()
{
	vec3 n = normalize(normal);
	vec3 L = normalize(lightPos.xyz - vec3(vertex).xyz);
	vec4 col = BlendTextures();

    if(useSSAO != 0)
    {
       col *=  SmoothSSAO();
    }

	OutColor = vec4(Phong(n, L, vertex), 1) * col;
}
