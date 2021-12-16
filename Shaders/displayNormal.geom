/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: displayNormal.geom
Purpose: Geometry shader for rendering normal vectors with proper length
Language: GLSL, Visual Studio 2019
Platform: Visual Studio 2019 v16.11.2, Windows 10
Project: haewon.shon_CS300_1
Author: Haewon Shon (haewon.shon@digipen.edu, 180002920)
Creation date: September 29, 2021
End Header --------------------------------------------------------*/

#version 400 core
layout(lines) in;
layout(line_strip, max_vertices = 2) out;

uniform mat4 viewMatrix;
uniform float normalLength;

void main()
{
   vec4 center = gl_in[0].gl_Position;
   vec4 normal = gl_in[1].gl_Position - gl_in[0].gl_Position;

   gl_Position = viewMatrix * center;
   EmitVertex();
   gl_Position = viewMatrix * (center + normal * normalLength);
   EmitVertex();

   EndPrimitive();
}  