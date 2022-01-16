/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Scene1.h
Purpose: Scene Header for CS300 assignment 1
Language: C++17, Visual Studio 2019
Platform: Visual Studio 2019 v16.11.2, Windows 10
Project: haewon.shon_CS300_1
Author: Haewon Shon (haewon.shon@digipen.edu, 180002920)
Creation date: August 29, 2021
End Header --------------------------------------------------------*/

#pragma once
#include "Shader.h"
#include "Texture.h"
#include <glm/gtc/constants.hpp>
#include <glm/vec2.hpp>

class Mesh;
class LightSphere;

class Scene
{
public:
	void Init();
	void Update([[maybe_unused]] float dt);
	void Clear();
	void Draw();
	void DrawGUI();
	void Reload();
	void LoadShaders();

	void UpdateCamA();
	void UpdateCamS();
	void UpdateCamD();
	void UpdateCamW();
private:
	void SetUniformBuffer();

	void RenderDeferredObjects();
	void RenderDebugObjects();

	Shader basicShader;
	Shader PhongShadingShader;
	Shader displayNormalShader;

	std::vector<Mesh*> meshContainer;

	// scene info
	Mesh* mainObject;
	Mesh* plane;
	Mesh* testCube;
	Mesh* orbit;
	glm::mat4 viewMatrix;

	// scene control
	glm::vec3 cameraPosition{ 0.f, 2.5f, 5.f };
	glm::vec3 cameraLookAt{};
	bool isLightsRotate{ true };
	// global variables
	glm::vec3 globalAmbient{ 0.f };
	glm::vec3 attenuation{ 1.f, 0.5f, 0.25f };
	glm::vec3 fog{ 0.2f };
	glm::vec2 fog_near_far{ 0.1f, 20.f };

	// spheres
	int sphereNums = 1;
	int const maxSphereNums = 16;
	float const sphereSpeed = glm::two_pi<float>() / 4.f;
	float const orbitRadius = 2.5f;
	int const orbitVertexCount = 64;
	std::vector<LightSphere*> spheres;
	glm::vec3 sphereSize{ 0.1f, 0.1f, 0.1f };

	// model
	int currentPickedObject{ 1 };
	int currentTexProjMode{ 2 };
	int currentPickedEntity{ 0 };

	float objectScale{ 1.f };
	glm::vec3 objectRotation{ 0.f };
	glm::vec3 ambientCo{ 1.f };
	bool shouldDrawFaceNormal{ false };
	bool shouldDrawVertexNormal{ false };
	float normalLength{ 0.1f };

	Texture diffuseTexture{"..\\Textures\\metal_roof_diff_512x512.ppm"};
	Texture specularTexture{ "..\\Textures\\metal_roof_spec_512x512.ppm" };
	Texture whiteTexture{ "..\\Textures\\white.ppm" };

	// shader
	int currentPickedShader{ 0 };

	// uniform block
	GLuint phongShadingIndex;

	GLuint uniformBlockID;
	const size_t uniformStructSize = 80;
};