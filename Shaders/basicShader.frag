/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: basicShader.frag
Purpose: Fragment shader for rendering object without any effect.
Language: GLSL, Visual Studio 2019
Platform: Visual Studio 2019 v16.11.2, Windows 10
Project: haewon.shon_CS300_1
Author: Haewon Shon (haewon.shon@digipen.edu, 180002920)
Creation date: September 7, 2021
End Header --------------------------------------------------------*/

#version 400 core

out vec4 color;
uniform vec3 inputColor;

void main()
{
   color = vec4(inputColor, 1.0); //vec4(uvs, 0.0, 1.0);
}