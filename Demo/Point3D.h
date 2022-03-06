/* Start Header -------------------------------------------------------
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Point3D.h
Purpose: Header for 3D point struct
Language: C++17, Visual Studio 2019
Platform: Visual Studio 2019 v16.11.2, Windows 10
Project: haewon.shon_CS350_2
Author: Haewon Shon (haewon.shon@digipen.edu, 180002920)
Creation date: Feb 22, 2022
End Header --------------------------------------------------------*/
#pragma once

#pragma once

#include "GPrimitive.h"
#include <glm/glm.hpp>

struct Point3D : public GPrimitive
{
	Point3D();
	Point3D(const glm::vec3& c, float size = 3.f);
	Point3D(float x, float y, float z, float size = 3.f);
	~Point3D();

	glm::vec3 coordinate;
	float size;
	void Draw() override;
private:
	void Init() override;
};