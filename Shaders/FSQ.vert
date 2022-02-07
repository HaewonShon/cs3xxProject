/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: gBuffer.vert
Purpose: Vertex shader for feeding G buffer.
Language: GLSL, Visual Studio 2019
Platform: Visual Studio 2019 v16.11.2, Windows 10
Project: haewon.shon_CS350_1
Author: Haewon Shon (haewon.shon@digipen.edu, 180002920)
Creation date: January 30th, 2022
End Header --------------------------------------------------------*/

#version 400 core
layout (location = 0) in vec3 pos;
layout (location = 2) in vec2 uv;

out vec2 uvs;

void main()
{
    uvs = uv;
    gl_Position = vec4(pos, 1.0);
}