/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Demo.cpp
Purpose: Application header for the program
Language: C++17, Visual Studio 2019
Platform: Visual Studio 2019 v16.11.2, Windows 10
Project: haewon.shon_CS300_1
Author: Haewon Shon (haewon.shon@digipen.edu, 180002920)
Creation date: August 29, 2021
End Header --------------------------------------------------------*/

#pragma once

#include <memory>
#include "Scene.h"

struct SDL_Window;
class Demo
{
public:
	void Init();
	void Update() noexcept;
	void Draw() noexcept;
	inline bool IsRunning() const noexcept { return isRunning; }

	template <typename LEVEL>
	void CreateAndGoToLevel();

private:
	SDL_Window* window{nullptr};
	void* glContext{nullptr};
	bool isRunning{false};

	uint32_t timestamp;
	int mouseX{}; 
	int mouseY{};
	bool isMouseDown = false;

	std::unique_ptr<Scene> currentLevel;
};

template <typename LEVEL>
void Demo::CreateAndGoToLevel()
{
	currentLevel = std::make_unique<LEVEL>();
	currentLevel->Init();
}