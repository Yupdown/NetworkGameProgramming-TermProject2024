#version 460 core

layout(location = 0) in vec3 inPosition;

uniform mat4 uModelViewProjectionMatrix;
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
    gl_Position = uModelViewProjectionMatrix * vec4(inPosition, 1.f);
    gl_Position.z = -gl_Position.z;
}


////////////////////////////////////////////////////////////////////////////////

#version 460 core

out vec4 outColor;
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
    outColor = vec4(1.0, 1.0, 1.0, 1.0);  
}