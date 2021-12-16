/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: EnvironmentalMapping.frag
Purpose: Fragment shader for environment mapping + phong shading.
Language: GLSL, Visual Studio 2019
Platform: Visual Studio 2019 v16.11.2, Windows 10
Project: haewon.shon_CS300_3
Author: Haewon Shon (haewon.shon@digipen.edu, 180002920)
Creation date: December 2, 2021
End Header --------------------------------------------------------*/

#version 400 core

struct Light
{
   int type; // 4
   // spot light
   float innerAngle; // 4
   float outerAngle; // 4
   float fallOff; // 4
   vec3 ambient; // 16
   vec3 diffuse; // 16
   vec3 specular; // 16
   vec3 pos; // 16
}; // total 80 - alignmnent

layout (std140) uniform LightInfo
{
   Light lights[16]; // 80 * 16 = 1280
   int lightCount; // 16 alignment
   vec3 camPosition; // 16
   vec3 ambientCoefficient; // 16
   vec3 globalAmbient; // 16
   vec3 attenuation; // 16
   vec3 fog; // 16
   vec2 fog_near_far;
};

uniform sampler2D left;
uniform sampler2D right;
uniform sampler2D bottom;
uniform sampler2D top;
uniform sampler2D front;
uniform sampler2D back;

uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform int currentProjectMode;

//uniform vec3 cameraPos;
in vec3 worldPos;
in vec3 worldNormal;
in vec3 viewDir;

uniform int mode;
uniform float refractionIndex;
uniform float fresnelPower;
uniform int useChromaticAberration;
uniform float chromaticMultiplier;

// phong
uniform bool usePhongShading;
uniform int projectionMode;

const float refraction_R = 0.985; // 0.65 / 0.66
const float refraction_G = 1.015; // 0.67 / 0.66
const float refraction_B = 1.045; // 0.69 / 0.66

out vec4 outColor;

vec4 GetColorFromEnvMap(vec3 entity)
{
    vec3 absVec = abs(entity);
    vec2 uv = vec2(0.0);

    if(absVec.x >= absVec.y && absVec.x >= absVec.z)
    {
        uv.y = entity.y / absVec.x;
        if(entity.x < 0.0) // left
        {
            uv.x = entity.z / absVec.x;
            return texture(right, vec2(1.0) - (uv + vec2(1.0)) * 0.5);
        }
        else // right
        {
            uv.x = -entity.z / absVec.x;
            return texture(left, vec2(1.0) - (uv + vec2(1.0)) * 0.5);
        }
    }
    else if(absVec.y >= absVec.x && absVec.y >= absVec.z)
    {
        uv.x = entity.x / absVec.y;
        if(entity.y < 0.0) // top? texture problem
        {
            uv.y = entity.z / absVec.y;
            return texture(top, vec2(1.0) - (uv + vec2(1.0)) * 0.5);
        }
        else // bottom
        {
            uv.y = -entity.z / absVec.y;
            return texture(bottom, vec2(1.0) - (uv + vec2(1.0)) * 0.5);
        }
    }
    else //if(absVec.z >= absVec.x && absVec.z >= absVec.y)
    {
        uv.y = entity.y / absVec.z;
        if(entity.z < 0.0) // back
        {
            uv.x = -entity.x / absVec.z;
            return texture(back, vec2(1.0) - (uv + vec2(1.0)) * 0.5);
        }
        else // front
        {
            uv.x = entity.x / absVec.z;
            return texture(front, vec2(1.0) - (uv + vec2(1.0)) * 0.5);
        }
    }
    return vec4(vec2(1.0)-(uv + vec2(1.0))*0.5, 0.0, 1.0);
    //return vec4(0.0);
}

vec3 refractVector(vec3 incident, vec3 normal, float K)
{
    float N_dot_View = dot(normal, incident);
    vec3 refraction = vec3(0.0);
    float part = 1.0 - K*K * (1.0 - N_dot_View * N_dot_View);
    if(part >= 0.0)
    {
        refraction = (K*N_dot_View + sqrt(part)) * normal - K * incident;
    }
    return refraction;
}

vec3 CalcDirectionalLight(vec3 lightDir, vec3 viewDir, int lightIndex, vec3 worldNormal, vec2 textureUV)
{
   // diffuse
   vec3 diffuse = max(dot(normalize(lightDir), worldNormal), 0.0) * lights[lightIndex].diffuse * texture(diffuseTexture, textureUV).xyz;
   // specular
   vec3 reflect = 2 * dot(worldNormal, lightDir) * worldNormal - lightDir;
   float spec = pow(max(dot(viewDir, reflect), 0.0), 32);
   vec3 specular = spec * lights[lightIndex].specular * texture(specularTexture, textureUV).xyz;

   return lights[lightIndex].ambient + diffuse + specular;
}

vec3 CalcPointLight(vec3 viewDir, int lightIndex, vec3 worldNormal, vec3 worldPos, vec2 textureUV)
{
   vec3 lightDir = normalize(lights[lightIndex].pos - worldPos);
   // diffuse
   vec3 diffuse = max(dot(normalize(lightDir), worldNormal), 0.0) * lights[lightIndex].diffuse * texture(diffuseTexture, textureUV).xyz;
   // specular
   vec3 reflect = 2 * dot(worldNormal, lightDir) * worldNormal - lightDir;
   float spec = pow(max(dot(viewDir, reflect), 0.0), 32);
   vec3 specular = spec * lights[lightIndex].specular * texture(specularTexture, textureUV).xyz;

   float distance = length(lights[lightIndex].pos - worldPos);
   float att = min(1.0 / (attenuation.x + attenuation.y * distance + attenuation.z * distance * distance), 1.0);

   return att * (lights[lightIndex].ambient + diffuse + specular);
}

vec3 CalcSpotLight(vec3 viewDir, int lightIndex, vec3 worldNormal, vec3 worldPos, vec2 textureUV)
{
   vec3 vecToLight = normalize(lights[lightIndex].pos - worldPos);
   vec3 lightDir = normalize(lights[lightIndex].pos);
   
   float angle = acos(dot(lightDir, vecToLight));
   if(angle <= radians(lights[lightIndex].innerAngle))
   {
      return CalcPointLight(viewDir, lightIndex, worldNormal, worldPos, textureUV);
   }
   else if(angle > radians(lights[lightIndex].outerAngle))
   {
      return vec3(0.0, 0.0, 0.0);
   }
   float spotLightEffect = pow( (cos(angle) - cos(radians(lights[lightIndex].outerAngle))) 
                  / (cos(radians(lights[lightIndex].innerAngle)) - cos(radians(lights[lightIndex].outerAngle))), lights[lightIndex].fallOff);
   
   // diffuse
   vec3 diffuse = max(dot(normalize(vecToLight), worldNormal), 0.0) * lights[lightIndex].diffuse * texture(diffuseTexture, textureUV).xyz;
   // specular
   vec3 reflect = 2 * dot(worldNormal, vecToLight) * worldNormal - vecToLight;
   float spec = pow(max(dot(viewDir, reflect), 0.0), 32);
   vec3 specular = spec * lights[lightIndex].specular * texture(specularTexture, textureUV).xyz;

   float distance = length(lights[lightIndex].pos - worldPos);
   float att = min(1.0 / (attenuation.x + attenuation.y * distance + attenuation.z * distance * distance), 1.0);
   
   return att * (lights[lightIndex].ambient + spotLightEffect * (diffuse + specular));
}

vec2 CubeMapping(vec3 entity)
{
    vec2 uv = vec2(0.0);
    vec3 absVec = abs(entity);

    if(absVec.x >= absVec.y && absVec.x >= absVec.z)
    {
        if(entity.x < 0.0) uv.x = entity.z;
        else uv.x = -entity.z;
        uv.y = entity.y;
    }
    else if(absVec.y >= absVec.z && absVec.y >= absVec.x)
    {
        if(entity.y < 0.0) uv.x = entity.z;
        else uv.x = -entity.z;
        uv.y = entity.x;
    }
    else
    {
        if(entity.z < 0.0) uv.x = -entity.x;
        else uv.x = entity.x;
        uv.y = entity.y;
    }
    
    uv = (uv + vec2(1.0)) * 0.5;
    return uv;
}

vec2 SphericalMapping(vec3 entity)
{
    float theta = degrees(atan(entity.z, entity.x)) + 180.0;
	float phi = degrees(acos(entity.y / length(entity)));
    vec2 uv;
    uv.x = theta / 360.0;
    uv.y = 1.0 - (phi / 180.0);
    return uv;
}

vec2 CylindricalMapping(vec3 entity)
{
    float theta = degrees(atan(entity.z, entity.x) + 180.0);
    vec2 uv;
    uv.x = theta / 360.0;
    uv.y = (entity.y + 1.0) * 0.5;
    return uv;
}
  

vec3 CalcPhong(vec3 viewDir, vec3 normal)
{
    vec3 phongColor;

    vec2 uv;
    switch(projectionMode)
    {
        case 0: // planer
        uv = CubeMapping(worldPos);
        break;
        case 1: // spherical
        uv = SphericalMapping(worldPos);
        break;
        case 2: // cylindrical
        uv = CylindricalMapping(worldPos);
        break;
    }

    for(int i = 0; i < lightCount; ++i)
    {
        switch(lights[i].type)
        {
            case 0:
            phongColor += CalcPointLight(viewDir, i, normal, worldPos, uv);
            break;
            case 1:
            phongColor += CalcDirectionalLight(normalize(lights[i].pos), viewDir, i, normal, uv);
            break;
            case 2:
            phongColor += CalcSpotLight(viewDir, i, normal, worldPos, uv);
            break;
        }
    }

    float S = max((fog_near_far.y - distance(camPosition, worldPos)) / (fog_near_far.y - fog_near_far.x), 0.0);
    phongColor = S * (globalAmbient * ambientCoefficient + phongColor) + (1.0-S) * fog;
    return phongColor;
}

void main()
{
    vec3 viewDir = normalize(worldPos - camPosition);
    vec3 worldNormal2 = normalize(worldNormal);
    float K = 1.0 / refractionIndex;

    // reflection
    vec3 reflection = (2 * dot(worldNormal2, viewDir) * worldNormal2 - viewDir);

    // refraction

    switch(mode)
    {
        case 0: // reflection only
            outColor = GetColorFromEnvMap(reflection);
        break;
        case 1: // refraction only
            if(useChromaticAberration == 0)
            {
                outColor = GetColorFromEnvMap(refractVector(viewDir, worldNormal2, K));
            }
            else
            {
                outColor.x = GetColorFromEnvMap(refractVector(viewDir, worldNormal2, K * refraction_R * chromaticMultiplier)).x;
                outColor.y = GetColorFromEnvMap(refractVector(viewDir, worldNormal2, K * refraction_G * chromaticMultiplier)).y;
                outColor.z = GetColorFromEnvMap(refractVector(viewDir, worldNormal2, K * refraction_B * chromaticMultiplier)).z;
                outColor.w = 1.0;
            }
        break;
        case 2: // fresnel termw
            float f = (1.0 - K) * (1.0 - K) / ((1.0 + K) * (1.0 + K));
            float F = f + (1.0-f) * pow((1.0 - dot(worldNormal2, -viewDir)), fresnelPower);
            vec3 reflectColor = GetColorFromEnvMap(reflection).xyz;
            vec3 refractColor;
            if(useChromaticAberration == 0)
            {
                refractColor = GetColorFromEnvMap(refractVector(viewDir, worldNormal2, K)).xyz;
            }
            else
            {
                refractColor.x = GetColorFromEnvMap(refractVector(viewDir, worldNormal2, K * refraction_R * chromaticMultiplier)).x;
                refractColor.y = GetColorFromEnvMap(refractVector(viewDir, worldNormal2, K * refraction_G * chromaticMultiplier)).y;
                refractColor.z = GetColorFromEnvMap(refractVector(viewDir, worldNormal2, K * refraction_B * chromaticMultiplier)).z;
            }
            outColor = vec4(mix(refractColor, reflectColor, F), 1.0);
        break;
    }

    if(usePhongShading == true)
    {
        outColor = vec4(mix(outColor.xyz, CalcPhong(-viewDir, worldNormal2), 0.5), 1.0);
        //outColor = vec4(CalcPhong(-viewDir, worldNormal2), 1.0);
    }
}