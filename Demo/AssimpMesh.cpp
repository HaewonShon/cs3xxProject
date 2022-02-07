/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: AssimpMesh.cpp
Purpose: Source for Mesh created by assimp library
Language: C++17, Visual Studio 2019
Platform: Visual Studio 2019 v16.11.2, Windows 10
Project: haewon.shon_CS350_1
Author: Haewon Shon (haewon.shon@digipen.edu, 180002920)
Creation date: January 17, 2022
End Header --------------------------------------------------------*/

#include "AssimpMesh.h"
#include <set>
#include <glew.h>

AssimpMesh::AssimpMesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<SimpleTexture> textures)
{
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;

    Setup();

    BuildNormal();
}

void AssimpMesh::Setup()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
        &indices[0], GL_STATIC_DRAW);

    // vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    // vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

    glBindVertexArray(0);
}

void AssimpMesh::Draw(Shader& shader)
{
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    for (int i = 0; i < textures.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i); // activate proper texture unit before binding
        // retrieve texture number (the N in diffuse_textureN)
        std::string number;
        std::string name = textures[i].type;
        if (name == "texture_diffuse")
            number = std::to_string(diffuseNr++);
        else if (name == "texture_specular")
            number = std::to_string(specularNr++);
        std::string matName("material." + name + number);
        shader.SetUniform(matName.c_str(), i);
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }
    glActiveTexture(GL_TEXTURE0);

    //shader.SetUniform("parentMatrix", parentTransform);

    // draw mesh
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void AssimpMesh::DrawFaceNormal(Shader& shader)
{
    glBindVertexArray(fNormalVAO);
    glDrawArrays(GL_LINES, 0, indices.size() / 3 * 2);
    glBindVertexArray(0);
}

void AssimpMesh::DrawVertexNormal(Shader& shader)
{
    glBindVertexArray(vNormalVAO);
    glDrawArrays(GL_LINES, 0, vertices.size() * 2);
    glBindVertexArray(0);
}

struct compareVec3
{
    bool operator() (const glm::vec3& lhs, const glm::vec3& rhs) const
    {
        if (lhs.x == rhs.x)
        {
            if (lhs.y == rhs.y)
            {
                return lhs.z < rhs.z;
            }
            else
            {
                return lhs.y < rhs.y;
            }
        }
        else
        {
            return lhs.x < rhs.x;
        }
    }
};

void AssimpMesh::BuildNormal()
{
    // face nomral
    if (vertices.empty() == true) return;

    int verticesSize = vertices.size();
    int indicesSize = indices.size();

    // compute face normals
    std::vector<glm::vec3> faceNormalVertices;
    faceNormalVertices.resize(indicesSize / 3 * 2); // one for center, one for center + normalstatic_cast<GLsizei>(
    for (size_t i = 0; i < indicesSize; i += 3)
    {
        const glm::vec3& v0 = vertices[indices[i]].Position;
        const glm::vec3& v1 = vertices[indices[i + 1]].Position;
        const glm::vec3& v2 = vertices[indices[i + 2]].Position;

        // calculate normal vector
        glm::vec3 center = (v0 + v1 + v2) / 3.f;
        glm::vec3 vecA = v2 - v1;
        glm::vec3 vecB = v0 - v1;
        glm::vec3 normal = glm::normalize(glm::cross(vecA, vecB));
        faceNormalVertices[i / 3 * 2] = center; // center
        faceNormalVertices[i / 3 * 2 + 1] = center + normal;
    }

    if (faceNormalVertices.empty()) // face normal must be set first!
    {
        return;
    }

    // vertex normal
    std::vector<glm::vec3> vertexNormalVertices;
    vertexNormalVertices.resize(verticesSize * 2);
    std::vector<std::set<glm::vec3, compareVec3>> vertexNormalSet(verticesSize); // key = vertices
    // compute vertex normals
    for (size_t i = 0; i < indices.size(); i += 3)
    {
        glm::vec3 normal = faceNormalVertices[(i / 3) * 2 + 1] - faceNormalVertices[(i / 3) * 2];
        vertexNormalSet[indices[i]].insert(normal);
        vertexNormalSet[indices[i + 1]].insert(normal);
        vertexNormalSet[indices[i + 2]].insert(normal);
    }

    for (size_t i = 0; i < verticesSize; ++i)
    {
        vertexNormalVertices[i * 2] = vertices[i].Position;

        glm::vec3 normalVector{ 0.f };
        for (const glm::vec3& normal : vertexNormalSet[i])
        {
            normalVector += normal;
        }
        normalVector = glm::normalize(normalVector / static_cast<float>(vertexNormalSet.size()));
        vertexNormalVertices[i * 2 + 1] = vertices[i].Position + normalVector;
    }

    unsigned int fVBO, vVBO;
    glCreateVertexArrays(1, &fNormalVAO);
    glBindVertexArray(fNormalVAO);

    glCreateBuffers(1, &fVBO);
    glBindBuffer(GL_ARRAY_BUFFER, fVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * faceNormalVertices.size(), &faceNormalVertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glCreateVertexArrays(1, &vNormalVAO);
    glBindVertexArray(vNormalVAO);

    glCreateBuffers(1, &vVBO);
    glBindBuffer(GL_ARRAY_BUFFER, vVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertexNormalVertices.size(), &vertexNormalVertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}