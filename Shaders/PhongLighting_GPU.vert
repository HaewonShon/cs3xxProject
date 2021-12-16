/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: PhongLighting_GPU.vert
Purpose: Vertex shader for Phong Lighting implementation with uv calculation.
Language: GLSL, Visual Studio 2019
Platform: Visual Studio 2019 v16.11.2, Windows 10
Project: haewon.shon_CS300_2
Author: Haewon Shon (haewon.shon@digipen.edu, 180002920)
Creation date: November 7, 2021
End Header --------------------------------------------------------*/

#version 400 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;

uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;

uniform int useNormalForUV;
uniform int uvType;
uniform vec3 center;

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

out vec3 outLightColor;

vec3 CalcDirectionalLight(vec3 lightDir, vec3 viewDir, int lightIndex, vec3 worldNormal, vec2 uv)
{
   // diffuse
   vec3 diffuse = max(dot(normalize(lightDir), worldNormal), 0.0) * lights[lightIndex].diffuse * texture(diffuseTexture, uv).xyz;
   // specular
   vec3 reflect = 2 * dot(worldNormal, lightDir) * worldNormal - lightDir;
   float spec = pow(max(dot(viewDir, reflect), 0.0), 32);
   vec3 specular = spec * lights[lightIndex].specular * texture(specularTexture, uv).xyz;

   return lights[lightIndex].ambient + diffuse + specular;
}

vec3 CalcPointLight(vec3 viewDir, int lightIndex, vec3 worldNormal, vec3 worldPos, vec2 uv)
{
   vec3 lightDir = normalize(lights[lightIndex].pos - worldPos);
   // diffuse
   vec3 diffuse = max(dot(normalize(lightDir), worldNormal), 0.0) * lights[lightIndex].diffuse * texture(diffuseTexture, uv).xyz;
   // specular
   vec3 reflect = 2 * dot(worldNormal, lightDir) * worldNormal - lightDir;
   float spec = pow(max(dot(viewDir, reflect), 0.0), 16);
   vec3 specular = spec * lights[lightIndex].specular * texture(specularTexture, uv).xyz;

   float distance = length(lights[lightIndex].pos - worldPos);
   float att = min(1.0 / (attenuation.x + attenuation.y * distance + attenuation.z * distance * distance), 1.0);

   return att * (lights[lightIndex].ambient + diffuse + specular);
}

vec3 CalcSpotLight(vec3 viewDir, int lightIndex, vec3 worldNormal, vec3 worldPos, vec2 uv)
{
   vec3 vecToLight = normalize(lights[lightIndex].pos - worldPos);
   vec3 lightDir = normalize(lights[lightIndex].pos);
   
   float angle = acos(dot(lightDir, vecToLight));
   if(angle <= (lights[lightIndex].innerAngle))
   {
      return CalcPointLight(viewDir, lightIndex, worldNormal, worldPos, uv);
   }
   else if(angle > (lights[lightIndex].outerAngle))
   {
      return vec3(0.0, 0.0, 0.0);
   }
   float spotLightEffect = pow( (cos(angle) - cos((lights[lightIndex].outerAngle))) 
                  / (cos((lights[lightIndex].innerAngle)) - cos((lights[lightIndex].outerAngle))), lights[lightIndex].fallOff);
   
   // diffuse
   vec3 diffuse = max(dot(normalize(vecToLight), worldNormal), 0.0) * lights[lightIndex].diffuse * texture(diffuseTexture, uv).xyz;
   // specular
   vec3 reflect = 2 * dot(worldNormal, vecToLight) * worldNormal - vecToLight;
   float spec = pow(max(dot(viewDir, reflect), 0.0), 32);
   vec3 specular = spec * lights[lightIndex].specular * texture(specularTexture, uv).xyz;

   float distance = length(lights[lightIndex].pos - worldPos);
   float att = min(1.0 / (attenuation.x + attenuation.y * distance + attenuation.z * distance * distance), 1.0);
   
   return att * (lights[lightIndex].ambient + spotLightEffect * (diffuse + specular));
}

vec2 CalcUV(vec3 entity)
{
   vec2 uv;
   switch(uvType)
   {
      case 0: // planer
      {
         vec3 absVec = vec3(abs(entity.x), abs(entity.y), abs(entity.z));
         if (absVec.x >= absVec.y && absVec.x >= absVec.z)
         {
            if (entity.x < 0.0)
            {
               uv.x = entity.z;
            }
            else
            {
               uv.x = -entity.z;
            }
            uv.y = entity.y;
         }
         else if (absVec.y >= absVec.x && absVec.y >= absVec.z)
         {
            if (entity.y < 0.0)
            {
               uv.y = entity.z;
            }
            else
            {
               uv.y = -entity.z;
            }
            uv.x = entity.x;
         }
         else if (absVec.z >= absVec.x && absVec.z >= absVec.y)
         {
            if (entity.z < 0.0)
            {
               uv.x = -entity.x;
            }
            else
            {
               uv.x = entity.x;
            }
            uv.y = entity.y;
         }
      }
      break;
      case 1:
      {
         float theta = degrees(atan(entity.z, entity.x));
         theta += 180.0;

         float y = (entity.y + 1.0) * 0.5;

         uv.x = theta / 360.0;
         uv.y = y;
      }
      break;
      case 2:
      {
         float theta = degrees(atan(entity.z, entity.x));
         theta += 180.0;

         float y = entity.y;
         float phi = degrees(acos(y / entity.length()));

         uv.x = theta / 360.0;
         uv.y = 1.0 - (phi / 180.0);
      }
      break;
   }
   return uv;
}

void main()
{
   vec3 worldNormal = normalize(mat3(transpose(inverse(modelMatrix))) * normal);  
   vec3 worldPos = vec4(modelMatrix * vec4(pos, 1.0)).xyz;

   vec3 viewDir = normalize(camPosition - worldPos);

   vec2 uv;
   if(useNormalForUV == 0)
   {
      uv = CalcUV(pos - center);
   }
   else
   {
      uv = CalcUV(normal);
   }

   vec3 c = vec3(0.0, 0.0, 0.0);
   for(int i = 0; i < lightCount; ++i)
   {
      switch(lights[i].type)
      {
         case 0:
         c += CalcPointLight(viewDir, i, worldNormal, worldPos, uv);
         break;
         case 1:
         c += CalcDirectionalLight(normalize(lights[i].pos), viewDir, i, worldNormal, uv);
         break;
         case 2:
         c += CalcSpotLight(viewDir, i, worldNormal, worldPos, uv);
         break;
      }
   }

   float S = max((fog_near_far.y - distance(camPosition, worldPos)) / (fog_near_far.y - fog_near_far.x), 0.0);
   vec3 final = S * (globalAmbient * ambientCoefficient + c) + (1.0-S) * fog;
   outLightColor = final;

   gl_Position = viewMatrix * vec4(worldPos, 1.0);
}