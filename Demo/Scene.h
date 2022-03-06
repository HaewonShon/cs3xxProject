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
#include "AssimpModel.h"
#include <glm/gtc/constants.hpp>
#include <glm/vec2.hpp>

class Mesh;
class LightSphere;

struct Camera
{
	glm::vec3 pos{ 0.f, 2.5f, 5.f };
	glm::vec3 lookAt{ 0.f, -2.5f, -5.f };
	float theta = 270.f;
	float pi = -26.5f;
	float moveSpeed = 2.f;
	float rotSpeed = 180.f / 1280.f; // 180 degree for 1280 pixels
};

class Scene
{
public:
	virtual void Init();
	virtual void Update([[maybe_unused]] float dt);
	virtual void Clear();
	virtual void Draw();
	virtual void DrawGUI();
	virtual void Reload();
	virtual void LoadShaders();

	void UpdateCamA(float dt);
	void UpdateCamS(float dt);
	void UpdateCamD(float dt);
	void UpdateCamW(float dt);
	void UpdateCamRotation(int xDiff, int yDiff);
protected:
	void SetUniformBuffer();

	void RenderDeferredObjects();
	void RenderDebugObjects();

	void SetupGBuffer();
	void CopyDepthInfo();

	Shader basicShader;
	Shader PhongShadingShader;
	Shader displayNormalShader;
	Shader gBufferShader;
	Shader FSQShader;


	std::vector<AssimpModel*> models;
	std::vector<Mesh*> debugObjects;
	Mesh* FSQ;
	Mesh* plane;

	// scene info
	glm::mat4 viewMatrix;
	glm::mat4 projMatrix;

	// scene control
	Camera cam;
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

	Texture diffuseTexture{ "..\\Textures\\metal_roof_diff_512x512.ppm" };
	Texture specularTexture{ "..\\Textures\\metal_roof_spec_512x512.ppm" };
	Texture whiteTexture{ "..\\Textures\\white.ppm" };

	// uniform block
	GLuint FSQUniformIndex;

	GLuint uniformBlockID;
	const size_t uniformStructSize = 80;

	// cs350 assignment 1 - deferred rendering
	GLuint gBuffer;
	GLuint gPosition, gNormal, gAlbedoSpec, gDepth;
	GLuint depth;

	// GUI info
	bool isDrawingBuffer{ false };
	bool isCopyingDepth{ true };
	int bufferRenderTarget{ 0 };
	bool shouldDrawFaceNormal{ false };
	bool shouldDrawVertexNormal{ false };
	float normalLength{ 0.1f };
	char buf[200];
};