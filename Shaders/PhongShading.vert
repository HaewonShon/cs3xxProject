/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: PhongShading.vert
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
layout (location = 2) in vec2 uv;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;

out vec3 worldNormal;
out vec3 worldPos;
out vec2 textureUV;

void main()
{
   worldNormal = normalize(mat3(transpose(inverse(modelMatrix))) * normal);  
   worldPos = vec4(modelMatrix * vec4(pos, 1.0)).xyz;
   textureUV = uv;
   gl_Position = viewMatrix * vec4(worldPos, 1.0);
}