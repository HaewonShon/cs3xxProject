/* Start Header -------------------------------------------------------
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Triangle.cpp
Purpose: Source for 3D point struct
Language: C++17, Visual Studio 2019
Platform: Visual Studio 2019 v16.11.2, Windows 10
Project: haewon.shon_CS350_2
Author: Haewon Shon (haewon.shon@digipen.edu, 180002920)
Creation date: Feb 27, 2022
End Header --------------------------------------------------------*/
#pragma once

#include "Triangle.h"
#include <vector>

Triangle::Triangle(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3) 
	: v1(v1), v2(v2), v3(v3) 
{
	Init();
}

Triangle::Triangle(const Point3D& v1, const Point3D& v2, const Point3D& v3) 
	: v1(v1), v2(v2), v3(v3) 
{
	Init();
}

void Triangle::Draw()
{
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, elementSize);
	glBindVertexArray(0);
}

void Triangle::Init()
{
	glDeleteVertexArrays(1, &vao);

	std::vector<glm::vec3> vertices;
	vertices.push_back(v1.coordinate);
	vertices.push_back(v2.coordinate);
	vertices.push_back(v3.coordinate);
	// for back
	vertices.push_back(v3.coordinate);
	vertices.push_back(v2.coordinate);
	vertices.push_back(v1.coordinate);

	glCreateVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint vbo;
	// vertex
	glCreateBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * 6, vertices.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	elementSize = 6;
	type = Type::TRIANGLE;
}