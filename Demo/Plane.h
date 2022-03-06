/* Start Header -------------------------------------------------------
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Plane.h
Purpose: Header for plane struct
Language: C++17, Visual Studio 2019
Platform: Visual Studio 2019 v16.11.2, Windows 10
Project: haewon.shon_CS350_2
Author: Haewon Shon (haewon.shon@digipen.edu, 180002920)
Creation date: Feb 22, 2022
End Header --------------------------------------------------------*/
#pragma once

#include "GPrimitive.h"
#include <glm/glm.hpp>

struct Plane : public GPrimitive
{
	Plane();
	Plane(const glm::vec4& n);
	Plane(float x, float y, float z, float d);
	Plane(const glm::vec3& _normal, const glm::vec3& point);
	glm::vec4 normal;

	void Draw() override;
private:
	void Init() override;
};