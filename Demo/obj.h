/* Start Header -------------------------------------------------------
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: obj.h
Purpose: header for drawn object interface
Language: C++17, Visual Studio 2019
Platform: Visual Studio 2019 v16.11.2, Windows 10
Project: haewon.shon_CS350_2
Author: Haewon Shon (haewon.shon@digipen.edu, 180002920)
Creation date: March 6, 2022
End Header --------------------------------------------------------*/
#pragma once

#include <glew.h>
#include <glm/glm.hpp>

struct obj
{
	enum class Type { AABB, SPHERE, RAY, PLANE, POINT3D, TRIANGLE };
	GLuint vao;
	GLsizei elementSize;
	glm::vec3 color;
	Type type;

	virtual void Draw() = 0;
private:
	virtual void Init() = 0;
};