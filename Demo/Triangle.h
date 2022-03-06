/* Start Header -------------------------------------------------------
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Triangle.h
Purpose: Header for triangle struct
Language: C++17, Visual Studio 2019
Platform: Visual Studio 2019 v16.11.2, Windows 10
Project: haewon.shon_CS350_2
Author: Haewon Shon (haewon.shon@digipen.edu, 180002920)
Creation date: Feb 25, 2022
End Header --------------------------------------------------------*/
#pragma once

#pragma once

#include "GPrimitive.h"
#include <glm/glm.hpp>
#include "Point3D.h"

struct Triangle : public GPrimitive
{
	Triangle(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3);
	Triangle(const Point3D& v1, const Point3D& v2, const Point3D& v3);

	Point3D v1;
	Point3D v2;
	Point3D v3;

	void Draw() override;
private:
	void Init() override;
};