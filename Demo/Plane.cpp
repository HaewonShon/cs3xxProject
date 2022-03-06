/* Start Header -------------------------------------------------------
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Plane.cpp
Purpose: Source for plane struct
Language: C++17, Visual Studio 2019
Platform: Visual Studio 2019 v16.11.2, Windows 10
Project: haewon.shon_CS350_2
Author: Haewon Shon (haewon.shon@digipen.edu, 180002920)
Creation date: March 3, 2022
End Header --------------------------------------------------------*/
#pragma once

#include "Plane.h"
#include <vector>
#include <glm/gtx/transform.hpp>

Plane::Plane() 
	: normal(glm::vec4(0.f, 1.f, 0.f, 0.f)) 
{
	Init();
}

Plane::Plane(const glm::vec4& n) 
	: normal(n) 
{
	Init();
}

Plane::Plane(float x, float y, float z, float d) 
	: normal(glm::vec4{ x, y, z, d }) 
{
	Init();
}

Plane::Plane(const glm::vec3& _normal, const glm::vec3& point)
	: normal(_normal.x, _normal.y, _normal.z, glm::dot(_normal, point)) 
{
	Init();
}

void Plane::Draw()
{
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, elementSize);
	glBindVertexArray(0);
}

void Plane::Init()
{
	glDeleteVertexArrays(1, &vao);

	constexpr float planeSize = 1000.f;
	glm::vec3 normal_v3{normal.x, normal.y, normal.z};
	glm::vec3 arbitVec{0.f, 1.f, 0.f};
	if (normal_v3.x == 0.f && normal_v3.y == 1.f && normal_v3.z == 0.f)
	{
		arbitVec.x = 1.f;
		arbitVec.y = 0.f;
	}
	glm::vec3 t1 = glm::cross(normal_v3, arbitVec);
	glm::vec3 t2 = glm::cross(normal_v3, t1);

	glm::vec3 pointOnPlane{ 0.f, 0.f, normal.w / normal.z };
	if (normal.z == 0.f)
		if (normal.y != 0.f) pointOnPlane = glm::vec3(0.f, normal.y / normal.w, 0.f);
		else pointOnPlane = glm::vec3(normal.x / normal.w, 0.f, 0.f);

	if (normal.w == 0.0f) pointOnPlane.z = 0.0f;
	glm::vec3 vertices[4] = {
		pointOnPlane + (-t1 + t2) * planeSize,
		pointOnPlane + (t1 + t2) * planeSize,
		pointOnPlane + (-t1 - t2) * planeSize,
		pointOnPlane + (t1 - t2) * planeSize,
	};

	glCreateVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint vbo;
	// vertex
	glCreateBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * 4, vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	elementSize = 4;
	type = Type::PLANE;
}