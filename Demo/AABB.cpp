/* Start Header -------------------------------------------------------
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: AABB.cpp
Purpose: Source for AABB
Language: C++17, Visual Studio 2019
Platform: Visual Studio 2019 v16.11.2, Windows 10
Project: haewon.shon_CS350_2
Author: Haewon Shon (haewon.shon@digipen.edu, 180002920)
Creation date: Feb 26, 2022
End Header --------------------------------------------------------*/
#pragma once

#include "AABB.h"
#include <vector>

AABB::AABB()
	: min(glm::vec3(-0.5f)), max(glm::vec3(0.5f))
{
	Init();
}

AABB::AABB(const glm::vec3& min, const glm::vec3& max)
	: min(min), max(max)
{
	Init();
}

AABB::~AABB()
{
	glDeleteVertexArrays(1, &vao);
}

void AABB::Draw()
{
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, elementSize, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void AABB::Init()
{
	glDeleteVertexArrays(1, &vao);

	glm::vec3 vertices[8] = {
		glm::vec3{min.x, min.y, min.z},
		glm::vec3{min.x, min.y, max.z},
		glm::vec3{min.x, max.y, min.z},
		glm::vec3{min.x, max.y, max.z},
		glm::vec3{max.x, min.y, min.z},
		glm::vec3{max.x, min.y, max.z},
		glm::vec3{max.x, max.y, min.z},
		glm::vec3{max.x, max.y, max.z},
	};

	std::vector<unsigned int> indices;
	indices.push_back(0);	indices.push_back(1);	indices.push_back(3);
	indices.push_back(0);	indices.push_back(3);	indices.push_back(1);

	indices.push_back(5);	indices.push_back(7);	indices.push_back(3);
	indices.push_back(5);	indices.push_back(3);	indices.push_back(1);

	indices.push_back(4);	indices.push_back(6);	indices.push_back(7);
	indices.push_back(4);	indices.push_back(7);	indices.push_back(5);

	indices.push_back(0);	indices.push_back(2);	indices.push_back(6);
	indices.push_back(0);	indices.push_back(6);	indices.push_back(4);

	indices.push_back(2);	indices.push_back(3);	indices.push_back(7);
	indices.push_back(2);	indices.push_back(7);	indices.push_back(6);

	indices.push_back(0);	indices.push_back(4);	indices.push_back(5);
	indices.push_back(0);	indices.push_back(5);	indices.push_back(1);

	glCreateVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint vbo, ebo;
	// vertex
	glCreateBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * 8, vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);

	// ebo
	glCreateBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_STATIC_DRAW);
	glVertexArrayElementBuffer(vao, ebo);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	elementSize = indices.size();
	type = Type::AABB;
}