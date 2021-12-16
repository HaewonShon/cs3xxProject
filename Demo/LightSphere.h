/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: LightSphere.h
Purpose: Header file LightSphere class, which is for the light source of the program
Language: C++17, Visual Studio 2019
Platform: Visual Studio 2019 v16.11.2, Windows 10
Project: haewon.shon_CS300_2
Author: Haewon Shon (haewon.shon@digipen.edu, 180002920)
Creation date: November 4, 2021
End Header --------------------------------------------------------*/

#include "Mesh.h"
#include <glm/gtc/constants.hpp>

class LightSphere : public Mesh
{
public:
	enum class LightType { POINT, DIRECTIONAL, SPOT };

public:
	LightSphere(float orbitRadius, const glm::vec3& scale, bool random = true, float speed = 45.f);
	virtual ~LightSphere();
	void Update(float dt);

public:
	glm::vec3 ambient{ .5f };
	glm::vec3 diffuse{ 1.f };
	glm::vec3 specular{ 1.f };
	float angle = 0.f;
	float angleOffset = 0.f; // degree
	float speed = 45.f;
	float radius = 1.f;
	LightType lType = LightType::POINT;

	//spot
	float innerAngle = 30.f;
	float outerAngle = 45.f;
	float fallOff = 1.f;
};