/* Start Header -------------------------------------------------------
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Point3D.cpp
Purpose: Source for 3D point struct
Language: C++17, Visual Studio 2019
Platform: Visual Studio 2019 v16.11.2, Windows 10
Project: haewon.shon_CS350_2
Author: Haewon Shon (haewon.shon@digipen.edu, 180002920)
Creation date: Feb 27, 2022
End Header --------------------------------------------------------*/
#pragma once

#pragma once

#include "Point3D.h"

Point3D::Point3D()
	: coordinate(glm::vec3(0.f)), size(3.f)
{
	Init();
}

Point3D::Point3D(const glm::vec3& c, float size)
	: coordinate(c), size(size)
{
	Init();
}

Point3D::Point3D(float x, float y, float z, float size)
	: Point3D(glm::vec3{ x, y, z }, size) {}

Point3D::~Point3D()
{
	glDeleteVertexArrays(1, &vao);
}

void Point3D::Draw()
{
	glBindVertexArray(vao);
	glPointSize(size);
	glDrawArrays(GL_POINTS, 0, elementSize);
	glBindVertexArray(0);
}

void Point3D::Init()
{
	glDeleteVertexArrays(1, &vao);
	
	glCreateVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint vbo;
	// vertex
	glCreateBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3), &coordinate, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	elementSize = 1;
	type = Type::POINT3D;
}