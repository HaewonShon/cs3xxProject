/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: ObjectConverter.cpp
Purpose: Source for the function receives file path and create a mesh based on the file.
Language: C++17, Visual Studio 2019
Platform: Visual Studio 2019 v16.11.2, Windows 10
Project: haewon.shon_CS300_1
Author: Haewon Shon (haewon.shon@digipen.edu, 180002920)
Creation date: October 1, 2021
End Header --------------------------------------------------------*/

#include "ObjectConverter.h"
#include <fstream>
#include <sstream>
#include "Mesh.h"

Mesh* CreateObjectFromFile(const std::filesystem::path& filepath, bool useNomral, bool useTexture, Mesh::UVType type, bool useNormalForTexture)
{
	std::ifstream objectFile(filepath.string(), std::ios::in);
	if (!objectFile.is_open())
	{
		throw std::runtime_error("Cannot open " + filepath.string());
	}

	std::stringstream objectStream;
	objectStream << objectFile.rdbuf();
	objectFile.close();
	
	Mesh* mesh = new Mesh();

	while (!objectStream.eof())
	{
		char type;
		objectStream >> type;

		switch (type)
		{
		case 'v': // vertex
		{             
			float x, y, z;
			objectStream >> x >> y >> z;
			mesh->AddVertex({ x, y, z });
			objectStream.ignore(INT_MAX, '\n');
			break;
		}
		case 'f': // face (index)
		{
			std::string line;
			std::getline(objectStream, line);
			std::stringstream lineStream(line);
			std::vector<unsigned int> faceIndices;
			while (!lineStream.eof())
			{
				unsigned int index;
				lineStream >> index;
				if (index == 0) break;
				faceIndices.push_back(index);

				// convert traingle fan into triangles - create new triangle with recent 3 indices
				size_t indicesNum = faceIndices.size();
				if (indicesNum >= 3)
				{
					mesh->AddIndex(faceIndices[0] - 1);
					mesh->AddIndex(faceIndices[indicesNum - 2] - 1);
					mesh->AddIndex(faceIndices[indicesNum - 1] - 1);
				}
			}
			break;
		}
		default:
		{
			objectStream.ignore(INT_MAX, '\n');
			break;
		}
		}
	}

	mesh->SetPrimitiveType(GL_TRIANGLES);
	mesh->UpdateModelNormalizeMatrix();
	mesh->Build(useNomral, useTexture, type, useNormalForTexture);

	return mesh;
}
