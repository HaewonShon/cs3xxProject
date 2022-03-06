/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: main.cpp
Purpose: main for the program which create Demo and run.
Language: C++17, Visual Studio 2019
Platform: Visual Studio 2019 v16.11.2, Windows 10
Project: haewon.shon_CS300_1
Author: Haewon Shon (haewon.shon@digipen.edu, 180002920)
Creation date: August 29, 2021
End Header --------------------------------------------------------*/

#include "Demo.h"
#include <stdexcept>
#include <iostream>
#include "Scene.h"
#include "GeometryScene.h"

int main()
try {
	{
		Demo demo;

		demo.Init();

		demo.CreateAndGoToLevel<GeometryScene>();

		while (demo.IsRunning())
		{
			demo.Update();
			demo.Draw();
		}
	}
}
catch (const std::exception& e)
{
	std::cerr << e.what() << std::endl;
}