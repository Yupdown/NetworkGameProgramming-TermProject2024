#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aTangent;
layout (location = 3) in vec2 aUV;
layout (location = 4) in vec4 aColor;
layout (location = 5) in ivec4 aBoneId;
layout (location = 6) in vec4 aWeight;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

struct LightData
{
	vec3 position;
	float pad1; 
	vec3 ambient;
	float pad2;
	vec3 diffuse;
	float pad3;
	vec3 specular;
	float pad4;
	vec3 lightDir;
	float pad5;
};

struct PointLightData
{
	LightData lData;
	float constant;
	float linear;
	float quadratic;
};

struct SpotLightData
{
	LightData lData;
	float constant;
	float linear;
	float quadratic;
	float cutOff;
	float outerCutoff;
};

struct DirectionalLightData
{
	LightData lData;
};

layout (std140, binding = 0) uniform UBOData
{
    mat4 uProj;
    mat4 uView;
    vec3 uViewPos;
	float pad5;
	PointLightData pointLights[20];
	SpotLightData spotLights[20];
	DirectionalLightData dirLight;
	mat4 uObserverView;
    vec3 uObserverPos;
	float pad6;
	ivec4 lightCounts;
};
uniform mat4 uModel;
uniform mat4 uBoneTransforms[100];

void main()
{
    mat4 totalMat=mat4(0.f);
    vec4 totalPos =vec4(0,0,0,0);
    for(int i=0;i<4;++i)
    {
        if(aBoneId[i] == -1)continue;
        totalMat += uBoneTransforms[aBoneId[i]] * aWeight[i];
        //vec4 localPos = uBoneTransforms[aBoneId[i]] * aWeight[i] *vec4(aPos, 1.f);
        //totalPos +=localPos;
    }

    gl_Position = uProj * uView  * uModel * totalMat *vec4(aPos, 1.f);
    FragPos = vec3( uView * uModel * vec4(aPos, 1.f));
    TexCoords = aUV;
    Normal = normalize(mat3(transpose(inverse(uView * uModel))) * aNormal);
}


////////////////////////////////////////////////////////////////////////////////

#version 460 core

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct LightData
{
	vec3 position;
	float pad1; 
	vec3 ambient;
	float pad2;
	vec3 diffuse;
	float pad3;
	vec3 specular;
	float pad4;
	vec3 lightDir;
	float pad5;
};

struct PointLightData
{
	LightData lData;
	float constant;
	float linear;
	float quadratic;
};

struct SpotLightData
{
	LightData lData;
	float constant;
	float linear;
	float quadratic;
	float cutOff;
	float outerCutoff;
};

struct DirectionalLightData
{
	LightData lData;
};

layout (std140, binding = 0) uniform UBOData
{
    mat4 uProj;
    mat4 uView;
    vec3 uViewPos;
	float pad5;
	PointLightData pointLights[20];
	SpotLightData spotLights[20];
	DirectionalLightData dirLight;
	mat4 uObserverView;
    vec3 uObserverPos;
	float pad6;
	ivec4 lightCounts;
};

uniform Material material;

//uniform samplerCube uCubeTexture[50];
//uniform sampler2D uTexture2D[50];

uniform samplerCube uCubeTexture;
uniform sampler2D uTexture2D;
uniform sampler2D uNormalTexture;

//uniform int uCubeTexIdx;
//uniform int uTex2DIdx;

in vec3 FragPos;
in vec3 Normal;

in vec2 TexCoords;

out vec4 FragColor;

void main()
{
    vec3 ambient = vec3(0.0, 0.0, 0.0);
    vec3 diffuse = vec3(0.0, 0.0, 0.0);
    vec3 specular = vec3(0.0, 0.0, 0.0);
    vec3 V = vec3(0.f,0.f,1.f);

   //for(int i = 0; i < lightCounts.x; ++i)
   //{
   //    vec3 L = normalize(lights[i].position - FragPos);
   //    vec3 R = reflect(-L, Normal);
   //
   //    ambient += lights[i].ambient * material.ambient;
   //   
   //    diffuse += max(dot(L, Normal), 0.0) * lights[i].diffuse  * material.diffuse;
   //        
   //    specular += pow(max(dot(R, V), 0.0), material.shininess) * lights[i].specular * material.specular;
   //}
   //
   //vec4 color = vec4((ambient + diffuse + specular),1.f);
   //
   //FragColor = min(1.5 * color * texture( uTexture2D, TexCoords ), vec4(1.f, 1.f, 1.f, 1.f));
}