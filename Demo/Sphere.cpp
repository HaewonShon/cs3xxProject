/* Start Header -------------------------------------------------------
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Sphere.cpp
Purpose: Source for Bounding sphere
Language: C++17, Visual Studio 2019
Platform: Visual Studio 2019 v16.11.2, Windows 10
Project: haewon.shon_CS350_2
Author: Haewon Shon (haewon.shon@digipen.edu, 180002920)
Creation date: Feb 27, 2022
End Header --------------------------------------------------------*/
#pragma once

#include "Sphere.h"
#include <vector>

Sphere::Sphere()
	: position(Point3D(0.f, 0.f, 0.f)), radius(1.f)
{
	Init();
}

Sphere::Sphere(const glm::vec3& pos, float r)
	: position(Point3D(pos)), radius(r)
{
	Init();
}

Sphere::Sphere(const Point3D& p, float r)
	: position(p), radius(r)
{
	Init();
}

Sphere::~Sphere()
{
	glDeleteVertexArrays(1, &vao);
}

void Sphere::Draw()
{
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, elementSize, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Sphere::Init()
{
	glDeleteVertexArrays(1, &vao);

	const float PI = 3.14159265359f;
	const float PI_over_8 = PI / 8.f;

	std::vector<glm::vec3> vertices;
	vertices.push_back(this->position.coordinate + glm::vec3{ 0.f, radius, 0.f });

	for (float pi = PI_over_8; pi < PI; pi += PI_over_8)
	{
		float y = std::cos(pi);
		float radius = std::sin(pi);
		for (float theta = 0; theta < 2 * PI; theta += PI_over_8)
		{
			float x = radius * std::sin(theta);
			float z = radius * std::cos(theta);
			vertices.push_back(this->position.coordinate + glm::vec3{ x, y, z });
		}
	}
	vertices.push_back(this->position.coordinate + glm::vec3{ 0.f, -radius, 0.f });

	std::vector<unsigned int> indices;

	for (int i = 1; i <= 15; ++i)
	{
		indices.push_back(0);
		indices.push_back(i);
		indices.push_back(i + 1);
	}
	indices.push_back(0);
	indices.push_back(16);
	indices.push_back(1);

	for (int i = 1; i <= 97; i += 16)
	{
		for (int j = 0; j < 15; ++j)
		{
			indices.push_back(i + j);
			indices.push_back(i + j + 1);
			indices.push_back(i + j + 16);
			indices.push_back(i + j + 16);
			indices.push_back(i + j + 17);
			indices.push_back(i + j + 1);
		}
		indices.push_back(i + 15);
		indices.push_back(i);
		indices.push_back(i + 31);
		indices.push_back(i + 31);
		indices.push_back(i + 16);
		indices.push_back(i);
	}

	for (int i = 1; i <= 16; ++i)
	{
		indices.push_back(129);
		indices.push_back(112 + i);
		indices.push_back(112 + i + 1);
	}
	indices.push_back(129);
	indices.push_back(128);
	indices.push_back(113);
	
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

	elementSize = static_cast<GLsizei>(indices.size());
	type = Type::SPHERE;
}