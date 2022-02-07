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

int const WIDTH = 1280;
int const HEIGHT = 720;

void Scene::Init()
{
	LoadShaders();

	models.push_back(new AssimpModel("..\\Models\\backpack\\backpack.obj"));

	// FSQ model 
	FSQ = new Mesh();
	FSQ->AddVertex({ 1.f, 1.f, 0.f });
	FSQ->AddVertex({ -1.f, 1.f, 0.f });
	FSQ->AddVertex({ -1.f, -1.f, 0.f });
	FSQ->AddVertex({ 1.f, -1.f, 0.f });
	FSQ->AddTextureCoord({ 1.f, 1.f });
	FSQ->AddTextureCoord({ 0.f, 1.f });
	FSQ->AddTextureCoord({ 0.f, 0.f });
	FSQ->AddTextureCoord({ 1.f, 0.f });
	FSQ->AddIndex(0);
	FSQ->AddIndex(1);
	FSQ->AddIndex(2);
	FSQ->AddIndex(0);
	FSQ->AddIndex(2);
	FSQ->AddIndex(3);
	FSQ->SetPrimitiveType(GL_TRIANGLES);
	FSQ->Build(true, true, Mesh::UVType::PLANER, true);//false, true);

	plane = new Mesh(*FSQ);
	plane->SetPosition({ 0.f, -1.f, 0.f });
	plane->SetRotation({ -glm::half_pi<float>(), 0.f, 0.f });
	plane->SetScale({5.f, 5.f, 1.f});

	/////////////// LIGHT SPHERES //////////////
	spheres.push_back(new LightSphere(orbitRadius, sphereSize, false));
	/////////////// LIGHT SPHERES //////////////

	/////////////// IMGUI INIT //////////////
	currentPickedObject = 0;

	/////////////// UNIFORM BLOCK SETUP //////////////
	FSQUniformIndex = glGetUniformBlockIndex(FSQShader.GetID(), "LightInfo");
	glUniformBlockBinding(FSQShader.GetID(), FSQUniformIndex, 0);

	glGenBuffers(1, &uniformBlockID);
	glBindBuffer(GL_UNIFORM_BUFFER, uniformBlockID);
	glBufferData(GL_UNIFORM_BUFFER, uniformStructSize * maxSphereNums + 16 * 6 + 8, NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, uniformBlockID, 0, uniformStructSize * maxSphereNums + 16 * 6 + 8);

	SetupGBuffer();
	projMatrix = glm::perspective(45.f, 1280.f / 720.f, 0.1f, 10.f);
};

void Scene::Update(float dt)
{
	viewMatrix = glm::lookAt(cam.pos,
			cam.pos + cam.lookAt,
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
	for (Mesh* mesh : debugObjects)
	{
		if (mesh)
		{
			delete mesh;
		}
	}
	debugObjects.clear();
	for (AssimpModel* mesh : models)
	{
		if (mesh)
		{
			delete mesh;
		}
	}
	models.clear();
	if(FSQ) delete FSQ;
	FSQ = nullptr;
	if(plane) delete plane;
	plane = nullptr;
	for (LightSphere* sphere : spheres)
	{
		if (sphere)
		{
			delete sphere;
		}
	}
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
	if (isCopyingDepth)
	{
		CopyDepthInfo();
	}

	RenderDebugObjects();
}

void Scene::DrawGUI()
{
	ImGui::Begin("Control Panel");

	if (ImGui::CollapsingHeader("Object"))
	{
		ImGui::Indent();
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
			//delete mainObject;
			//mainObject = CreateObjectFromFile(OBJECTS[currentPickedObject], true, true, static_cast<Mesh::UVType>(currentTexProjMode), currentPickedEntity == 0 ? false : true);
		}
		ImGui::Unindent();
	}

	if (ImGui::CollapsingHeader("Scene control"))
	{
		ImGui::Indent();
		if (ImGui::Button("Reload Scene"))
		{
			this->Reload();
		}
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

	if (ImGui::CollapsingHeader("Lights"))
	{
		ImGui::Indent();
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
			}
		}
		ImGui::Unindent();
	}

	if (ImGui::Button("Reload Shader"))
	{
		this->LoadShaders();
	}
	/*
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
	*/

	//if (ImGui::CollapsingHeader("Deferred Shading"))
	{
		ImGui::Checkbox("Draw FBO", &isDrawingBuffer);
		if(isDrawingBuffer == true)
		{
			constexpr std::array RENDER_TARGETS = {
				"Position",
				"Normal",
				"Diffuse",
				"Depth"
			};
			ImGui::Combo("Pick model to render", &bufferRenderTarget, RENDER_TARGETS.data(), static_cast<int>(RENDER_TARGETS.size()));
		}
		ImGui::Checkbox("Depth Copy", &isCopyingDepth);
		ImGui::Separator();
		ImGui::Checkbox("Draw Face Normal", &shouldDrawFaceNormal);
		ImGui::Checkbox("Draw Vertex Normal", &shouldDrawVertexNormal);
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
	gBufferShader.Delete();
	FSQShader.Delete();

	basicShader.Init("basicShader",
		"basicShader.vert",
		"basicShader.frag");

	PhongShadingShader.Init("PhongShadingShader",
		"PhongShading.vert",
		"PhongShading.frag");

	displayNormalShader.Init("displayNormalShader",
		"displayNormal.vert",
		"basicShader.frag",
		"displayNormal.geom");

	gBufferShader.Init("gBufferShader", "gBuffer.vert", "gBuffer.frag");
	FSQShader.Init("FSQ", "FSQ.vert", "FSQ.frag");
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
	glBufferSubData(GL_UNIFORM_BUFFER, uniformStructSize * maxSphereNums + sizeof(glm::vec4), sizeof(glm::vec3), glm::value_ptr(cam.pos));
	glBufferSubData(GL_UNIFORM_BUFFER, uniformStructSize * maxSphereNums + sizeof(glm::vec4) * 2, sizeof(glm::vec3), glm::value_ptr(ambientCo));
	glBufferSubData(GL_UNIFORM_BUFFER, uniformStructSize * maxSphereNums + sizeof(glm::vec4) * 3, sizeof(glm::vec3), glm::value_ptr(globalAmbient));
	glBufferSubData(GL_UNIFORM_BUFFER, uniformStructSize * maxSphereNums + sizeof(glm::vec4) * 4, sizeof(glm::vec3), glm::value_ptr(attenuation));
	glBufferSubData(GL_UNIFORM_BUFFER, uniformStructSize * maxSphereNums + sizeof(glm::vec4) * 5, sizeof(glm::vec3), glm::value_ptr(fog));
	glBufferSubData(GL_UNIFORM_BUFFER, uniformStructSize * maxSphereNums + sizeof(glm::vec4) * 6, sizeof(glm::vec2), glm::value_ptr(fog_near_far));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Scene::RenderDeferredObjects()
{
	// geometry pass
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
	glClearColor(0.f, 0.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	gBufferShader.Enable();
	gBufferShader.SetUniform("modelMatrix", glm::mat4(1.f));
	gBufferShader.SetUniform("viewMatrix", viewMatrix);
	gBufferShader.SetUniform("projMatrix", projMatrix);

	glBindTexture(GL_TEXTURE_2D, diffuseTexture.textureID);
	FSQShader.SetUniform("texture_diffuse", 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, specularTexture.textureID);
	FSQShader.SetUniform("texture_specular", 1);
	models[0]->Draw(gBufferShader);

	// plane
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, whiteTexture.textureID);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, whiteTexture.textureID);
	gBufferShader.SetUniform("modelMatrix", plane->GetModelMatrix());
	plane->Draw();

	gBufferShader.Disable();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glClearColor(fog.x, fog.y, fog.z, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	FSQShader.Enable(); 
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	FSQShader.SetUniform("gPosition", 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	FSQShader.SetUniform("gNormal", 1);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
	FSQShader.SetUniform("gAlbedoSpec", 2);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, gDepth);
	FSQShader.SetUniform("gDepth", 3);
	FSQShader.SetUniform("viewMatrix", viewMatrix);
	FSQShader.SetUniform("isDrawingBuffer", isDrawingBuffer);
	FSQShader.SetUniform("bufferRenderTarget", bufferRenderTarget);

	SetUniformBuffer();

	FSQ->Draw();
	FSQShader.Disable();
}

void Scene::RenderDebugObjects()
{
	basicShader.Enable();
	basicShader.SetUniform("viewMatrix", viewMatrix);
	basicShader.SetUniform("projMatrix", projMatrix);

	// draw lights
	for (LightSphere* light : spheres)
	{
		basicShader.SetUniform("modelMatrix", light->GetModelMatrix());
		basicShader.SetUniform("inputColor", light->diffuse);
		light->Draw();
	}
	basicShader.Disable();

	const glm::vec3 fNormalColor{ 0.f, 0.f, 1.f };
	const glm::vec3 vNormalColor = { 1.f, 0.f, 0.f };

	///////////// DRAW NORMAL VECTORS //////////////
	displayNormalShader.Enable();
	displayNormalShader.SetUniform("viewMatrix", viewMatrix);
	displayNormalShader.SetUniform("projMatrix", projMatrix);
	displayNormalShader.SetUniform("modelMatrix", glm::mat4(1.f));
	displayNormalShader.SetUniform("normalLength", normalLength);
	if (shouldDrawFaceNormal == true)
	{
		displayNormalShader.SetUniform("inputColor", fNormalColor);
		models[0]->DrawFaceNormal(displayNormalShader);
	}
	
	if (shouldDrawVertexNormal == true)
	{
		displayNormalShader.SetUniform("inputColor", vNormalColor);
		models[0]->DrawVertexNormal(displayNormalShader);
	}
	displayNormalShader.Disable();
	///////////// DRAW NORMAL VECTORS //////////////
}

void Scene::UpdateCamA(float dt)
{
	cam.pos -= glm::cross(cam.lookAt, glm::vec3(0.f, 1.f, 0.f)) * cam.moveSpeed * dt;
}

void Scene::UpdateCamS(float dt)
{
	cam.pos -= cam.lookAt * cam.moveSpeed * dt;
}

void Scene::UpdateCamD(float dt)
{
	cam.pos += glm::cross(cam.lookAt, glm::vec3(0.f, 1.f, 0.f)) * cam.moveSpeed * dt;
}

void Scene::UpdateCamW(float dt)
{
	cam.pos += cam.lookAt * cam.moveSpeed * dt;
}

void Scene::UpdateCamRotation(int xDiff, int yDiff)
{
	cam.theta += xDiff * cam.rotSpeed;
	cam.pi -= yDiff * cam.rotSpeed;

	if (cam.theta > 360.f) cam.theta -= 360.f;
	if (cam.theta < 0.f) cam.theta += 360.f;
	if (cam.pi > 89.f) cam.pi = 89.f;
	if (cam.pi < -89.f) cam.pi = -89.f;

	cam.lookAt.x = glm::cos(glm::radians(cam.theta));
	cam.lookAt.y = glm::sin(glm::radians(cam.pi));
	cam.lookAt.z = glm::sin(glm::radians(cam.theta));
}

void Scene::SetupGBuffer()
{
	glDeleteFramebuffers(1, &gBuffer);
	glGenFramebuffers(1, &gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

	//position
	glGenTextures(1, &gPosition);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, WIDTH, HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

	// normal 
	glGenTextures(1, &gNormal);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, WIDTH, HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

	// albedo + spec
	glGenTextures(1, &gAlbedoSpec);
	glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WIDTH, HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, gAlbedoSpec, 0);

	// depth
	glGenTextures(1, &gDepth);
	glBindTexture(GL_TEXTURE_2D, gDepth);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, WIDTH, HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gDepth, 0);

	// - tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
	unsigned int attachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };

	// depth buffer
	glGenRenderbuffers(1, &depth);
	glBindRenderbuffer(GL_RENDERBUFFER, depth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, WIDTH, HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth);

	glDrawBuffers(4, attachments);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		throw std::exception("FBO Setup for deferred shading is failed.");
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Scene::CopyDepthInfo()
{
	// bind the gbuffers for read
	glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
	// write to default framebuffer
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	// copy over depth info
	glBlitFramebuffer(
		0, 0, WIDTH, HEIGHT, // source region
		0, 0, WIDTH, HEIGHT, // destination region
		GL_DEPTH_BUFFER_BIT, // field to copy
		GL_NEAREST // filtering mechanism
	);
	// set the default framebuffer for draw
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}