/* Start Header -------------------------------------------------------
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: AABB.h
Purpose: Header for AABB
Language: C++17, Visual Studio 2019
Platform: Visual Studio 2019 v16.11.2, Windows 10
Project: haewon.shon_CS350_2
Author: Haewon Shon (haewon.shon@digipen.edu, 180002920)
Creation date: Feb 25, 2022
End Header --------------------------------------------------------*/
#pragma once

#include "BVolume.h"
#include <glm/glm.hpp>

struct AABB : public BVolume
{
	AABB();
	AABB(const glm::vec3& min, const glm::vec3& max);
	~AABB();
	//AABB(const glm::vec3& center, const glm::vec3 half) : min(center - half), max(center + half) {}
	glm::vec3 min;
	glm::vec3 max;

	void Draw() override;
private:
	void Init() override;
};