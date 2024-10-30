#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aTangent;
layout (location = 3) in vec2 aUV;
layout (location = 4) in vec4 aColor;
layout (location = 5) in mat4 instanceModelMatrix;

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
    gl_Position = uProj * uView * instanceModelMatrix * vec4(aPos, 1.f);
    FragPos = vec3(uView * instanceModelMatrix * vec4(aPos, 1.f));
    TexCoords = aUV;
    Normal = normalize(mat3(transpose(inverse(uView * instanceModelMatrix))) * aNormal);
    ShapeColor=aColor;
}


////////////////////////////////////////////////////////////////////////////////

#version 460 core

struct Material 
{
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



uniform sampler2D uTexture2D;


in vec3 FragPos;
in vec3 Normal;
in vec4 ShapeColor;

in vec2 TexCoords;

out vec4 FragColor;


vec3 ReflectVector(vec3 lightDir, vec3 normal) {
    return reflect(-lightDir, normal);
}


float CalculateAttenuation(float distance, float constant, float linear, float quadratic) {
    return 1.0 / (constant + linear * distance + quadratic * distance * distance);
}


vec3 CalculateLightDirection(vec3 lightPos, vec3 fragPos) {
    return normalize(lightPos - fragPos);
}

vec3 CalculateViewDirection(vec3 viewPos, vec3 fragPos) {
    return normalize(viewPos - fragPos);
}

vec3 CalculatePointLight(PointLightData light, vec3 normal, vec3 fragPos,vec3 viewpos);
vec3 CalculateSpotLight(SpotLightData light, vec3 normal, vec3 fragPos,vec3 viewpos) ;
vec3 CalculateDirectionalLight(DirectionalLightData light, vec3 normal, vec3 fragPos,vec3 viewpos) ;

vec3 defaultAmbient = vec3(.1, .1, .1);
uniform Material material;
const float refractiveIndex = 1.33; 
const vec4 glassColor = vec4(1.0, 1.0, 1.0, 0.05); 

//vec2 getEnvMapCoord(vec3 viewDir, vec3 normal) {
//    vec3 refractDir = refract(-viewDir, normalize(normal), 1.0 / refractiveIndex);
//    float m = 2.0 * sqrt(dot(refractDir.xy, refractDir.xy) + pow(refractDir.z + 1.0, 2.0));
//    return refractDir.xy / m + 0.5;
//}
vec2 getEnvMapCoord(vec3 viewDir, vec3 normal) {
    vec3 refractDir = refract(-viewDir, normalize(normal), 1.0 / refractiveIndex);
    float m = 2.0 * sqrt(dot(refractDir.xy, refractDir.xy) + pow(refractDir.z + 1.0, 2.0));
    vec2 coord = refractDir.xy / m + 0.5;

  
    coord.y = 1.0 - coord.y; // 노말 정상화 되면 바꿔야함

    return coord;
}
void main()
{
    vec3 viewDir = normalize(FragPos - uViewPos);
    vec2 envCoord = getEnvMapCoord(viewDir, Normal);

    vec3 envColor = texture(uTexture2D, envCoord).rgb;

    vec3 lightingColor = vec3(0.0, 0.0, 0.0);
    vec3 normal = -Normal;
    vec3 fragPos = FragPos;

   
    for (int i = 0; i < lightCounts.x; ++i)
    {
        lightingColor += CalculatePointLight(pointLights[i], normal, fragPos, vec3(0,0,0));
    }

   
    for (int i = 0; i < lightCounts.y; ++i)
    {
        lightingColor += CalculateSpotLight(spotLights[i], normal, fragPos, vec3(0,0,0));
    }

  
    if (lightCounts.z > 0) 
    {
        lightingColor += CalculateDirectionalLight(dirLight, normal, fragPos, vec3(0,0,0));
    }

    vec3 finalColor = mix(envColor, lightingColor, 0.5); 
    finalColor = mix(finalColor, glassColor.rgb, glassColor.a); 

    FragColor = vec4(finalColor, 0.1);
    
    
}
vec3 CalculatePointLight(PointLightData light, vec3 normal, vec3 fragPos,vec3 viewpos) 
{
    vec3 lightDir = CalculateLightDirection(vec3(uObserverView * vec4(light.lData.position,1.f)), fragPos);
    vec3 viewDir = CalculateViewDirection(viewpos, fragPos);
    vec3 reflectDir = ReflectVector(lightDir, normal);

    
    float smoothDiffuse = max(dot(normal, lightDir), 0.0);

 
    vec3 ambient = light.lData.ambient * material.ambient;

   
    vec3 diffuse = light.lData.diffuse * smoothDiffuse * material.diffuse;

   
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.lData.specular * spec * material.specular * smoothDiffuse;

  
    float distance = length(vec3(uObserverView * vec4(light.lData.position,1.f)) - fragPos);
    float attenuation = CalculateAttenuation(distance, light.constant, light.linear, light.quadratic);

    return (ambient + diffuse + specular) * attenuation + defaultAmbient;
}

vec3 CalculateSpotLight(SpotLightData light, vec3 normal, vec3 fragPos, vec3 viewPos)
{
    vec3 lightPosCamSpace = vec3(uObserverView * vec4(light.lData.position, 1.0));
    vec3 lightDir = CalculateLightDirection(lightPosCamSpace, fragPos);
    vec3 viewDir = CalculateViewDirection(viewPos, fragPos);
    vec3 reflectDir = ReflectVector(lightDir, normal);
    float distance = length(lightPosCamSpace - fragPos);

   
    float smoothDiffuse = max(dot(normal, lightDir), 0.0);

   
    float maxOuterCutoff = 90.0; 
    float maxDistance = 500.0; 
    float adjustedOuterCutoff = mix(light.outerCutoff, maxOuterCutoff, smoothstep(0.0, maxDistance, distance));

    float attenuation = CalculateAttenuation(distance, light.constant, light.linear, light.quadratic);

 
    vec3 diffuse = light.lData.diffuse * smoothDiffuse * material.diffuse;

   
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.lData.specular * spec * material.specular * smoothDiffuse;

   
    vec3 lightDirCamSpace = normalize(vec3(uObserverView * vec4(light.lData.lightDir, 0.0)));
    float theta = dot(lightDir, lightDirCamSpace);
    float epsilon = adjustedOuterCutoff - light.cutOff;
    float intensity = clamp((theta - light.cutOff) / epsilon, 0.0, 1.0);

   
    vec3 ambient = light.lData.ambient * material.ambient;

    return (ambient + diffuse + specular) * intensity * attenuation;
}

vec3 CalculateDirectionalLight(DirectionalLightData light, vec3 normal, vec3 fragPos,vec3 viewpos)
{
    vec3 lightDir = normalize(vec3(uObserverView * vec4(light.lData.lightDir,0)));
    vec3 viewDir = CalculateViewDirection(viewpos, fragPos);
    vec3 reflectDir = ReflectVector(lightDir, normal);

  
    vec3 ambient = light.lData.ambient * material.ambient;

    
    float diff = max(dot(normal, lightDir), 0.0);
    if(diff <= 0.0)
    {
    return ambient + defaultAmbient;
    }
    vec3 diffuse = light.lData.diffuse * diff * material.diffuse;

   
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.lData.specular * spec * material.specular;

    return ambient + diffuse + specular + defaultAmbient;
}