#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aTangent;
layout (location = 3) in vec2 aUV;
layout (location = 4) in vec4 aColor;


out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

uniform mat4 uModel;
uniform mat4 uBoneTransforms[100];
out vec4 ShapeColor;

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

void main()
{
    
    gl_Position = uProj * uView  * uModel  *vec4(aPos, 1.f);
    FragPos = vec3( uView * uModel * vec4(aPos, 1.f));
    TexCoords = aUV;
    Normal = normalize(mat3(transpose(inverse(uView * uModel))) * aNormal);
    ShapeColor=aColor;
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
in vec4 ShapeColor;

in vec2 TexCoords;

out vec4 FragColor;

void main()
{
    vec3 ambient = vec3(1.5, 1.5, 1.5); 
    vec3 diffuse = vec3(1.3, 1.3, 1.3);
    vec3 specular = vec3(1.3, 1.3, 1.3);
    
    
    vec3 V = normalize(uViewPos - FragPos);

    for(int i = 0; i < lightCounts.x; ++i)
    {
        vec3 L = normalize(pointLights[i].lData.position - FragPos);
        vec3 R = reflect(-L, Normal);

        diffuse += max(dot(L, Normal), 0.0) * pointLights[i].lData.diffuse;
        
        float spec = max(dot(R, V), 0.0);
        specular += pow(spec, 16.0) * pointLights[i].lData.specular;  
    }

     vec4 color = ShapeColor * (vec4(ambient, 1.0) + vec4(diffuse, 1.0) + vec4(specular, 1.0));
     FragColor = min(color, vec4(1.f, 1.f, 1.f, 1.f));
}