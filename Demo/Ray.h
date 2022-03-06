/* Start Header -------------------------------------------------------
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Ray.h
Purpose: Header for ray boudning volume
Language: C++17, Visual Studio 2019
Platform: Visual Studio 2019 v16.11.2, Windows 10
Project: haewon.shon_CS350_2
Author: Haewon Shon (haewon.shon@digipen.edu, 180002920)
Creation date: Feb 25, 2022
End Header --------------------------------------------------------*/
#pragma once

#include "BVolume.h"
#include <glm/glm.hpp>

struct Ray : public BVolume
{
	Ray();
	Ray(const glm::vec3& start, const glm::vec3& dir);
	~Ray();
	glm::vec3 start;
	glm::vec3 direction;
	void Draw() override;
private:
	void Init() override;
};