/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: skybox.vert
Purpose: Vertex shader for rendering skybox
Language: GLSL, Visual Studio 2019
Platform: Visual Studio 2019 v16.11.2, Windows 10
Project: haewon.shon_CS300_3
Author: Haewon Shon (haewon.shon@digipen.edu, 180002920)
Creation date: November 28, 2021
End Header --------------------------------------------------------*/

#version 400 core
layout (location = 0) in vec3 aPos;

uniform mat4 viewMatrix;

out vec3 texCoord;

void main()
{
    vec3 worldPos = aPos - 1.0; // from 0~2 to -1~1
    texCoord = worldPos;
    gl_Position = viewMatrix * vec4(worldPos, 1.0);
}