/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: IScene.h
Purpose: Scene interface header.
Language: C++17, Visual Studio 2019
Platform: Visual Studio 2019 v16.11.2, Windows 10
Project: haewon.shon_CS300_1
Author: Haewon Shon (haewon.shon@digipen.edu, 180002920)
Creation date: August 29, 2021
End Header --------------------------------------------------------*/

#pragma once

class IScene
{
public:
	virtual void Init() = 0;
	virtual void Update([[maybe_unused]] float dt) = 0;
	virtual void Clear() = 0;
	virtual void Draw() = 0;
	virtual void DrawGUI() = 0;

	virtual void Reload() = 0;

	IScene() = default;
	virtual ~IScene() = default;
	IScene(const IScene&) = delete;
	IScene(IScene&&) = delete;
	IScene& operator=(const IScene&) = delete;
	IScene& operator=(IScene&&) = delete;
};