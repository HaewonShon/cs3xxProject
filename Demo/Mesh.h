/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Mesh.h
Purpose: Header file for mesh class, including mesh transform and normals, etc.
Language: C++17, Visual Studio 2019
Platform: Visual Studio 2019 v16.11.2, Windows 10
Project: haewon.shon_CS300_1
Author: Haewon Shon (haewon.shon@digipen.edu, 180002920)
Creation date: September 21, 2021
End Header --------------------------------------------------------*/

#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <glew.h>

class Mesh
{
public:
	enum class UVType { PLANER, SPHERICAL, CYLINDRICAL };
public:
	Mesh() noexcept;
	Mesh(const Mesh& other);
	virtual ~Mesh();

	void Build(bool useNormal = false, bool useTexture = false, UVType type = UVType::CYLINDRICAL, bool useNormalForTexture = false) noexcept;	
	void Draw() noexcept;
	void DrawFaceNormal() noexcept;
	void DrawVertexNormal() noexcept;

	// For adding properties manually
	void AddVertex(const glm::vec3& vertex) noexcept;
	void AddTextureCoord(glm::vec2 texCoord) noexcept;
	void AddIndex(unsigned int index) noexcept;
	void AddFace(unsigned int index1, unsigned int index2, unsigned int index3) noexcept;
	void SetPrimitiveType(GLenum primitive) noexcept;

	// transform
	void SetPosition(const glm::vec3& newPosition) noexcept;
	void SetScale(const glm::vec3& newScale) noexcept;
	void SetRotation(const glm::vec3& newRotation) noexcept;
	void Translate(const glm::vec3 translate) noexcept;
	void Rotate(const float angle, const glm::vec3& axis) noexcept;

	inline const GLuint GetVAO() const noexcept { return vao; }
	inline const size_t GetVerticesSize() const noexcept { return verticesSize; }
	inline const size_t GetIndicesSize() const noexcept { return indicesSize; }
	inline const GLenum GetPrimitiveType() const noexcept { return primitiveType; }
	inline const glm::mat4& GetModelNormalizeMatrix() const noexcept { return modelNormalizeMatrix; }

	inline const glm::mat4& GetModelMatrix() const noexcept { return modelMatrix; }
	inline const glm::vec3& GetPosition() const noexcept { return position; }
	inline const glm::vec3& GetScale() const noexcept { return scale; }
	inline const glm::mat4& GetRotation() const noexcept { return rotation; }
	inline const glm::vec3& GetCenter() const noexcept { return center; }

	void UpdateModelNormalizeMatrix() noexcept;
	void RebuildUVs(UVType uvType, bool useNormalForTexture) noexcept;
protected:
	void UpdateMatrix() noexcept;
	void BuildModel(bool useNormal, bool useTexture) noexcept;
	void BuildFaceNormal() noexcept;
	void BuildVertexNormal() noexcept;
	void CalcUVs(UVType uvtype, bool useNormalForTexture) noexcept;

	std::vector<glm::vec3> vertices{};
	std::vector<glm::vec2> uvs{};
	std::vector<unsigned int> indices{};
	std::vector<glm::vec3> vertexNormals{};
	std::vector<glm::vec3> faceNormals{};
	size_t verticesSize{};
	size_t indicesSize{};
	GLenum primitiveType;

	std::vector<glm::vec3> faceNormalVertices{};
	std::vector<glm::vec3> vertexNormalVertices{};
	std::vector<unsigned int> normalIndices{};

	glm::vec3 min, max;
	glm::vec3 center;

	// transform
	glm::vec3 position{ 0.f };
	glm::vec3 scale{ 1.f };
	glm::mat4 rotation{ 1.f };
	glm::mat4 modelMatrix{ 1.f };
	glm::mat4 modelNormalizeMatrix{ 1.f };

	GLuint vao{}, vertexVBO{}, uvVBO{}, normalVbo{}, ebo{};
	GLuint faceNormalVao{}, faceNormalVbo{};
	GLuint vertexNormalVao{}, vertexNormalVbo{};
	GLuint normalEbo{};
};

namespace MESH
{
	Mesh* CreateCube();
	Mesh* CreateSphere();
};