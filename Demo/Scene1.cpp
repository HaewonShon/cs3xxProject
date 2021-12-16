/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Scene1.cpp
Purpose: Scene source for CS300 assignment 1
Language: C++17, Visual Studio 2019
Platform: Visual Studio 2019 v16.11.2, Windows 10
Project: haewon.shon_CS300_1
Author: Haewon Shon (haewon.shon@digipen.edu, 180002920)
Creation date: August 29, 2021
End Header --------------------------------------------------------*/

#include "Scene1.h"
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui/imgui.h>
#include <array>
#include <string>
#include <iostream>
#include "Mesh.h"
#include "LightSphere.h"
#include "ObjectConverter.h"

void Scene1::Init()
{
	/////////////// SHADER //////////////
	LoadShaders();

	/////////////// SHADER //////////////

	/////////////// MAIN OBJECT //////////////
	// main object in the center
	mainObject = CreateObjectFromFile("..\\Models\\bunny.obj", true, true, static_cast<Mesh::UVType>(currentTexProjMode), currentPickedEntity == 0 ? false : true);
	//meshContainer.push_back(mainObject);

	/////////////// MAIN OBJECT //////////////

	/////////////// PLANE //////////////
	//plane = CreateObjectFromFile("..\\Models\\quad.obj", true, true);
	//plane->SetPosition({ 0.f, -1.f, 0.f });
	////plane->SetRotation({ -glm::half_pi<float>(), 0.f, 0.f });
	////plane->SetScale({ 5.f, 5.f, 1.f });
	//meshContainer.push_back(plane);

	/////////////// PLANE //////////////

	/////////////// SKYBOX //////////////
	skybox = CreateObjectFromFile("..\\Models\\cube.obj");
	meshContainer.push_back(skybox);
	/////////////// SKYBOX //////////////

	/////////////// LIGHT SPHERES //////////////
	spheres.push_back(new LightSphere(orbitRadius, sphereSize, false));
	meshContainer.push_back(spheres[0]);
	/////////////// LIGHT SPHERES //////////////

	/////////////// ORBIT //////////////
	orbit = new Mesh();
	float const angleDiff = glm::two_pi<float>() / orbitVertexCount;
	int i = 0;
	for (float angle = 0.f; angle < glm::two_pi<float>(); angle += angleDiff, ++i)
	{
		orbit->AddVertex({ orbitRadius * std::cos(angle), 0.f, orbitRadius * std::sin(angle) });
		orbit->AddIndex(i);
		orbit->AddIndex((i + 1) % orbitVertexCount);
	}

	orbit->SetPrimitiveType(GL_LINES);
	orbit->Build();
	meshContainer.push_back(orbit);
	/////////////// ORBIT //////////////

	/////////////// IMGUI INIT //////////////
	currentPickedObject = 0;

	/////////////// UNIFORM BLOCK SETUP //////////////
	phongLightingIndex_CPU = glGetUniformBlockIndex(PhongLightingShader_CPU.GetID(), "LightInfo");
	glUniformBlockBinding(PhongLightingShader_CPU.GetID(), phongLightingIndex_CPU, 0);
	phongShadingIndex_CPU = glGetUniformBlockIndex(PhongShadingShader_CPU.GetID(), "LightInfo");
	glUniformBlockBinding(PhongShadingShader_CPU.GetID(), phongShadingIndex_CPU, 0);
	blinnShadingIndex_CPU = glGetUniformBlockIndex(BlinnShadingShader_CPU.GetID(), "LightInfo");
	glUniformBlockBinding(BlinnShadingShader_CPU.GetID(), blinnShadingIndex_CPU, 0);
	phongLightingIndex_GPU = glGetUniformBlockIndex(PhongLightingShader_GPU.GetID(), "LightInfo");
	glUniformBlockBinding(PhongLightingShader_GPU.GetID(), phongLightingIndex_GPU, 0);
	phongShadingIndex_GPU = glGetUniformBlockIndex(PhongShadingShader_GPU.GetID(), "LightInfo");
	glUniformBlockBinding(PhongShadingShader_GPU.GetID(), phongShadingIndex_GPU, 0);
	blinnShadingIndex_GPU = glGetUniformBlockIndex(BlinnShadingShader_GPU.GetID(), "LightInfo");
	glUniformBlockBinding(BlinnShadingShader_GPU.GetID(), blinnShadingIndex_GPU, 0);

	environmentalMappingIndex = glGetUniformBlockIndex(EnvironmentalMappingShader.GetID(), "LightInfo");
	glUniformBlockBinding(EnvironmentalMappingShader.GetID(), environmentalMappingIndex, 0);

	glGenBuffers(1, &uniformBlockID);
	glBindBuffer(GL_UNIFORM_BUFFER, uniformBlockID);
	glBufferData(GL_UNIFORM_BUFFER, uniformStructSize * maxSphereNums + 16 * 6 + 8, NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, uniformBlockID, 0, uniformStructSize * maxSphereNums + 16 * 6 + 8);
	/////////////// UNIFORM BLOCK SETUP //////////////

	SetupFBO();
};

void Scene1::Update(float dt)
{
	if (!isLightsRotate)
	{
		dt = 0.f;
	}
	for (LightSphere* sphere : spheres)
	{
		sphere->Update(dt);
	}
}

void Scene1::Clear()
{
	for (Mesh* mesh : meshContainer)
	{
		if (mesh)
		{
			delete mesh;
		}
	}
	meshContainer.clear();
	spheres.clear();

	delete mainObject;

	glDeleteBuffers(1, &uniformBlockID);

	basicShader.Delete();
	PhongLightingShader_CPU.Delete();
	PhongShadingShader_CPU.Delete();
	BlinnShadingShader_CPU.Delete();
	displayNormalShader.Delete();
	skyboxShader.Delete();
	EnvironmentalMappingShader.Delete();

	glDeleteFramebuffers(6, frameBufferID);
	glDeleteRenderbuffers(6, depthrenderbuffer);
	glDeleteTextures(6, renderedTexture);
}

void Scene1::Draw()
{
	/////////////// SCENE CLEAR ///////////////
	glClearColor(fog.x, fog.y, fog.z, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/////////////// RENDERING TO TEXTURE FOR ENV MAPPING ///////////////
	RenderToTexture();

	/////////////// COLOR INIT //////////////
	glm::vec3 otherColor{ 1.f };
	glm::vec3 fNormalColor{ 0.f, 0.f, 1.f };
	glm::vec3 vNormalColor = { 1.f, 0.f, 0.f };
	/////////////// COLOR INIT //////////////

	/////////////// SKYBOX /////////////
	viewMatrix = glm::perspective(45.f, 1280.f / 720.f, 0.1f, 10.f)
		* glm::mat4(glm::mat3(glm::lookAt(cameraPosition, glm::vec3(0.f), glm::vec3(0.f, 1.f, 0.f))));

	glDepthMask(GL_FALSE);
	skyboxShader.Enable();

	glActiveTexture(GL_TEXTURE3); 
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture.textureID);
	skyboxShader.SetUniform("viewMatrix", viewMatrix);
	skyboxShader.SetUniform("skyboxTexture", 3);
	skybox->Draw();
	skyboxShader.Disable();
	glDepthMask(GL_TRUE);
	/////////////// SKYBOX /////////////

	viewMatrix = glm::perspective(45.f, 1280.f / 720.f, 0.1f, 10.f)
		* glm::lookAt(cameraPosition,
			glm::vec3(0.f),
			glm::vec3(0.f, 1.f, 0.f));

	/////////////// LIGHT SHADER START //////////////
	//switch (currentPickedShader)
	//{
	//case 0: // Phong Lighting
	//{
	//	if (currentPickedProcessor == 0)
	//	{
	//		PhongLightingShader_CPU.Enable();
	//		PhongLightingShader_CPU.SetUniform("modelMatrix", mainObject->GetModelMatrix());
	//		PhongLightingShader_CPU.SetUniform("viewMatrix", viewMatrix);
	//		PhongLightingShader_CPU.SetUniform("diffuseTexture", 0);
	//		PhongLightingShader_CPU.SetUniform("specularTexture", 1);
	//		mainObject->Draw();
	//	}
	//	else
	//	{
	//		PhongLightingShader_GPU.Enable();
	//		PhongLightingShader_GPU.SetUniform("modelMatrix", mainObject->GetModelMatrix());
	//		PhongLightingShader_GPU.SetUniform("viewMatrix", viewMatrix);
	//		PhongLightingShader_GPU.SetUniform("diffuseTexture", 0);
	//		PhongLightingShader_GPU.SetUniform("specularTexture", 1);
	//		PhongLightingShader_GPU.SetUniform("useNormalForUV", currentPickedEntity);
	//		PhongLightingShader_GPU.SetUniform("uvType", currentTexProjMode);
	//		PhongLightingShader_GPU.SetUniform("center", mainObject->GetCenter());
	//		mainObject->Draw();
	//	}
	//	SetUniformBuffer();
	//	PhongLightingShader_CPU.Disable();
	//}
	//break;
	//case 1: // Phong Shading
	//{
	//	/////////////// LIGHT SHADER INIT //////////////
	//	if (currentPickedProcessor == 0)
	//	{
	//		PhongShadingShader_CPU.Enable();
	//		/////////////// DRAW MAIN OBJECT //////////////
	//		PhongShadingShader_CPU.SetUniform("viewMatrix", viewMatrix);
	//		PhongShadingShader_CPU.SetUniform("modelMatrix", mainObject->GetModelMatrix());

	//		PhongShadingShader_CPU.SetUniform("diffuseTexture", 0);
	//		PhongShadingShader_CPU.SetUniform("specularTexture", 1);
	//		mainObject->Draw();
	//		/////////////// DRAW MAIN OBJECT //////////////
	//	}
	//	else
	//	{
	//		PhongShadingShader_GPU.Enable();
	//		/////////////// DRAW MAIN OBJECT //////////////
	//		PhongShadingShader_GPU.SetUniform("viewMatrix", viewMatrix);
	//		PhongShadingShader_GPU.SetUniform("modelMatrix", mainObject->GetModelMatrix());

	//		PhongShadingShader_GPU.SetUniform("diffuseTexture", 0);
	//		PhongShadingShader_GPU.SetUniform("specularTexture", 1);
	//		PhongShadingShader_GPU.SetUniform("useNormalForUV", currentPickedEntity);
	//		PhongShadingShader_GPU.SetUniform("uvType", currentTexProjMode);
	//		PhongShadingShader_GPU.SetUniform("center", mainObject->GetCenter());
	//		mainObject->Draw();
	//		/////////////// DRAW MAIN OBJECT //////////////
	//	}
	//	SetUniformBuffer();


	//	PhongShadingShader_CPU.Disable();
	//}
	//break;
	//case 2: // Blinn Shading
	//{
	//	/////////////// LIGHT SHADER INIT //////////////
	//	if (currentPickedProcessor == 0)
	//	{
	//		BlinnShadingShader_CPU.Enable();
	//		/////////////// DRAW MAIN OBJECT //////////////
	//		BlinnShadingShader_CPU.SetUniform("viewMatrix", viewMatrix);
	//		BlinnShadingShader_CPU.SetUniform("modelMatrix", mainObject->GetModelMatrix());

	//		BlinnShadingShader_CPU.SetUniform("diffuseTexture", 0);
	//		BlinnShadingShader_CPU.SetUniform("specularTexture", 1);
	//		mainObject->Draw();
	//		/////////////// DRAW MAIN OBJECT //////////////
	//	}
	//	else
	//	{
	//		BlinnShadingShader_GPU.Enable();
	//		/////////////// DRAW MAIN OBJECT //////////////
	//		BlinnShadingShader_GPU.SetUniform("viewMatrix", viewMatrix);
	//		BlinnShadingShader_GPU.SetUniform("modelMatrix", mainObject->GetModelMatrix());

	//		BlinnShadingShader_GPU.SetUniform("diffuseTexture", 0);
	//		BlinnShadingShader_GPU.SetUniform("specularTexture", 1);
	//		BlinnShadingShader_GPU.SetUniform("useNormalForUV", currentPickedEntity);
	//		BlinnShadingShader_GPU.SetUniform("uvType", currentTexProjMode);
	//		BlinnShadingShader_GPU.SetUniform("center", mainObject->GetCenter());
	//		mainObject->Draw();
	//		/////////////// DRAW MAIN OBJECT //////////////
	//	}
	//	SetUniformBuffer();

	//	BlinnShadingShader_CPU.Disable();
	//}
	//break;
	//}
	/////////////// LIGHT SHADER END //////////////

	/////////////// DRAW MAIN OBJECT ASSIGNMENT 3 //////////////
	
	EnvironmentalMappingShader.Enable();
	EnvironmentalMappingShader.SetUniform("viewMatrix", viewMatrix);
	EnvironmentalMappingShader.SetUniform("modelMatrix", mainObject->GetModelMatrix());
	EnvironmentalMappingShader.SetUniform("cameraPos", cameraPosition);
	
	EnvironmentalMappingShader.SetUniform("mode", currentPickedEnvMapping);
	EnvironmentalMappingShader.SetUniform("refractionIndex", refractionIndex);
	EnvironmentalMappingShader.SetUniform("fresnelPower", fresnelPower);
	EnvironmentalMappingShader.SetUniform("useChromaticAberration", useChromaticAberration);
	EnvironmentalMappingShader.SetUniform("chromaticMultiplier", chromaticMultiplier);
	EnvironmentalMappingShader.SetUniform("usePhongShading", usePhongShading);
	EnvironmentalMappingShader.SetUniform("projectionMode", currentTexProjMode);

	SetUniformBuffer();
	

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, renderedTexture[0]);
	EnvironmentalMappingShader.SetUniform("left", 4);
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, renderedTexture[1]);
	EnvironmentalMappingShader.SetUniform("right", 5);
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, renderedTexture[2]);
	EnvironmentalMappingShader.SetUniform("bottom", 6);
	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D, renderedTexture[3]);
	EnvironmentalMappingShader.SetUniform("top", 7);
	glActiveTexture(GL_TEXTURE8);
	glBindTexture(GL_TEXTURE_2D, renderedTexture[4]);
	EnvironmentalMappingShader.SetUniform("front", 8);
	glActiveTexture(GL_TEXTURE9);
	glBindTexture(GL_TEXTURE_2D, renderedTexture[5]);
	EnvironmentalMappingShader.SetUniform("back", 9);

	mainObject->Draw();

	EnvironmentalMappingShader.Disable();


	/////////////// DRAW LIGHT SPHERES //////////////
	basicShader.Enable();
	basicShader.SetUniform("viewMatrix", viewMatrix);


	for (LightSphere* sphere : spheres)
	{
		basicShader.SetUniform("inputColor", sphere->diffuse);
		basicShader.SetUniform("modelMatrix", sphere->GetModelMatrix());
		sphere->Draw();
	}

	basicShader.SetUniform("inputColor", otherColor);
	basicShader.SetUniform("modelMatrix", orbit->GetModelMatrix());
	orbit->Draw();
	basicShader.Disable();
	/////////////// DRAW LIGHT SPHERES //////////////

	/////////////// DRAW NORMAL VECTORS //////////////
	displayNormalShader.Enable();
	displayNormalShader.SetUniform("viewMatrix", viewMatrix);
	displayNormalShader.SetUniform("modelMatrix", mainObject->GetModelMatrix());
	displayNormalShader.SetUniform("normalLength", normalLength);
	if (shouldDrawVertexNormal == true)
	{
		displayNormalShader.SetUniform("inputColor", vNormalColor);
		mainObject->DrawVertexNormal();
	}

	if (shouldDrawFaceNormal == true)
	{
		displayNormalShader.Enable();
		displayNormalShader.SetUniform("inputColor", fNormalColor);
		mainObject->DrawFaceNormal();
	}
	displayNormalShader.Disable();
	/////////////// DRAW NORMAL VECTORS //////////////
}

void Scene1::DrawGUI()
{
	ImGui::Begin("Control Panel");

	//if (ImGui::CollapsingHeader("Object"))
	{
		constexpr std::array OBJECTS = {
			"..\\Models\\4Sphere.obj",
			"..\\Models\\bunny.obj",
			"..\\Models\\bunny_high_poly.obj",
			"..\\Models\\cube.obj",
			"..\\Models\\cube2.obj",
			"..\\Models\\cup.obj",
			"..\\Models\\lucy_princeton.obj",
			"..\\Models\\quad.obj",
			//"..\\Models\\rhino.obj",
			"..\\Models\\sphere.obj",
			"..\\Models\\sphere_modified.obj",
			//"..\\Models\\starwars1.obj",
			"..\\Models\\triangle.obj",
		};
		if (ImGui::Combo("Pick model to render", &currentPickedObject, OBJECTS.data(), static_cast<int>(OBJECTS.size())))
		{
			delete mainObject;
			mainObject = CreateObjectFromFile(OBJECTS[currentPickedObject], true, true, static_cast<Mesh::UVType>(currentTexProjMode), currentPickedEntity == 0 ? false : true);
		}

		/*ImGui::Separator();
		if (ImGui::DragFloat("Object Scale", &objectScale, 0.01f, 0.1f, 5.f))
		{
			mainObject->SetScale(glm::vec3{ objectScale });
		}
		const float ROTATION_ANGLE_LIMIT = glm::pi<float>();
		if (ImGui::DragFloat3("Object Rotation", &objectRotation.x, 0.01f, -ROTATION_ANGLE_LIMIT, ROTATION_ANGLE_LIMIT))
		{
			mainObject->SetRotation(objectRotation);
		}

		ImGui::Separator();

		constexpr std::array TEXTURE_PROJECTION_MODE = {
			"Planer",
			"Spherical",
			"Cylindrical",
		};
		if (ImGui::Combo("Texture Projection Mode", &currentTexProjMode, TEXTURE_PROJECTION_MODE.data(), static_cast<int>(TEXTURE_PROJECTION_MODE.size())))
		{
			switch (currentTexProjMode)
			{
			case 0:
				mainObject->RebuildUVs(Mesh::UVType::PLANER, currentPickedEntity == 0 ? false : true);
				break;
			case 1:
				mainObject->RebuildUVs(Mesh::UVType::SPHERICAL, currentPickedEntity == 0 ? false : true);
				break;
			case 2:
				mainObject->RebuildUVs(Mesh::UVType::CYLINDRICAL, currentPickedEntity == 0 ? false : true);
				break;
			}
		}

		constexpr std::array PROCESSORS = {
			"CPU",
			"GPU",
		};
		ImGui::Combo("Texture Projection Processor", &currentPickedProcessor, PROCESSORS.data(), static_cast<int>(PROCESSORS.size()));

		constexpr std::array ENTITY = {
			"Position",
			"Normal",
		};
		if (ImGui::Combo("Texture Projection Entity", &currentPickedEntity, ENTITY.data(), static_cast<int>(ENTITY.size())))\
		{
			mainObject->RebuildUVs(static_cast<Mesh::UVType>(currentTexProjMode), currentPickedEntity == 0 ? false : true);
		}
		*/
		ImGui::Separator();

		ImGui::Checkbox("Draw face normals", &shouldDrawFaceNormal);
		ImGui::Checkbox("Draw vertex normals", &shouldDrawVertexNormal);

		if (shouldDrawFaceNormal || shouldDrawVertexNormal)
		{
			ImGui::DragFloat("Normal Length", &normalLength, 0.01f, 0.01f, 1.f);
		}
	}

	if (ImGui::CollapsingHeader("Scene control"))
	{
		ImGui::DragFloat3("Camera Position", &cameraPosition.x, 0.1f, -10.f, 10.f);
		ImGui::Separator();
		if (ImGui::Button("Reload Scene"))
		{
			this->Reload();
		}
		ImGui::Indent();
		if (ImGui::CollapsingHeader("Global Variables"))
		{
			ImGui::ColorEdit3("ambient Coefficient", glm::value_ptr(ambientCo), 0);
			ImGui::ColorEdit3("Global Ambient", glm::value_ptr(globalAmbient), 0);
			ImGui::DragFloat3("Attenuation", &attenuation.x, 0.01f, 0.f);
			ImGui::ColorEdit3("Fog Color", glm::value_ptr(fog), 0);
			ImGui::DragFloat2("Fog near and far", &fog_near_far.x, 0.1f, 0.1f, 100.f);
		}
		ImGui::Unindent();
	}

	/*if (ImGui::CollapsingHeader("Shader"))
	{
		constexpr std::array SHADERS = {
			"Phong Lighting",
			"Phong Shading",
			"Blinn Shading",
		};
		ImGui::Combo("Pick Shader To Render", &currentPickedShader, SHADERS.data(), static_cast<int>(SHADERS.size()));
		if (ImGui::Button("Reload Shader"))
		{
			this->LoadShaders();
		}
	}*/

	if (ImGui::CollapsingHeader("Lights"))
	{
		ImGui::Checkbox("Light Rotation", &isLightsRotate);

		constexpr std::array LIGHTS = {
			"Point",
			"Directional",
			"SpotLight",
		};
		if (ImGui::CollapsingHeader("Light Sources Setting"))
		{
			ImGui::Indent(16.0f);
			for (int i = 0; i < spheres.size(); ++i)
			{
				int toDelete = -1;

				std::string lightName = "Light " + std::to_string(i + 1);
				if (ImGui::TreeNode(lightName.c_str()))
				{
					ImGui::Combo("Light Type", reinterpret_cast<int*>(&(spheres[i]->lType)), LIGHTS.data(), static_cast<int>(LIGHTS.size()));
					ImGui::ColorEdit3("Ambient", glm::value_ptr(spheres[i]->ambient), 0);
					ImGui::ColorEdit3("Diffuse", glm::value_ptr(spheres[i]->diffuse), 0);
					ImGui::ColorEdit3("Specular", glm::value_ptr(spheres[i]->specular), 0);
					ImGui::DragFloat("Angle Offset", &spheres[i]->angleOffset, 0.1f);
					ImGui::DragFloat("Speed", &spheres[i]->speed, 0.1f);
					if (spheres[i]->lType == LightSphere::LightType::SPOT)
					{
						ImGui::Text("Spot Light Properties");
						ImGui::DragFloat("Inner Angle", &(spheres[i]->innerAngle), 0.1f, 0.f, 90.f);
						ImGui::DragFloat("Outer Angle", &(spheres[i]->outerAngle), 0.1f, 0.f, 90.f);
						ImGui::DragFloat("Falloff", &(spheres[i]->fallOff), 0.1f, 0.f, 10.f);
					}

					if (sphereNums > 1)
					{
						if (ImGui::Button("Delete"))
						{
							toDelete = i;
						}
					}
					ImGui::TreePop();
				}
				if (toDelete != -1)
				{
					delete spheres[i];
					spheres.erase(spheres.begin() + i);
					--sphereNums;
				}
			}
			ImGui::Unindent();
		}

		ImGui::Separator();

		if (sphereNums < maxSphereNums)
		{
			if (ImGui::Button("Add New Light"))
			{
				++sphereNums;
				spheres.push_back(new LightSphere(orbitRadius, sphereSize, false));
				meshContainer.push_back(spheres.back());
			}
		}
	}

	if (ImGui::CollapsingHeader("Scenarios"))
	{
		if (ImGui::Button("1st Scenario"))
		{
			while (sphereNums < 8)
			{
				++sphereNums;
				spheres.push_back(new LightSphere(orbitRadius, sphereSize, false));
				meshContainer.push_back(spheres.back());
			}

			glm::vec3 ambient{ 0.f };
			glm::vec3 diffuse{ 0.1f, 0.f, 0.2f };
			glm::vec3 specular{ 0.f, 0.2f, 0.1f };
			LightSphere::LightType lightType = static_cast<LightSphere::LightType>(rand() % 3);

			std::cout << "Scenario #1 : all same color, same light type" << std::endl;
			std::cout << "Light type : ";
			switch (lightType)
			{
			case LightSphere::LightType::POINT: std::cout << "Point" << std::endl; break;
			case LightSphere::LightType::DIRECTIONAL: std::cout << "Directional" << std::endl; break;
			case LightSphere::LightType::SPOT: std::cout << "Spot Light" << std::endl; break;
			}

			int angleOffset = 0;
			for (LightSphere* light : spheres)
			{
				light->ambient = ambient;
				light->diffuse = diffuse;
				light->specular = specular;
				light->lType = lightType;
				light->angleOffset = 45.f * angleOffset++;
			}
		}
		if (ImGui::Button("2nd Scenario"))
		{
			while (sphereNums < 8)
			{
				++sphereNums;
				spheres.push_back(new LightSphere(orbitRadius, sphereSize, false));
				meshContainer.push_back(spheres.back());
			}
			LightSphere::LightType lightType = static_cast<LightSphere::LightType>(rand() % 3);

			std::cout << "Scenario #2 : all same color, different light type" << std::endl;

			int angleOffset = 0;
			for (LightSphere* light : spheres)
			{
				light->ambient = glm::vec3{ (rand() % 256) / 255.f, (rand() % 256) / 255.f, (rand() % 256) / 255.f } *0.05f;
				light->diffuse = glm::vec3{ (rand() % 256) / 255.f, (rand() % 256) / 255.f, (rand() % 256) / 255.f } *0.1f;
				light->specular = glm::vec3{ (rand() % 256) / 255.f, (rand() % 256) / 255.f, (rand() % 256) / 255.f } *0.1f;
				light->lType = lightType;
				light->angleOffset = 45.f * angleOffset++;
			}
		}
		if (ImGui::Button("3rd Scenario"))
		{
			while (sphereNums < 8)
			{
				++sphereNums;
				spheres.push_back(new LightSphere(orbitRadius, sphereSize, false));
				meshContainer.push_back(spheres.back());
			}

			std::cout << "Scenario #3 : custom scenario" << std::endl;

			int angleOffset = 0;
			for (LightSphere* light : spheres)
			{
				if (angleOffset % 2 == 0)
				{
					light->ambient = glm::vec3{ 0.f };
					light->diffuse = glm::vec3{ 1.f, 180.f / 255.f, 0.f };
					light->specular = glm::vec3{ 1.f, 180.f / 255.f, 0.f };
					light->lType = LightSphere::LightType::SPOT;
				}
				else
				{
					light->ambient = glm::vec3{ 0.f };
					light->ambient = glm::vec3{ 0.1f, 0.f, 0.2f };
					light->ambient = glm::vec3{ 0.f, 0.2f, 0.1f };
					light->lType = LightSphere::LightType::POINT;
				}
				light->angleOffset = 45.f * angleOffset++;
			}

		}
	}

	if (ImGui::CollapsingHeader("Environment Mapping"))
	{
		constexpr std::array SETTINGS = {
			"Reflection Only",
			"Refraction Only",
			"Fresnel"
		};
		ImGui::Combo("Envrionment Mapping", &currentPickedEnvMapping, SETTINGS.data(), static_cast<int>(SETTINGS.size()));
		ImGui::Separator();

		switch (currentPickedEnvMapping)
		{
		case 0: // reflection only
		{

		}
		break;
		case 1: // refraction only
		{
			ImGui::DragFloat("Refraction Index", &refractionIndex, 0.1f, 0.0f, 100.0f);
			constexpr std::array INDICES = {
				"Air",
				"Hydrogen",
				"Water",
				"Olive Oil",
				"Ice",
				"Quartz",
				"Diamond",
				"Acrylic",
				"Plexiglas",
				"Lucite",
			};
			if (ImGui::Combo("Surface Material", &currentPickedMaterial, INDICES.data(), static_cast<int>(INDICES.size())))
			{
				switch (currentPickedMaterial)
				{
				case 0: // air
					refractionIndex = 1.000293f;
					break;
				case 1: // hydrogen
					refractionIndex = 1.000132f;
					break;
				case 2: // water
					refractionIndex = 1.333f;
					break;
				case 3: // olive oil
					refractionIndex = 1.47f;
					break;
				case 4: // Ice
					refractionIndex = 1.31f;
					break;
				case 5: // Quartz
					refractionIndex = 1.46f;
					break;
				case 6: // Diamond 
					refractionIndex = 2.42f;
					break;
				case 7: // acrylic
				case 8: // plexiglas
				case 9: // lucite
					refractionIndex = 1.49f;
					break;
				}
			}
			ImGui::Separator();
			ImGui::Checkbox("Chromatic Aberration", &useChromaticAberration);
			ImGui::DragFloat("Chromatic Multiplier", &chromaticMultiplier, 0.0f, 0.01f, 1.f);
		}
		break;
		case 2: // fresnel
		{
			ImGui::DragFloat("Refraction Index", &refractionIndex, 0.1f, 0.0f, 100.0f);
			constexpr std::array INDICES = {
				"Air",
				"Hydrogen",
				"Water",
				"Olive Oil",
				"Ice",
				"Quartz",
				"Diamond",
				"Acrylic",
				"Plexiglas",
				"Lucite",
			};
			if (ImGui::Combo("Surface Material", &currentPickedMaterial, INDICES.data(), static_cast<int>(INDICES.size())))
			{
				switch (currentPickedMaterial)
				{
				case 0: // air
					refractionIndex = 1.000293f;
					break;
				case 1: // hydrogen
					refractionIndex = 1.000132f;
					break;
				case 2: // water
					refractionIndex = 1.333f;
					break;
				case 3: // olive oil
					refractionIndex = 1.47f;
					break;
				case 4: // Ice
					refractionIndex = 1.31f;
					break;
				case 5: // Quartz
					refractionIndex = 1.46f;
					break;
				case 6: // Diamond 
					refractionIndex = 2.42f;
					break;
				case 7: // acrylic
				case 8: // plexiglas
				case 9: // lucite
					refractionIndex = 1.49f;
					break;
				}
			}
			ImGui::Separator();
			ImGui::Checkbox("Chromatic Aberration", &useChromaticAberration);
			ImGui::DragFloat("Chromatic Multiplier", &chromaticMultiplier, 0.0f, 0.01f, 1.f);
			ImGui::Separator();
			ImGui::DragFloat("Fresnel Power", &fresnelPower, 0.1f, 0.f, 10.f);

		}
		break;			
		}

		// for phong shading
		ImGui::Separator();
		ImGui::Checkbox("use phong shading", &usePhongShading);
		if (usePhongShading)
		{
			constexpr std::array TEXTURE_PROJECTION_MODE = {
			"Planer",
			"Spherical",
			"Cylindrical",
			};
			ImGui::Combo("Texture Projection Mode", &currentTexProjMode, TEXTURE_PROJECTION_MODE.data(), static_cast<int>(TEXTURE_PROJECTION_MODE.size()));
		}
	}

	if (ImGui::Button("Reload Shader"))
	{
		this->LoadShaders();
	}

	ImGui::End();
}

void Scene1::Reload()
{
	Clear();
	Init();
}

void Scene1::LoadShaders()
{
	basicShader.Delete();
	PhongLightingShader_CPU.Delete();
	PhongShadingShader_CPU.Delete();
	BlinnShadingShader_CPU.Delete();
	PhongLightingShader_GPU.Delete();
	PhongShadingShader_GPU.Delete();
	BlinnShadingShader_GPU.Delete();
	displayNormalShader.Delete();
	skyboxShader.Delete();
	EnvironmentalMappingShader.Delete();

	basicShader.Init("basicShader",
		"basicShader.vert",
		"basicShader.frag");

	PhongLightingShader_CPU.Init("PhongLightingShader_CPU",
		"PhongLighting.vert",
		"PhongLighting.frag");

	PhongShadingShader_CPU.Init("PhongShadingShader_CPU",
		"PhongShading.vert",
		"PhongShading.frag");

	BlinnShadingShader_CPU.Init("BlinnShadingShader_CPU",
		"PhongShading.vert",
		"BlinnShading.frag");

	PhongLightingShader_GPU.Init("PhongLightingShader_GPU",
		"PhongLighting_GPU.vert",
		"PhongLighting.frag");

	PhongShadingShader_GPU.Init("PhongShadingShader_GPU",
		"PhongShading_GPU.vert",
		"PhongShading.frag");

	BlinnShadingShader_GPU.Init("BlinnShadingShader_GPU",
		"PhongShading_GPU.vert",
		"BlinnShading.frag");

	displayNormalShader.Init("displayNormalShader",
		"displayNormal.vert",
		"basicShader.frag",
		"displayNormal.geom");

	skyboxShader.Init("skyboxShader",
		"skybox.vert", "skybox.frag");

	EnvironmentalMappingShader.Init("EnvironmentalMappingShader", "EnvironmentalMapping.vert", "EnvironmentalMapping.frag");// , "CalcNormal.geom");
}

void Scene1::SetUniformBuffer()
{
	// TEXTURE SETUP
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, diffuseTexture.textureID);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, specularTexture.textureID);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, whiteTexture.textureID);

	// uniform buffer set
	glBindBuffer(GL_UNIFORM_BUFFER, uniformBlockID);
	size_t i = 0;
	for (; i < spheres.size(); ++i)
	{
		glBufferSubData(GL_UNIFORM_BUFFER, uniformStructSize * i, sizeof(int), reinterpret_cast<int*>(&spheres[i]->lType));
		glBufferSubData(GL_UNIFORM_BUFFER, uniformStructSize * i + sizeof(float), sizeof(float), &(spheres[i]->innerAngle));
		glBufferSubData(GL_UNIFORM_BUFFER, uniformStructSize * i + sizeof(float) * 2, sizeof(float), &(spheres[i]->outerAngle));
		glBufferSubData(GL_UNIFORM_BUFFER, uniformStructSize * i + sizeof(float) * 3, sizeof(float), &(spheres[i]->fallOff));
		glBufferSubData(GL_UNIFORM_BUFFER, uniformStructSize * i + sizeof(glm::vec4), sizeof(glm::vec3), glm::value_ptr(spheres[i]->ambient));
		glBufferSubData(GL_UNIFORM_BUFFER, uniformStructSize * i + sizeof(glm::vec4) * 2, sizeof(glm::vec3), glm::value_ptr(spheres[i]->diffuse));
		glBufferSubData(GL_UNIFORM_BUFFER, uniformStructSize * i + sizeof(glm::vec4) * 3, sizeof(glm::vec3), glm::value_ptr(spheres[i]->specular));
		glBufferSubData(GL_UNIFORM_BUFFER, uniformStructSize * i + sizeof(glm::vec4) * 4, sizeof(glm::vec3), glm::value_ptr(spheres[i]->GetPosition()));
	}
	glBufferSubData(GL_UNIFORM_BUFFER, uniformStructSize * maxSphereNums, sizeof(int), &sphereNums);
	glBufferSubData(GL_UNIFORM_BUFFER, uniformStructSize * maxSphereNums + sizeof(glm::vec4), sizeof(glm::vec3), glm::value_ptr(cameraPosition));
	glBufferSubData(GL_UNIFORM_BUFFER, uniformStructSize * maxSphereNums + sizeof(glm::vec4) * 2, sizeof(glm::vec3), glm::value_ptr(ambientCo));
	glBufferSubData(GL_UNIFORM_BUFFER, uniformStructSize * maxSphereNums + sizeof(glm::vec4) * 3, sizeof(glm::vec3), glm::value_ptr(globalAmbient));
	glBufferSubData(GL_UNIFORM_BUFFER, uniformStructSize * maxSphereNums + sizeof(glm::vec4) * 4, sizeof(glm::vec3), glm::value_ptr(attenuation));
	glBufferSubData(GL_UNIFORM_BUFFER, uniformStructSize * maxSphereNums + sizeof(glm::vec4) * 5, sizeof(glm::vec3), glm::value_ptr(fog));
	glBufferSubData(GL_UNIFORM_BUFFER, uniformStructSize * maxSphereNums + sizeof(glm::vec4) * 6, sizeof(glm::vec2), glm::value_ptr(fog_near_far));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Scene1::SetupFBO()
{
	glGenTextures(6, renderedTexture);
	glGenRenderbuffers(6, depthrenderbuffer);
	glGenFramebuffers(6, frameBufferID);

	for (int i = 0; i < 6; ++i)
	{
		glBindTexture(GL_TEXTURE_2D, renderedTexture[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 512, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, (void*)0);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}
	for (int i = 0; i < 6; ++i)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID[i]);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTexture[i], 0);
		glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer[i]);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 512, 512);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer[i]);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Scene1::RenderToTexture()
{
	glViewport(0, 0, 512, 512);

	const glm::vec3 cameraAim[6] = {
		{-1.f, 0.f, 0.f},
		{1.f, 0.f, 0.f},
		{0.f, -1.f, 0.f},
		{0.f, 1.f, 0.f},
		{0.f, 0.f, -1.f},
		{0.f, 0.f, 1.f},
	};

	const glm::vec3 cameraUp[6] = {
		{0.f, 1.f, 0.f},
		{0.f, 1.f, 0.f},
		{0.f, 0.f, -1.f},
		{0.f, 0.f, 1.f},
		{0.f, 1.f, 0.f},
		{0.f, 1.f, 0.f},
	};

	for (int i = 0; i < 6; ++i)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID[i]);
		glClearColor(fog.x, fog.y, fog.z, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		GLenum drawBuffers[1] = { GL_COLOR_ATTACHMENT0 };

		GLuint numBuffers = 1;
		glDrawBuffers(numBuffers, drawBuffers);

		// framebuffer is incomplete if - color/depth not bound, diff dimensions, attachments not specified, unsupported format
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			throw std::runtime_error("Framebuffer is not completed!");
		}

		viewMatrix = glm::perspective(glm::half_pi<float>(), 1.f, 0.1f, 10.f)
			* glm::lookAt(glm::vec3{0.f}, cameraAim[i], cameraUp[i]);

		/////////////// SKYBOX /////////////
		glDepthMask(GL_FALSE);
		skyboxShader.Enable();

		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture.textureID);
		skyboxShader.SetUniform("viewMatrix", viewMatrix);
		skyboxShader.SetUniform("skyboxTexture", 3);
		skybox->Draw();
		glDepthMask(GL_TRUE);
		skyboxShader.Disable();
		/////////////// SKYBOX /////////////

		/////////////// DRAW LIGHT SPHERES //////////////
		basicShader.Enable();
		basicShader.SetUniform("viewMatrix", viewMatrix);
		for (LightSphere* sphere : spheres)
		{
			basicShader.SetUniform("inputColor", sphere->diffuse);
			basicShader.SetUniform("modelMatrix", sphere->GetModelMatrix());
			sphere->Draw();
		}

		glm::vec3 otherColor{ 1.f };
		basicShader.SetUniform("inputColor", otherColor);
		basicShader.SetUniform("modelMatrix", orbit->GetModelMatrix());
		orbit->Draw();
		basicShader.Disable();
		/////////////// DRAW LIGHT SPHERES //////////////
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, 1280, 720);
}