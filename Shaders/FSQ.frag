/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: gBuffer.frag
Purpose: Fragment shader for feeding G buffer.
Language: GLSL, Visual Studio 2019
Platform: Visual Studio 2019 v16.11.2, Windows 10
Project: haewon.shon_CS350_1
Author: Haewon Shon (haewon.shon@digipen.edu, 180002920)
Creation date: January 30th, 2022
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

layout (location = 0) out vec4 color;
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

////////// PHONG SHADING ///////////

uniform sampler2D gPosition; // 0
uniform sampler2D gNormal; // 1
uniform sampler2D gAlbedoSpec; // 2
uniform sampler2D gDepth; // 3
uniform mat4 viewMatrix;
in vec2 uvs;

uniform bool isDrawingBuffer;
uniform int bufferRenderTarget;

////////// DEFERRED SHADING /////////

vec3 CalcPointLight(vec3 fragPos, int lightIndex, vec3 normal, vec4 albedoSpec)
{
   vec3 lightPos = vec4(viewMatrix * vec4(lights[lightIndex].pos, 1.0)).xyz;
   vec3 lightDir = normalize(lightPos - fragPos);
   // diffuse
   vec3 diffuse = max(dot(normalize(lightDir), normal), 0.0) * lights[lightIndex].diffuse * albedoSpec.xyz;
   // specular
   vec3 reflect = 2 * dot(normal, lightDir) * normal - lightDir;
   float spec = pow(max(dot(normalize(-fragPos), reflect), 0.0), 32);
   vec3 specular = spec * lights[lightIndex].specular * albedoSpec.w;

   float distance = length(lightPos - fragPos);
   float att = min(1.0 / (attenuation.x + attenuation.y * distance + attenuation.z * distance * distance), 1.0);

   return att * (lights[lightIndex].ambient + diffuse + specular);
}

vec3 CalcDirectionalLight(vec3 fragPos, int lightIndex, vec3 normal, vec4 albedoSpec)
{
   vec3 lightPos = vec4(viewMatrix * vec4(lights[lightIndex].pos, 1.0)).xyz;
   vec3 lightDir = normalize(lightPos - fragPos);
   // diffuse
   vec3 diffuse = max(dot(normalize(lightDir), normal), 0.0) * lights[lightIndex].diffuse * albedoSpec.xyz;
   // specular
   vec3 reflect = 2 * dot(normal, lightDir) * normal - lightDir;
   float spec = pow(max(dot(normalize(-fragPos), reflect), 0.0), 32);
   vec3 specular = spec * lights[lightIndex].specular * albedoSpec.w;

   return lights[lightIndex].ambient + diffuse + specular;
}

vec3 CalcSpotLight(vec3 fragPos, int lightIndex, vec3 normal, vec4 albedoSpec)
{
   vec3 lightPos = vec4(viewMatrix * vec4(lights[lightIndex].pos, 1.0)).xyz;
   vec3 lightDir = normalize(lightPos - fragPos);

   vec3 vecToLight = normalize(lightPos - vec4(viewMatrix * vec4(0.0, 0.0, 0.0, 1.0)).xyz);
   
   float angle = acos(dot(lightDir, vecToLight));
   if(angle <= radians(lights[lightIndex].innerAngle))
   {
      return CalcPointLight(fragPos, lightIndex, normal, albedoSpec);
   }
   else if(angle > radians(lights[lightIndex].outerAngle))
   {
      return vec3(0.0, 0.0, 0.0);
   }
   float spotLightEffect = pow((cos(angle) - cos(radians(lights[lightIndex].outerAngle))) 
                  / (cos(radians(lights[lightIndex].innerAngle)) - cos(radians(lights[lightIndex].outerAngle))), lights[lightIndex].fallOff);
   
   // diffuse
   vec3 diffuse = max(dot(normalize(lightDir), normal), 0.0) * lights[lightIndex].diffuse * albedoSpec.xyz;
   // specular
   vec3 reflect = 2 * dot(normal, lightDir) * normal - lightDir;
   float spec = pow(max(dot(normalize(-fragPos), reflect), 0.0), 32);
   vec3 specular = spec * lights[lightIndex].specular * albedoSpec.w;

   float distance = length(lightPos - fragPos);
   float att = min(1.0 / (attenuation.x + attenuation.y * distance + attenuation.z * distance * distance), 1.0);
   
   return att * (lights[lightIndex].ambient + spotLightEffect * (diffuse + specular));
}
//////// LIGHT FUNCTIONS /////////

void main()
{    
   if(isDrawingBuffer == true)
   {
      switch(bufferRenderTarget)
      {
      case 0:
         color = vec4(texture(gPosition, uvs).xyz, 1.0);
         break;
      case 1:
         color = vec4(texture(gNormal, uvs).xyz, 1.0);
         break;
      case 2:
         color = vec4(texture(gAlbedoSpec, uvs).xyz, 1.0);
         break;
      case 3:
         color = vec4(texture(gDepth, uvs).xyz, 1.0);
         break;
      }
   }
   else
   {
      vec3 c = vec3(0.0);
      vec3 _pos = texture(gPosition, uvs).xyz;
      vec3 _normal = texture(gNormal, uvs).xyz;
      if(_normal == vec3(0.0)) return;
      vec4 _albedoSpec = texture(gAlbedoSpec, uvs);
      for(int i = 0; i < lightCount; ++i)
      {
         switch(lights[i].type)
         {
            case 0:
            c += CalcPointLight(_pos, i, _normal, _albedoSpec);
            break;
            case 1:
            c += CalcDirectionalLight(_pos, i, _normal, _albedoSpec);
            break;
            case 2:
            c += CalcSpotLight(_pos, i, _normal, _albedoSpec);
            break;
         }
      }

      float S = max((fog_near_far.y - length(_pos)) / (fog_near_far.y - fog_near_far.x), 0.0);
      vec3 final = S * (globalAmbient * ambientCoefficient + c) + (1.0-S) * fog;
      color = vec4(final, 1.0);
   }
}  