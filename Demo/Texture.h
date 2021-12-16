/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Texture.h
Purpose: Header for OpenGL texture
Language: C++17, Visual Studio 2019
Platform: Visual Studio 2019 v16.11.2, Windows 10
Project: haewon.shon_CS300_2
Author: Haewon Shon (haewon.shon@digipen.edu, 180002920)
Creation date: November 5, 2021
End Header --------------------------------------------------------*/
#pragma once

#include <filesystem>
#include <glew.h>
#include <vector>

struct Texture
{
	Texture(std::filesystem::path const& filepath);
	Texture(std::vector<std::filesystem::path> const& filepaths);
	~Texture();
	void ReadFile(std::filesystem::path const& filepath);
	void ReadFile_PNG(std::filesystem::path const& filepath);
	GLuint textureID;
	int width;
	int height;
	int channels;
	unsigned char* data{ nullptr };
};