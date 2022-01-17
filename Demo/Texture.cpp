/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Texture.cpp
Purpose: Source for OpenGL texture
Language: C++17, Visual Studio 2019
Platform: Visual Studio 2019 v16.11.2, Windows 10
Project: haewon.shon_CS300_2
Author: Haewon Shon (haewon.shon@digipen.edu, 180002920)
Creation date: November 5, 2021
End Header --------------------------------------------------------*/

#include "Texture.h"
#include <glew.h>
#include <sstream>
#include <fstream>
#include <iostream>

Texture::Texture(std::filesystem::path const& filepath)
{
    ReadFile(filepath);

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

    glBindTexture(GL_TEXTURE_2D, 0);

    delete[] data;
    data = nullptr;
}

Texture::~Texture()
{
    glDeleteTextures(1, &textureID);
}

void Texture::ReadFile(std::filesystem::path const& filepath)
{
    if (!filepath.has_filename())
    {
        throw std::runtime_error("Could not find texture file : " + filepath.string());
    }

	std::ifstream file(filepath);

	std::stringstream fileStream;
	fileStream << file.rdbuf();
	file.close();

    int phase = 0;
    int dataIndex = 0;
    while (phase < 3)
	{
        std::string line;
        std::getline(fileStream, line);

        if (line[0] == '#') // comment
        {
            continue;
        }

        switch (phase)
        {
        case 0: // magic number
            if (!(line == "P3" || line == "P6"))
            {
                throw std::runtime_error("ppm format is not correct : " + filepath.string());
            }
            phase = 1;
            break;

        case 1: // width, height
        {
            std::stringstream stream(line);
            stream >> width >> height;
            data = new unsigned char[width * height * 3 + 1];
            phase = 2;
        }
            break;
        case 2: // max value
            phase = 3;
            break;
        }
	}

    while (!fileStream.eof())
    {
        int val;
        fileStream >> val;
        data[dataIndex++] = static_cast<unsigned char>(val);
    }

    return;
}