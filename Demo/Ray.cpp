/* Start Header -------------------------------------------------------
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Ray.cpp
Purpose: Source for Ray
Language: C++17, Visual Studio 2019
Platform: Visual Studio 2019 v16.11.2, Windows 10
Project: haewon.shon_CS350_2
Author: Haewon Shon (haewon.shon@digipen.edu, 180002920)
Creation date: Feb 27, 2022
End Header --------------------------------------------------------*/
#pragma once

#include "Ray.h"
#include <vector>

Ray::Ray()
	: start({0.f}), direction({1.f, 0.f, 0.f})
{
	Init();
}

Ray::Ray(const glm::vec3& start, const glm::vec3& dir)
	: start(start), direction(dir)
{
	Init();
}

Ray::~Ray()
{
	glDeleteVertexArrays(1, &vao);
}

void Ray::Draw()
{
	glBindVertexArray(vao);
	glDrawElements(GL_LINES, elementSize, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Ray::Init()
{
	glDeleteVertexArrays(1, &vao);

	std::vector<glm::vec3> vertices;
	vertices.push_back(start);
	vertices.push_back(start + direction * 10000.f);

	std::vector<unsigned int> indices;
	indices.push_back(0);
	indices.push_back(1);

	glCreateVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint vbo, ebo;
	// vertex
	glCreateBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);

	// ebo
	glCreateBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_STATIC_DRAW);
	glVertexArrayElementBuffer(vao, ebo);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	elementSize = 2;
	type = Type::RAY;
}