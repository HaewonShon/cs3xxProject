/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: AssimpMesh.h
Purpose: Header for Mesh created by assimp library
Language: C++17, Visual Studio 2019
Platform: Visual Studio 2019 v16.11.2, Windows 10
Project: haewon.shon_CS350_1
Author: Haewon Shon (haewon.shon@digipen.edu, 180002920)
Creation date: January 17, 2022
End Header --------------------------------------------------------*/
#pragma once

#include <glm/glm.hpp>
#include <string>
#include <vector>
#include "Shader.h"

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

struct SimpleTexture {
    unsigned int id;
    std::string type;
};

class AssimpMesh {
public:
    // mesh data
    std::vector<Vertex>       vertices;
    std::vector<unsigned int> indices;
    std::vector<SimpleTexture>      textures;

    AssimpMesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<SimpleTexture> textures);
    void Draw(Shader& shader);
private:
    //  render data
    unsigned int VAO, VBO, EBO;

    void setupMesh();
};