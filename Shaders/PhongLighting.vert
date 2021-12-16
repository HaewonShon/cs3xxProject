/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: PhongLighting.vert
Purpose: Vertex shader for Phong Lighting implementation.
Language: GLSL, Visual Studio 2019
Platform: Visual Studio 2019 v16.11.2, Windows 10
Project: haewon.shon_CS300_2
Author: Haewon Shon (haewon.shon@digipen.edu, 180002920)
Creation date: November 4, 2021
End Header --------------------------------------------------------*/

#version 400 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;

uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;

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

vec3 CalcDirectionalLight(vec3 lightDir, vec3 viewDir, int lightIndex, vec3 worldNormal)
{
   // diffuse
   vec3 diffuse = max(dot(normalize(lightDir), worldNormal), 0.0) * lights[lightIndex].diffuse * texture(diffuseTexture, uv).xyz;
   // specular
   vec3 reflect = 2 * dot(worldNormal, lightDir) * worldNormal - lightDir;
   float spec = pow(max(dot(viewDir, reflect), 0.0), 32);
   vec3 specular = spec * lights[lightIndex].specular * texture(specularTexture, uv).xyz;

   return lights[lightIndex].ambient + diffuse + specular;
}

vec3 CalcPointLight(vec3 viewDir, int lightIndex, vec3 worldNormal, vec3 worldPos)
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

vec3 CalcSpotLight(vec3 viewDir, int lightIndex, vec3 worldNormal, vec3 worldPos)
{
   vec3 vecToLight = normalize(lights[lightIndex].pos - worldPos);
   vec3 lightDir = normalize(lights[lightIndex].pos);
   
   float angle = acos(dot(lightDir, vecToLight));
   if(angle <= (lights[lightIndex].innerAngle))
   {
      return CalcPointLight(viewDir, lightIndex, worldNormal, worldPos);
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

void main()
{
   vec3 worldNormal = normalize(mat3(transpose(inverse(modelMatrix))) * normal);  
   vec3 worldPos = vec4(modelMatrix * vec4(pos, 1.0)).xyz;

   vec3 viewDir = normalize(camPosition - worldPos);

   vec3 c = vec3(0.0, 0.0, 0.0);
   for(int i = 0; i < lightCount; ++i)
   {
      switch(lights[i].type)
      {
         case 0:
         c += CalcPointLight(viewDir, i, worldNormal, worldPos);
         break;
         case 1:
         c += CalcDirectionalLight(normalize(lights[i].pos), viewDir, i, worldNormal);
         break;
         case 2:
         c += CalcSpotLight(viewDir, i, worldNormal, worldPos);
         break;
      }
   }

   float S = max((fog_near_far.y - distance(camPosition, worldPos)) / (fog_near_far.y - fog_near_far.x), 0.0);
   vec3 final = S * (globalAmbient * ambientCoefficient + c) + (1.0-S) * fog;
   outLightColor = final;

   gl_Position = viewMatrix * vec4(worldPos, 1.0);
}