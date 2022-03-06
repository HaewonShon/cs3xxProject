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

#pragma once
#include "Scene.h"
#include "obj.h"
#include <list>

struct Plane;
struct Ray;
struct AABB;
struct Point3D;
struct Triangle;
struct Sphere;

class GeometryScene : public Scene
{
public:
	void Init();
	void Update([[maybe_unused]] float dt);
	void Clear();
	void Draw();
	void DrawGUI();
	void LoadShaders();

private:
	obj* TestCollision(std::list<obj*>::iterator it);

	Shader basicShader;
	std::list<obj*> list;
};