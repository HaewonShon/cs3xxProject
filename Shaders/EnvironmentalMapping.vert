/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: EnvironmentalMapping.vert
Purpose: Vertex shader for environment mapping + phong shading.
Language: GLSL, Visual Studio 2019
Platform: Visual Studio 2019 v16.11.2, Windows 10
Project: haewon.shon_CS300_3
Author: Haewon Shon (haewon.shon@digipen.edu, 180002920)
Creation date: December 2, 2021
End Header --------------------------------------------------------*/

#version 400 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;

out vec3 worldNormal;
out vec3 worldPos;

void main()
{
   worldPos = vec4(modelMatrix * vec4(aPos, 1.0)).xyz;
   worldNormal = normalize(mat3(transpose(inverse(modelMatrix))) * aNormal); 
   gl_Position = viewMatrix * vec4(worldPos, 1.0);
}