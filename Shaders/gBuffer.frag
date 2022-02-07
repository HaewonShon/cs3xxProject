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

#version 430 core
layout (location = 0) out vec3 pos;
layout (location = 1) out vec3 normal;
layout (location = 2) out vec3 depth;
layout (location = 3) out vec4 albedoSpec;

in vec3 fragPos;
in vec3 fragNormal;
in vec2 fragUV;

uniform sampler2D texture_diffuse1; // 0
uniform sampler2D texture_specular1; // 1


void main()
{    
   pos = fragPos;
   normal = fragNormal;
   float dist = length(fragPos);
   depth = vec3(dist);
    
   albedoSpec.xyz = texture(texture_diffuse1, fragUV).xyz;
   albedoSpec.w = texture(texture_specular1, fragUV).x;
}  