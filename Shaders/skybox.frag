/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: skybox.frag
Purpose: Fragment shader for rendering skybox
Language: GLSL, Visual Studio 2019
Platform: Visual Studio 2019 v16.11.2, Windows 10
Project: haewon.shon_CS300_3
Author: Haewon Shon (haewon.shon@digipen.edu, 180002920)
Creation date: November 28, 2021
End Header --------------------------------------------------------*/

#version 400 core
layout (location = 0) out vec3 color;

in vec3 texCoord;
uniform samplerCube skyboxTexture;

//out vec4 FragColor;

void main()
{
    color = texture(skyboxTexture, texCoord.xyz).xyz;
}