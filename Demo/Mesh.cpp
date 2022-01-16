/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Mesh.cpp
Purpose: Source file for mesh class, including mesh transform and normals, etc.
Language: C++17, Visual Studio 2019
Platform: Visual Studio 2019 v16.11.2, Windows 10
Project: haewon.shon_CS300_1
Author: Haewon Shon (haewon.shon@digipen.edu, 180002920)
Creation date: September 21, 2021
End Header --------------------------------------------------------*/

#include "Mesh.h"
#include <numeric>
#include <glm/gtx/transform.hpp>
#include <set>
#include <glm/gtc/epsilon.hpp>

Mesh::Mesh() noexcept
{
	position = glm::vec3(0.f, 0.f, 0.f);
	scale = glm::vec3(1.f, 1.f, 1.f);
	rotation = glm::mat4(1.f);

	modelMatrix = glm::mat4(1.f);
}

Mesh::Mesh(const Mesh& other)
	: Mesh()
{
	if (other.verticesSize == 0)
	{
		return;
	}

	this->vao = other.vao;
	this->verticesSize = other.verticesSize;
	this->indicesSize = other.indicesSize;
	this->primitiveType = other.primitiveType;
	this->modelNormalizeMatrix = other.modelNormalizeMatrix;
}

Mesh::~Mesh()
{
	glDeleteBuffers(1, &vertexVBO);
	glDeleteBuffers(1, &normalVbo);
	glDeleteBuffers(1, &ebo);
	glDeleteBuffers(1, &faceNormalVbo);
	glDeleteBuffers(1, &vertexNormalVbo);
	glDeleteBuffers(1, &normalEbo);

	glDeleteVertexArrays(1, &vao);
	glDeleteVertexArrays(1, &faceNormalVao);
	glDeleteVertexArrays(1, &vertexNormalVao);
}

void Mesh::Build(bool useNormal, bool useTexture, UVType type, bool useNormalForTexture) noexcept
{
	verticesSize = vertices.size();
	indicesSize = indices.size();

	UpdateMatrix();

	if (useTexture == true)
	{
		CalcUVs(type, useNormalForTexture);
	}
	if (useNormal == true)
	{
		//prevent non-triangles model to build normal
		if (indices.size() % 3 == 0)
		{
			BuildFaceNormal();
			BuildVertexNormal();
		}
	}

	BuildModel(useNormal, useTexture);
}

void Mesh::Draw() noexcept
{
	glBindVertexArray(vao);
	glDrawElements(primitiveType, static_cast<GLsizei>(indicesSize), GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);
}

void Mesh::DrawFaceNormal() noexcept
{
	glBindVertexArray(faceNormalVao);
	glDrawElements(GL_LINES, static_cast<GLsizei>(normalIndices.size()), GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);
}

void Mesh::DrawVertexNormal() noexcept
{
	glBindVertexArray(vertexNormalVao);
	glDrawElements(GL_LINES, static_cast<GLsizei>(normalIndices.size()), GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);
}

void Mesh::AddVertex(const glm::vec3& vertex) noexcept
{
	vertices.push_back(vertex);
}

void Mesh::AddTextureCoord(glm::vec2 texCoord) noexcept
{
	uvs.push_back(texCoord);
}

void Mesh::AddIndex(unsigned int index) noexcept
{
	indices.push_back(index);
}

void Mesh::AddFace(unsigned int index1, unsigned int index2, unsigned int index3) noexcept
{
	indices.push_back(index1);
	indices.push_back(index2);
	indices.push_back(index3);
}

void Mesh::SetPrimitiveType(GLenum primitive) noexcept
{
	primitiveType = primitive;
}

void Mesh::SetPosition(const glm::vec3& newPosition) noexcept
{
	position = newPosition;
	UpdateMatrix();
}

void Mesh::SetScale(const glm::vec3& newScale) noexcept
{
	scale = newScale;
	UpdateMatrix();
}

//  x = about x axis, y = about y axis, z = about z axis
void Mesh::SetRotation(const glm::vec3& newRotation) noexcept
{
	rotation = glm::rotate(newRotation.x, glm::vec3{ 1.f, 0.f, 0.f })
		* glm::rotate(newRotation.y, glm::vec3{ 0.f, 1.f, 0.f })
		* glm::rotate(newRotation.z, glm::vec3{ 0.f, 0.f, 1.f });
	UpdateMatrix();
}

void Mesh::Translate(const glm::vec3 translate) noexcept
{
	position += translate;
	UpdateMatrix();
}

void Mesh::Rotate(const float angle, const glm::vec3& axis) noexcept
{
	rotation *= glm::rotate(angle, axis);
	UpdateMatrix();
}

void Mesh::UpdateModelNormalizeMatrix() noexcept
{
	min = glm::vec3{ std::numeric_limits<float>().max() };
	max = glm::vec3{ std::numeric_limits<float>().min() };

	for (glm::vec3& v : vertices)
	{
		if (v.x < min.x) min.x = v.x;
		if (v.y < min.y) min.y = v.y;
		if (v.z < min.z) min.z = v.z;
		if (v.x > max.x) max.x = v.x;
		if (v.y > max.y) max.y = v.y;
		if (v.z > max.z) max.z = v.z;
	}

	//glm::vec3 normalizedV = glm::vec3((v.x - min.x) / length.x,
	//	(v.y - min.y) / length.y,
	//	(v.z - min.z) / length.z);
	//normalizedV = 2.f * normalizedV - glm::vec3(1.f);

	glm::vec3 length = max - min;
	center = (min + max) / 2.f;
	modelNormalizeMatrix = glm::translate(glm::vec3(-1.f)) * glm::scale(glm::vec3(2.f / length)) * glm::translate(-min);
}

void Mesh::RebuildUVs(Mesh::UVType uvType, bool useNormalForTexture) noexcept
{
	CalcUVs(uvType, useNormalForTexture);
	BuildModel(true, true);
}

void Mesh::UpdateMatrix() noexcept
{
	modelMatrix = glm::translate(position)
		* rotation
		* glm::scale(scale)
		* modelNormalizeMatrix;
}

void Mesh::BuildModel(bool useNormal, bool useTexture) noexcept
{
	glCreateVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// vertex
	glCreateBuffers(1, &vertexVBO);
	glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * verticesSize, &vertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);


	// buffer normal data to main vao
	if (useNormal == true)
	{
		glCreateBuffers(1, &normalVbo);
		glBindBuffer(GL_ARRAY_BUFFER, normalVbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertexNormals.size(), &vertexNormals[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
	}

	if (useTexture == true)
	{
		// uvs
		glCreateBuffers(1, &uvVBO);
		glBindBuffer(GL_ARRAY_BUFFER, uvVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * verticesSize, &uvs[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
	}

	glCreateBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indicesSize, &indices[0], GL_STATIC_DRAW);
	glVertexArrayElementBuffer(vao, ebo);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Mesh::BuildFaceNormal() noexcept
{
	if (vertices.empty() == true) // vertices must be set first!
	{
		return;
	}

	// compute face normals
	faceNormalVertices.resize(indicesSize / 3 * 2); // one for center, one for center + normalstatic_cast<GLsizei>(
	for (size_t i = 0; i < indicesSize; i += 3)
	{
		const glm::vec3& v0 = vertices[indices[i]];
		const glm::vec3& v1 = vertices[indices[i + 1]];
		const glm::vec3& v2 = vertices[indices[i + 2]];

		// calculate normal vector
		glm::vec3 center = (v0 + v1 + v2) / 3.f;
		glm::vec3 vecA = v2 - v1;
		glm::vec3 vecB = v0 - v1;
		glm::vec3 normal = glm::normalize(glm::cross(vecA, vecB));
		faceNormalVertices[i / 3 * 2] = center; // center
		faceNormalVertices[i / 3 * 2 + 1] = center + normal / modelNormalizeMatrix[0][0];
	}

	normalIndices.resize(faceNormalVertices.size());
	for (unsigned int i = 0; i < static_cast<unsigned int>(normalIndices.size()); ++i)
	{
		normalIndices[i] = i;
	}

	glCreateVertexArrays(1, &faceNormalVao);
	glBindVertexArray(faceNormalVao);

	// for now vertices only
	glCreateBuffers(1, &faceNormalVbo);
	glBindBuffer(GL_ARRAY_BUFFER, faceNormalVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * faceNormalVertices.size(), &faceNormalVertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
	glEnableVertexAttribArray(0);

	glCreateBuffers(1, &normalEbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, normalEbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * normalIndices.size(), &normalIndices[0], GL_STATIC_DRAW);
	glVertexArrayElementBuffer(faceNormalVao, normalEbo);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
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

void Mesh::BuildVertexNormal() noexcept
{
	if (faceNormalVertices.empty()) // face normal must be set first!
	{
		return;
	}

	vertexNormals.resize(verticesSize);
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
		vertexNormalVertices[i * 2] = vertices[i];

		glm::vec3 normalVector{ 0.f };
		for (const glm::vec3& normal : vertexNormalSet[i])
		{
			normalVector += normal;
		}
		normalVector = glm::normalize(normalVector / static_cast<float>(vertexNormalSet.size()));
		vertexNormals[i] = normalVector;
		vertexNormalVertices[i * 2 + 1] = vertices[i] + normalVector / modelNormalizeMatrix[0][0];
	}

	glCreateVertexArrays(1, &vertexNormalVao);
	glBindVertexArray(vertexNormalVao);

	// for now vertices only
	glCreateBuffers(1, &vertexNormalVbo);
	glBindBuffer(GL_ARRAY_BUFFER, vertexNormalVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertexNormalVertices.size(), &vertexNormalVertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, normalEbo);
	glVertexArrayElementBuffer(vertexNormalVao, normalEbo);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Mesh::CalcUVs(UVType uvType, bool useNormalForTexture) noexcept
{
	// clear any existing UV
	uvs.clear();

	if (useNormalForTexture == true)
	{
		for (int i = 0; i < vertexNormals.size(); ++i)
		{
			glm::vec3 centroidVec = vertexNormals[i];
			glm::vec2 uv(0.0f);

			float theta(0.0f);
			float y(0.0f);
			float phi(0.0f);

			switch (uvType)
			{
			case Mesh::UVType::PLANER:
			{
				glm::vec3 absVec{ glm::abs(centroidVec.x), glm::abs(centroidVec.y), glm::abs(centroidVec.z) };
				if (absVec.x >= absVec.y && absVec.x >= absVec.z)
				{
					if (centroidVec.x < 0.f)
					{
						uv.x = centroidVec.z;
					}
					else
					{
						uv.x = -centroidVec.z;
					}
					uv.y = centroidVec.y;
				}
				else if (absVec.y >= absVec.x && absVec.y >= absVec.z)
				{
					if (centroidVec.y < 0.f)
					{
						uv.y = centroidVec.z;
					}
					else
					{
						uv.y = -centroidVec.z;
					}
					uv.x = centroidVec.x;
				}
				else if (absVec.z >= absVec.x && absVec.z >= absVec.y)
				{
					if (centroidVec.z < 0.f)
					{
						uv.x = -centroidVec.x;
					}
					else
					{
						uv.x = centroidVec.x;
					}
					uv.y = centroidVec.y;
				}
			}
			break;

			case Mesh::UVType::SPHERICAL:
				theta = glm::degrees(static_cast<float>(atan2(centroidVec.z, centroidVec.x)));
				theta += 180.0f;

				y = (centroidVec.y + 1.0f) * 0.5f;

				uv.x = theta / 360.0f;
				uv.y = y;
				break;

			case Mesh::UVType::CYLINDRICAL:
				theta = glm::degrees(static_cast<float>(glm::atan(centroidVec.z, centroidVec.x)));
				theta += 180.0f;

				y = centroidVec.y;
				phi = glm::degrees(glm::acos(y / centroidVec.length()));

				uv.x = theta / 360.0f;
				uv.y = 1.0f - (phi / 180.0f);
				break;
			}

			uvs.push_back(uv);
		}
	}
	else
	{
		glm::vec3 length = max - min;
		for (int i = 0; i < vertices.size(); ++i)
		{
			glm::vec3 v = vertices[i];
			glm::vec2 uv(0.0f);

			glm::vec3 centroidVec = glm::normalize(v - center);

			float theta(0.0f);
			float y(0.0f);
			float phi(0.0f);

			switch (uvType)
			{
			case Mesh::UVType::PLANER:
			{
				glm::vec3 absVec{ glm::abs(centroidVec.x), glm::abs(centroidVec.y), glm::abs(centroidVec.z) };
				if (absVec.x >= absVec.y && absVec.x >= absVec.z)
				{
					if (centroidVec.x < 0.f)
					{
						uv.x = centroidVec.z;
					}
					else
					{
						uv.x = -centroidVec.z;
					}
					uv.y = centroidVec.y;
				}
				else if (absVec.y >= absVec.x && absVec.y >= absVec.z)
				{
					if (centroidVec.y < 0.f)
					{
						uv.y = centroidVec.z;
					}
					else
					{
						uv.y = -centroidVec.z;
					}
					uv.x = centroidVec.x;
				}
				else if (absVec.z >= absVec.x && absVec.z >= absVec.y)
				{
					if (centroidVec.z < 0.f)
					{
						uv.x = -centroidVec.x;
					}
					else
					{
						uv.x = centroidVec.x;
					}
					uv.y = centroidVec.y;
				}
			}
				break;

			case Mesh::UVType::SPHERICAL:
				theta = glm::degrees(static_cast<float>(atan2(centroidVec.z, centroidVec.x)));
				theta += 180.0f;

				y = (centroidVec.y + 1.0f) * 0.5f;

				uv.x = theta / 360.0f;
				uv.y = y;
				break;

			case Mesh::UVType::CYLINDRICAL:
				theta = glm::degrees(static_cast<float>(glm::atan(centroidVec.z, centroidVec.x)));
				theta += 180.0f;

				y = centroidVec.y;
				phi = glm::degrees(glm::acos(y / centroidVec.length()));

				uv.x = theta / 360.0f;
				uv.y = 1.0f - (phi / 180.0f);
				break;
			}

			uvs.push_back(uv);
		}
	}
}

namespace MESH
{
	// Create cube mesh with side length 2, origin centered.
	Mesh* CreateCube()
	{
		Mesh* cubeMesh = new Mesh();
		glm::vec3 vertices[8] = {
			{1.f, 1.f, 1.f},
			{1.f, 1.f, -1.f},
			{1.f, -1.f, 1.f},
			{1.f, -1.f, -1.f},
			{-1.f, 1.f, 1.f},
			{-1.f, 1.f, -1.f},
			{-1.f, -1.f, 1.f},
			{-1.f, -1.f, -1.f},
		};

		for (int i = 0; i < 8; ++i)
		{
			cubeMesh->AddVertex(vertices[i]);
		}

		cubeMesh->AddFace(0, 2, 3);
		cubeMesh->AddFace(0, 3, 1);
		cubeMesh->AddFace(5, 4, 7);
		cubeMesh->AddFace(4, 3, 7);
		cubeMesh->AddFace(5, 4, 0);
		cubeMesh->AddFace(5, 0, 1);
		cubeMesh->AddFace(4, 5, 7);
		cubeMesh->AddFace(4, 7, 6);
		cubeMesh->AddFace(6, 7, 3);
		cubeMesh->AddFace(6, 3, 2);
		cubeMesh->AddFace(0, 2, 3);
		cubeMesh->AddFace(0, 3, 1);
		cubeMesh->AddFace(4, 6, 2);
		cubeMesh->AddFace(4, 2, 0);

		cubeMesh->SetPrimitiveType(GL_TRIANGLES);
		cubeMesh->Build();
		return cubeMesh;
	}

	// Create sphere mesh with radius 1, origin centered.
	Mesh* CreateSphere()
	{
		Mesh* sphereMesh = new Mesh();

		const float PI = 3.14159265359f;
		const float PI_over_10 = PI / 10.f;

		sphereMesh->AddVertex({ 0.f, 1.f, 0.f }); // top vertex
		for (float pi = PI_over_10; pi < PI; pi += PI_over_10)
		{
			float y = std::cos(pi);
			float radius = std::sin(pi);
			for (float theta = 0; theta < 2 * PI; theta += PI_over_10)
			{
				float x = radius * std::sin(theta);
				float z = radius * std::cos(theta);
				sphereMesh->AddVertex({ x, y, z });
			}
		}
		sphereMesh->AddVertex({ 0.f, -1.f, 0.f }); // bottom vertex

		for (int i = 1; i <= 19; ++i)
		{
			sphereMesh->AddFace(0, i, i + 1);
		}
		sphereMesh->AddFace(0, 20, 1);

		for (int i = 1; i <= 141; i += 20)
		{
			for (int j = 0; j < 19; ++j)
			{
				sphereMesh->AddFace(i + j, i + j + 1, i + j + 20);
				sphereMesh->AddFace(i + j + 20, i + j + 21, i + j + 1);
			}
			sphereMesh->AddFace(i + 19, i, i + 39);
			sphereMesh->AddFace(i + 39, i + 20, i);
		}

		for (int i = 1; i <= 19; ++i)
		{
			sphereMesh->AddFace(181, 160 + i, 160 + i + 1);
		}
		sphereMesh->AddFace(181, 180, 161);

		sphereMesh->SetPrimitiveType(GL_TRIANGLES);
		sphereMesh->Build();
		return sphereMesh;
	}
};

