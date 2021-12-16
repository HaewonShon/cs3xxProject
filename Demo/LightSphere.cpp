/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: LightSphere.cpp
Purpose: Source file LightSphere class, which is for the light source of the program
Language: C++17, Visual Studio 2019
Platform: Visual Studio 2019 v16.11.2, Windows 10
Project: haewon.shon_CS300_2
Author: Haewon Shon (haewon.shon@digipen.edu, 180002920)
Creation date: November 4, 2021
End Header --------------------------------------------------------*/

#include "LightSphere.h"

LightSphere::LightSphere(float orbitRadius, const glm::vec3& scale, bool random, float speed) : radius(orbitRadius), speed(speed)
{
	Mesh* mesh = MESH::CreateSphere();

	this->vao = mesh->GetVAO();
	this->verticesSize = mesh->GetVerticesSize();
	this->indicesSize = mesh->GetIndicesSize();
	this->primitiveType = mesh->GetPrimitiveType();
	this->modelNormalizeMatrix = mesh->GetModelNormalizeMatrix();

	SetPosition({ orbitRadius, 0.f, 0.f });
	SetScale(scale);

	if (random)
	{
		ambient = glm::vec3((rand() % 256) / 255.f, (rand() % 256) / 255.f, (rand() % 256) / 255.f);
		diffuse = glm::vec3((rand() % 256) / 255.f, (rand() % 256) / 255.f, (rand() % 256) / 255.f);
		specular = glm::vec3((rand() % 256) / 255.f, (rand() % 256) / 255.f, (rand() % 256) / 255.f);
	}
}

LightSphere::~LightSphere()
{

}

void LightSphere::Update(float dt)
{
	angle += speed * dt;
	SetPosition({ radius * std::cos(glm::radians(angle + angleOffset)), 0.f, radius * std::sin(glm::radians(angle + angleOffset)) });
}