/* Start Header -------------------------------------------------------
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Sphere.h
Purpose: Header for Bounding Sphere
Language: C++17, Visual Studio 2019
Platform: Visual Studio 2019 v16.11.2, Windows 10
Project: haewon.shon_CS350_2
Author: Haewon Shon (haewon.shon@digipen.edu, 180002920)
Creation date: Feb 25, 2022
End Header --------------------------------------------------------*/
#pragma once

#include "BVolume.h"
#include "Point3D.h"
#include <glm/glm.hpp>

struct Sphere : public BVolume
{
	Sphere();
	Sphere(const glm::vec3& pos, float r);
	Sphere(const Point3D& p, float r);
	~Sphere();
	Point3D position;
	float radius;
	void Draw() override;
private:
	void Init() override;
};