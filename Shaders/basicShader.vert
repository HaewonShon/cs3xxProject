/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: basicShader.vert
Purpose: Vertex shader for rendering object without any effect.
Language: GLSL, Visual Studio 2019
Platform: Visual Studio 2019 v16.11.2, Windows 10
Project: haewon.shon_CS300_1
Author: Haewon Shon (haewon.shon@digipen.edu, 180002920)
Creation date: September 7, 2021
End Header --------------------------------------------------------*/

#version 400 core
layout (location = 0) in vec3 aPos;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;

void main()
{
   vec4 worldPos = modelMatrix * vec4(aPos.x, aPos.y, aPos.z, 1.0);
   gl_Position = viewMatrix * worldPos;
}