/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: GeometryScene.h
Purpose: Scene Header for CS350 assignment 2
Language: C++17, Visual Studio 2019
Platform: Visual Studio 2019 v16.11.2, Windows 10
Project: haewon.shon_CS350_2
Author: Haewon Shon (haewon.shon@digipen.edu, 180002920)
Creation date: Febuary 26, 2022
End Header --------------------------------------------------------*/

#include "GeometryScene.h" 
#include "Sphere.h"
#include "AABB.h"
#include "Ray.h"
#include "Plane.h"
#include "Point3D.h"
#include "Triangle.h"
#include "Intersections.h"
#include <glm/gtx/transform.hpp>
#include "imgui/imgui.h"
#include <array>

void GeometryScene::Init()
{
	LoadShaders();

	list.push_back(new AABB({ -1.f, -1.f, -1.f }, { 1.f, 1.f, 1.f }));
	list.back()->color = glm::vec3(1.f, 0.f, 0.f);
	list.push_back(new Sphere(glm::vec3{ 1.f, 1.f, 1.f }, 0.5f));
	list.back()->color = glm::vec3(0.f, 0.f, 1.f);

	projMatrix = glm::perspective(45.f, 1280.f / 720.f, 0.001f, 100.f);
}

void GeometryScene::Update([[maybe_unused]] float dt)
{
	viewMatrix = glm::lookAt(cam.pos,
		cam.pos + cam.lookAt,
		glm::vec3(0.f, 1.f, 0.f));
}

void GeometryScene::Clear()
{
	for (obj* obj : list)
	{
		if (obj) delete obj;
	}
	list.clear();
}

void GeometryScene::Draw()
{
	glClearColor(1.f, 1.f, 1.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Scene::basicShader.Enable();
	Scene::basicShader.SetUniform("inputColor", glm::vec3(0.f, 0.f, 1.f));
	Scene::basicShader.SetUniform("modelMatrix", glm::mat4(1.f));
	Scene::basicShader.SetUniform("viewMatrix", viewMatrix);
	Scene::basicShader.SetUniform("projMatrix", projMatrix);
	for (auto it = list.begin(); it != list.end(); ++it)
	{
		obj* result = TestCollision(it);
		if (result != nullptr)
		{
			Scene::basicShader.SetUniform("inputColor", result->color);
		}
		else
		{
			Scene::basicShader.SetUniform("inputColor", (*it)->color);
		}
		(*it)->Draw();
	}
	Scene::basicShader.Disable();
}

void GeometryScene::DrawGUI()
{
	ImGui::Begin("Control Panel");

	obj* toDelete = nullptr;
	int objNum = 0;
	for (obj* obj : list)
	{
		std::string objString = "Object " + std::to_string(objNum++ + 1);
		if (ImGui::TreeNode(objString.c_str()))
		{
			ImGui::Indent();
			switch (obj->type)
			{
			case obj::Type::AABB:
			{
				ImGui::BulletText("AABB");
				ImGui::ColorEdit3("color", &obj->color.x);
				AABB* aabb = dynamic_cast<AABB*>(obj);
				ImGui::Text("min : %f, %f, %f", aabb->min.x, aabb->min.y, aabb->min.z);
				ImGui::Text("max : %f, %f, %f", aabb->max.x, aabb->max.y, aabb->max.z);
				break;
			}
			case obj::Type::PLANE:
			{
				ImGui::BulletText("Plane");
				ImGui::ColorEdit3("color", &obj->color.x);
				Plane* plane = dynamic_cast<Plane*>(obj);
				ImGui::Text("normal : %f, %f, %f", plane->normal.x, plane->normal.y, plane->normal.z);
				ImGui::Text("d : %f", plane->normal.w);
				break;
			}
			case obj::Type::POINT3D:
			{
				ImGui::BulletText("Point3D");
				ImGui::ColorEdit3("color", &obj->color.x);
				Point3D* point = dynamic_cast<Point3D*>(obj);
				ImGui::Text("pos : %f, %f, %f", point->coordinate.x, point->coordinate.y, point->coordinate.z);
				ImGui::Text("size : %f", point->size);
				break;
			}
			case obj::Type::RAY:
			{
				ImGui::BulletText("Ray");
				ImGui::ColorEdit3("color", &obj->color.x);
				Ray* ray = dynamic_cast<Ray*>(obj);
				ImGui::Text("start : %f, %f, %f", ray->start.x, ray->start.y, ray->start.z);
				ImGui::Text("dir : %f, %f, %f", ray->direction.x, ray->direction.y, ray->direction.z);
				break;
			}
			case obj::Type::SPHERE:
			{
				ImGui::BulletText("Sphere");
				ImGui::ColorEdit3("color", &obj->color.x);
				Sphere* sphere = dynamic_cast<Sphere*>(obj);
				ImGui::Text("center : %f, %f, %f", sphere->position.coordinate.x, sphere->position.coordinate.y, sphere->position.coordinate.z);
				ImGui::Text("radius : %f", sphere->radius);
				break;
			}
			case obj::Type::TRIANGLE:
			{
				ImGui::BulletText("Triangle");
				ImGui::ColorEdit3("color", &obj->color.x);
				Triangle* triangle = dynamic_cast<Triangle*>(obj);
				ImGui::Text("p1 : %f, %f, %f", triangle->v1.coordinate.x, triangle->v1.coordinate.y, triangle->v1.coordinate.z);
				ImGui::Text("p1 : %f, %f, %f", triangle->v2.coordinate.x, triangle->v2.coordinate.y, triangle->v2.coordinate.z);
				ImGui::Text("p1 : %f, %f, %f", triangle->v3.coordinate.x, triangle->v3.coordinate.y, triangle->v3.coordinate.z);
				break;
			}
			}
			if (ImGui::Button("Delete"))
			{
				toDelete = obj;
			}
			ImGui::Unindent();
			ImGui::TreePop();
		}
	}
	if (toDelete != nullptr)
	{
		list.remove(toDelete);
	}

	ImGui::Separator();

	static int mode = 0;
	static glm::vec3 v1{};
	static glm::vec3 v2{};
	static glm::vec3 v3{};
	static float val{};
	static glm::vec3 col{ 1.f, 0.f, 0.f };
	constexpr std::array OBJ_TYPE = {
				"AABB",
				"Plane",
				"Point",
				"Ray",
				"Sphere",
				"Triangle"
	};
	ImGui::Combo("type", &mode, OBJ_TYPE.data(), static_cast<int>(OBJ_TYPE.size()));

	switch (mode)
	{
	case 0: // aabb
		ImGui::InputFloat3("min", &v1.x);
		ImGui::InputFloat3("max", &v2.x);
		ImGui::ColorEdit3("color", &col.x);
		if (ImGui::Button("Create new"))
		{
			list.push_back(new AABB(v1, v2));
			list.back()->color = col;
		}
		break;
	case 1: // plane
		ImGui::InputFloat3("normal", &v1.x);
		ImGui::InputFloat("d value", &val);
		ImGui::ColorEdit3("color", &col.x);
		if (ImGui::Button("Create new"))
		{
			list.push_back(new Plane(v1.x, v1.y, v1.z, val));
			list.back()->color = col;
		}
		break;
	case 2: // point
		ImGui::InputFloat3("position", &v1.x);
		ImGui::InputFloat("size", &val);
		ImGui::ColorEdit3("color", &col.x);
		if (ImGui::Button("Create new"))
		{
			list.push_back(new Point3D(v1.x, v1.y, v1.z, val));
			list.back()->color = col;
		}
		break;
	case 3: // ray
		ImGui::InputFloat3("start", &v1.x);
		ImGui::InputFloat3("direction", &v2.x);
		ImGui::ColorEdit3("color", &col.x);
		if (ImGui::Button("Create new"))
		{
			list.push_back(new Ray(v1, v2));
			list.back()->color = col;
		}
		break;
	case 4: // sphere
		ImGui::InputFloat3("center", &v1.x);
		ImGui::InputFloat("radius", &val);
		ImGui::ColorEdit3("color", &col.x);
		if (ImGui::Button("Create new"))
		{
			list.push_back(new Sphere(v1, val));
			list.back()->color = col;
		}
		break;
	case 5: // triangle
		ImGui::InputFloat3("v1", &v1.x);
		ImGui::InputFloat3("v2", &v2.x);
		ImGui::InputFloat3("v3", &v3.x);
		ImGui::ColorEdit3("color", &col.x);
		if (ImGui::Button("Create new"))
		{
			list.push_back(new Triangle(v1, v2, v3));
			list.back()->color = col;
		}
		break;
	}
	ImGui::End();
}

void GeometryScene::LoadShaders()
{
	Scene::LoadShaders();
}

// return collide obj
obj* GeometryScene::TestCollision(std::list<obj*>::iterator it)
{
	obj* object = *it;
	float t = 0.f;
	for (auto testIt = ++it; testIt != list.end(); ++testIt)
	{
		switch (object->type)
		{
		case obj::Type::AABB:
			switch ((*testIt)->type)
			{
			case obj::Type::SPHERE:
				if (SphereAABB(*dynamic_cast<Sphere*>(*testIt), *dynamic_cast<AABB*>(object)) == true) return *testIt;
				break;
			case obj::Type::POINT3D:
				if (PointAABB(*dynamic_cast<Point3D*>(*testIt), *dynamic_cast<AABB*>(object)) == true) return *testIt;
				break;
			case obj::Type::RAY:
				if (RayAABB(*dynamic_cast<Ray*>(*testIt), *dynamic_cast<AABB*>(object), t) == true) return *testIt;
				break;
			case obj::Type::PLANE:
				if (PlaneAABB(*dynamic_cast<Plane*>(*testIt), *dynamic_cast<AABB*>(object)) == true) return *testIt;
				break;
			case obj::Type::AABB:
				if (AABBAABB(*dynamic_cast<AABB*>(*testIt), *dynamic_cast<AABB*>(object)) == true) return *testIt;
				break;
			}
			break;
		case obj::Type::PLANE:
			switch ((*testIt)->type)
			{
			case obj::Type::SPHERE:
				if (PlaneSphere(*dynamic_cast<Plane*>(object), *dynamic_cast<Sphere*>(*testIt)) == true) return *testIt;
				break;
			case obj::Type::AABB:
				if (PlaneAABB(*dynamic_cast<Plane*>(object), *dynamic_cast<AABB*>(*testIt)) == true) return *testIt;
				break;
			case obj::Type::RAY:
				if (RayPlane(*dynamic_cast<Ray*>(*testIt), *dynamic_cast<Plane*>(object), t) == true) return *testIt;
				break;
			case obj::Type::POINT3D:
				if (PointPlane(*dynamic_cast<Point3D*>(*testIt), *dynamic_cast<Plane*>(object)) == true) return *testIt;
				break;
			}
			break;
		case obj::Type::RAY:
			switch ((*testIt)->type)
			{
			case obj::Type::SPHERE:
				if (RaySphere(*dynamic_cast<Ray*>(object), *dynamic_cast<Sphere*>(*testIt), t, t) == true) return *testIt;
				break;
			case obj::Type::AABB:
				if (RayAABB(*dynamic_cast<Ray*>(object), *dynamic_cast<AABB*>(*testIt), t) == true) return *testIt;
				break;
			case obj::Type::TRIANGLE:
				if (RayTriangle(*dynamic_cast<Ray*>(object), *dynamic_cast<Triangle*>(*testIt), t) == true) return *testIt;
				break;
			case obj::Type::PLANE:
				if (RayPlane(*dynamic_cast<Ray*>(object), *dynamic_cast<Plane*>(*testIt), t) == true) return *testIt;
				break;
			}
			break;
		case obj::Type::POINT3D:
			switch ((*testIt)->type)
			{
			case obj::Type::SPHERE:
				if (PointSphere(*dynamic_cast<Point3D*>(object), *dynamic_cast<Sphere*>(*testIt)) == true) return *testIt;
				break;
			case obj::Type::AABB:
				if (PointAABB(*dynamic_cast<Point3D*>(object), *dynamic_cast<AABB*>(*testIt)) == true) return *testIt;
				break;
			case obj::Type::TRIANGLE:
				if (PointTriangle(*dynamic_cast<Point3D*>(object), *dynamic_cast<Triangle*>(*testIt)) == true) return *testIt;
				break;
			case obj::Type::PLANE:
				if (PointPlane(*dynamic_cast<Point3D*>(object), *dynamic_cast<Plane*>(*testIt)) == true) return *testIt;
				break;
			}
			break;
		case obj::Type::SPHERE:
			switch ((*testIt)->type)
			{
			case obj::Type::SPHERE:
				if (SphereSphere(*dynamic_cast<Sphere*>(object), *dynamic_cast<Sphere*>(*testIt)) == true) return *testIt;
				break;
			case obj::Type::AABB:
				if (SphereAABB(*dynamic_cast<Sphere*>(object), *dynamic_cast<AABB*>(*testIt)) == true) return *testIt;
				break;
			case obj::Type::PLANE:
				if (PlaneSphere(*dynamic_cast<Plane*>(*testIt) , *dynamic_cast<Sphere*>(object)) == true) return *testIt;
				break;
			case obj::Type::RAY:
				if (RaySphere(*dynamic_cast<Ray*>(*testIt), *dynamic_cast<Sphere*>(object), t, t) == true) return *testIt;
				break;
			case obj::Type::POINT3D:
				if (PointSphere(*dynamic_cast<Point3D*>(*testIt), *dynamic_cast<Sphere*>(object)) == true) return *testIt;
				break;
			}
		case obj::Type::TRIANGLE:
			switch ((*testIt)->type)
			{
			case obj::Type::RAY:
				if (RayTriangle(*dynamic_cast<Ray*>(*testIt), *dynamic_cast<Triangle*>(object), t) == true) return *testIt;
				break;
			case obj::Type::POINT3D:
				if (PointTriangle(*dynamic_cast<Point3D*>(*testIt), *dynamic_cast<Triangle*>(object)) == true) return *testIt;
				break;
			}
		}
	}
	return nullptr;
}
