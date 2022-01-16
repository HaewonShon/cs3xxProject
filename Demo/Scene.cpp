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

#include "Scene.h"
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui/imgui.h>
#include <array>
#include <string>
#include <iostream>
#include "Mesh.h"
#include "LightSphere.h"
#include "ObjectConverter.h"

void Scene::Init()
{
	/////////////// SHADER //////////////
	LoadShaders();

	/////////////// SHADER //////////////

	/////////////// MAIN OBJECT //////////////
	// main object in the center
	mainObject = CreateObjectFromFile("..\\Models\\bunny.obj", true, true, static_cast<Mesh::UVType>(currentTexProjMode), currentPickedEntity == 0 ? false : true);
	meshContainer.push_back(mainObject);

	/////////////// MAIN OBJECT //////////////

	/////////////// PLANE //////////////
	plane = CreateObjectFromFile("..\\Models\\quad.obj", true);
	plane->SetPosition({ 0.f, -1.f, 0.f });
	plane->SetRotation({ -glm::half_pi<float>(), 0.f, 0.f });
	plane->SetScale({ 5.f, 5.f, 1.f });
	meshContainer.push_back(plane);

	/////////////// PLANE //////////////

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
	phongShadingIndex = glGetUniformBlockIndex(PhongShadingShader.GetID(), "LightInfo");
	glUniformBlockBinding(PhongShadingShader.GetID(), phongShadingIndex, 0);

	glGenBuffers(1, &uniformBlockID);
	glBindBuffer(GL_UNIFORM_BUFFER, uniformBlockID);
	glBufferData(GL_UNIFORM_BUFFER, uniformStructSize * maxSphereNums + 16 * 6 + 8, NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, uniformBlockID, 0, uniformStructSize * maxSphereNums + 16 * 6 + 8);

	/////////////// TEXTURE SETUP //////////////
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, diffuseTexture.textureID);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, specularTexture.textureID);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, whiteTexture.textureID);
};

void Scene::Update(float dt)
{
	viewMatrix = glm::perspective(45.f, 1280.f / 720.f, 0.1f, 10.f)
		* glm::lookAt(cameraPosition,
			glm::vec3(0.f),
			glm::vec3(0.f, 1.f, 0.f));

	if (!isLightsRotate)
	{
		dt = 0.f;
	}
	for (LightSphere* sphere : spheres)
	{
		sphere->Update(dt);
	}
}

void Scene::Clear()
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

	glDeleteBuffers(1, &uniformBlockID);

	basicShader.Delete();
	PhongShadingShader.Delete();
	displayNormalShader.Delete();
}

void Scene::Draw()
{
	/////////////// SCENE CLEAR ///////////////
	glClearColor(fog.x, fog.y, fog.z, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	RenderDeferredObjects();

	// Copy depth buffer into default depth buffer

	//if(bCopyDepthInfo) Copy_Depth_Info();

	RenderDebugObjects();
}

void Scene::DrawGUI()
{
	ImGui::Begin("Control Panel");

	if (ImGui::CollapsingHeader("Object"))
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

		ImGui::Separator();
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

		constexpr std::array ENTITY = {
			"Position",
			"Normal",
		};
		if(ImGui::Combo("Texture Projection Entity", &currentPickedEntity, ENTITY.data(), static_cast<int>(ENTITY.size())))\
		{
			mainObject->RebuildUVs(static_cast<Mesh::UVType>(currentTexProjMode), currentPickedEntity == 0 ? false : true);
		}

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

	//if (ImGui::CollapsingHeader("Shader"))
	{
		if (ImGui::Button("Reload Shader"))
		{
			this->LoadShaders();
		}
	}

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
				light->ambient = glm::vec3{ (rand() % 256) / 255.f, (rand() % 256) / 255.f, (rand() % 256) / 255.f } * 0.05f;
				light->diffuse = glm::vec3{ (rand() % 256) / 255.f, (rand() % 256) / 255.f, (rand() % 256) / 255.f } * 0.1f;
				light->specular = glm::vec3{ (rand() % 256) / 255.f, (rand() % 256) / 255.f, (rand() % 256) / 255.f } * 0.1f;
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

	ImGui::End();
}

void Scene::Reload()
{
	Clear();
	Init();
}

void Scene::LoadShaders()
{
	basicShader.Delete();
	PhongShadingShader.Delete();
	displayNormalShader.Delete();

	basicShader.Init("basicShader",
		"basicShader.vert",
		"basicShader.frag");

	PhongShadingShader.Init("PhongShadingShader_CPU",
		"PhongShading.vert",
		"PhongShading.frag");

	displayNormalShader.Init("displayNormalShader",
		"displayNormal.vert",
		"basicShader.frag",
		"displayNormal.geom");
}

void Scene::SetUniformBuffer()
{
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

void Scene::RenderDeferredObjects()
{
	glm::vec3 otherColor{ 1.f };

	/////////////// LIGHT SHADER START //////////////
	PhongShadingShader.Enable();
	/////////////// DRAW MAIN OBJECT //////////////
	PhongShadingShader.SetUniform("viewMatrix", viewMatrix);
	PhongShadingShader.SetUniform("modelMatrix", mainObject->GetModelMatrix());

	PhongShadingShader.SetUniform("diffuseTexture", 0);
	PhongShadingShader.SetUniform("specularTexture", 1);
	mainObject->Draw();
	/////////////// DRAW MAIN OBJECT //////////////

	/////////////// DRAW PLANE OBJECT //////////////
	PhongShadingShader.SetUniform("diffuseTexture", 2);
	PhongShadingShader.SetUniform("specularTexture", 2);
	PhongShadingShader.SetUniform("modelMatrix", plane->GetModelMatrix());
	plane->Draw();

	SetUniformBuffer();
	/////////////// LIGHT SHADER END //////////////

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
}

void Scene::RenderDebugObjects()
{
	const glm::vec3 fNormalColor{ 0.f, 0.f, 1.f };
	const glm::vec3 vNormalColor = { 1.f, 0.f, 0.f };

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

void Scene::UpdateCamA() { cameraPosition.x -= 0.5; }
void Scene::UpdateCamS() { cameraPosition.y -= 0.5; }
void Scene::UpdateCamD() { cameraPosition.x += 0.5; }
void Scene::UpdateCamW() { cameraPosition.y += 0.5; }