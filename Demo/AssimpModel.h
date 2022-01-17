/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: AssimpModel.h
Purpose: Header for Model object created by assimp library
Language: C++17, Visual Studio 2019
Platform: Visual Studio 2019 v16.11.2, Windows 10
Project: haewon.shon_CS350_1
Author: Haewon Shon (haewon.shon@digipen.edu, 180002920)
Creation date: January 17, 2022
End Header --------------------------------------------------------*/
#pragma once

#include "AssimpMesh.h"
#include <vector>
#include <string>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Shader.h"

class AssimpModel
{
public:
    AssimpModel(std::string path)
    {
        loadModel(path);
    }
    void Draw(Shader& shader);
private:
    // model data
    std::vector<AssimpMesh> meshes;
    std::string directory;

    void loadModel(std::string path);
    void processNode(aiNode* node, const aiScene* scene);
    AssimpMesh processMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<SimpleTexture> loadMaterialTextures(aiMaterial* mat, aiTextureType type,
        std::string typeName);
};