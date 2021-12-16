/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: CalcNormal.geom
Purpose: Geometry shader for rendering normal vectors with proper length
Language: GLSL, Visual Studio 2019
Platform: Visual Studio 2019 v16.11.2, Windows 10
Project: haewon.shon_CS300_3
Author: Haewon Shon (haewon.shon@digipen.edu, 180002920)
Creation date: December 2, 2021
End Header --------------------------------------------------------*/

#version 400 core
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

out vec3 worldNormal;
out vec3 worldPos;


uniform mat4 viewMatrix;

void main()
{
    vec3 v1 = gl_in[2].gl_Position.xyz - gl_in[1].gl_Position.xyz;
    vec3 v2 = gl_in[0].gl_Position.xyz - gl_in[1].gl_Position.xyz;
    worldNormal = normalize(cross(v1, v2));

    for(int i = 0; i < 3; ++i)
    {
        worldPos = gl_in[i].gl_Position.xyz;
        gl_Position = viewMatrix * gl_in[i].gl_Position;
        EmitVertex();
    }

    EndPrimitive();
}  