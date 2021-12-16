/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: PhongShading_GPU.vert with uv calculation
Purpose: Vertex shader for Phong Shading implementation.
Language: GLSL, Visual Studio 2019
Platform: Visual Studio 2019 v16.11.2, Windows 10
Project: haewon.shon_CS300_2
Author: Haewon Shon (haewon.shon@digipen.edu, 180002920)
Creation date: November 4, 2021
End Header --------------------------------------------------------*/

#version 400 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;

out vec3 worldNormal;
out vec3 worldPos;
out vec2 textureUV;

uniform int useNormalForUV;
uniform int uvType;
uniform vec3 center;

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
   worldNormal = normalize(mat3(transpose(inverse(modelMatrix))) * normal);  
   worldPos = vec4(modelMatrix * vec4(pos, 1.0)).xyz;

   if(useNormalForUV == 0)
   {
      textureUV = CalcUV(pos - center);
   }
   else
   {
      textureUV = CalcUV(normal);
   }


   gl_Position = viewMatrix * vec4(worldPos, 1.0);
}