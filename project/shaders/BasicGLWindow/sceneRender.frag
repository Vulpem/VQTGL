#version 330 core

in vec3 vertex;
in vec3 normal;
in vec2 UV;

in vec3 matamb;
in vec3 matdiff;
in vec3 matspec;
in float matshin;

uniform mat4 projTransform;

uniform float farPlane;
uniform float nearPlane;

uniform int tex1Loaded;
uniform int tex2Loaded;
uniform sampler2D tex1Texture;
uniform sampler2D tex2Texture;

uniform vec4 lightPos;
uniform vec3 lightCol;


layout (location = 0) out vec4 FragColorRT0;
layout (location = 1) out vec4 FragColorDepth;
layout (location = 2) out vec4 FragColorNormals;

vec3 ambientLight = vec3(0.3, 0.3, 0.3);

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

	FragColorRT0 = vec4(Phong(n, L, vertex), 1) * col;

	float depthColor = (gl_FragCoord.z / gl_FragCoord.w) / (farPlane - nearPlane);
	FragColorDepth = vec4(depthColor,depthColor,depthColor,1);

    vec3 norm = n * mat3(projTransform);

	FragColorNormals = vec4(norm.x / 2.0 + 0.5f, norm.y / 2.0 + 0.5f, norm.z / 2.0 + 0.5f, 1);

		//Debug
	//FragColorRT0 = FragColorDepth;
	//FragColorRT0 = FragColorNormals;
}
