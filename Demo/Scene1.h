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

#include "IScene.h"
#include "Shader.h"
#include "Texture.h"
#include <glm/gtc/constants.hpp>
#include <glm/vec2.hpp>

class Mesh;
class LightSphere;

class Scene1 : public IScene
{
public:
	void Init() override;
	void Update([[maybe_unused]] float dt) override;
	void Clear() override;
	void Draw() override;
	void DrawGUI() override;
	void Reload() override;
	void LoadShaders();
private:
	void SetUniformBuffer();
	void SetupFBO();
	void RenderToTexture();
	Shader basicShader;
	Shader PhongLightingShader_CPU;
	Shader PhongShadingShader_CPU;
	Shader BlinnShadingShader_CPU;
	Shader PhongLightingShader_GPU;
	Shader PhongShadingShader_GPU;
	Shader BlinnShadingShader_GPU;
	Shader displayNormalShader;


	Shader skyboxShader; 
	Shader EnvironmentalMappingShader;

	int i = 0;

	std::vector<Mesh*> meshContainer;

	// scene info
	Mesh* mainObject;
	Mesh* plane;
	Mesh* skybox;
	Mesh* testCube;
	Mesh* orbit;
	glm::mat4 viewMatrix;

	// scene control
	glm::vec3 cameraPosition{ 0.f, 0.f, 6.5f };
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
	int currentPickedProcessor{ 0 };
	int currentPickedEntity{ 0 };
	int currentPickedEnvMapping{ 2 };

	float objectScale{ 1.f };
	glm::vec3 objectRotation{ 0.f };
	glm::vec3 ambientCo{ 1.f };
	bool shouldDrawFaceNormal{ false };
	bool shouldDrawVertexNormal{ false };
	float normalLength{ 0.1f };

	Texture diffuseTexture{"..\\Textures\\metal_roof_diff_512x512.ppm"};
	Texture specularTexture{ "..\\Textures\\metal_roof_spec_512x512.ppm" };
	Texture whiteTexture{ "..\\Textures\\white.ppm" };
	Texture skyboxTexture{ {"..\\Textures\\skybox\\right.jpg", "..\\Textures\\skybox\\left.jpg", "..\\Textures\\skybox\\top.jpg",
		"..\\Textures\\skybox\\bottom.jpg", "..\\Textures\\skybox\\front.jpg", "..\\Textures\\skybox\\back.jpg"} };

	// shader
	int currentPickedShader{ 0 };

	// uniform block
	GLuint phongLightingIndex_CPU;
	GLuint phongShadingIndex_CPU;
	GLuint blinnShadingIndex_CPU;
	GLuint phongLightingIndex_GPU;
	GLuint phongShadingIndex_GPU;
	GLuint blinnShadingIndex_GPU;
	GLuint environmentalMappingIndex;

	GLuint uniformBlockID;
	const size_t uniformStructSize = 80;

	// framebuffer id
	GLuint frameBufferID[6];
	GLuint renderedTexture[6];
	GLuint depthrenderbuffer[6];

	// ImGui - Assignment 3
	float refractionIndex = 1.f;
	bool useChromaticAberration = false;
	int currentPickedMaterial = 0;
	float fresnelPower = 5.f;
	float chromaticMultiplier = 1.f;

	bool usePhongShading = false;
};