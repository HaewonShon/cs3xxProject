/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: ObjectConverter.h
Purpose: Header for the function receives file path and create a mesh based on the file.
Language: C++17, Visual Studio 2019
Platform: Visual Studio 2019 v16.11.2, Windows 10
Project: haewon.shon_CS300_1
Author: Haewon Shon (haewon.shon@digipen.edu, 180002920)
Creation date: October 1, 2021
End Header --------------------------------------------------------*/

#pragma once
#include <filesystem>
#include "Mesh.h"

Mesh* CreateObjectFromFile(const std::filesystem::path& filepath,
	bool useNomral = false, bool useTexture = false,
	Mesh::UVType uvType = Mesh::UVType::CYLINDRICAL, bool useNormalForTexture = false);