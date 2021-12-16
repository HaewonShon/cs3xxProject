/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Shader.h
Purpose: Header for the shader class including shader compiling and sending uniforms
Language: C++17, Visual Studio 2019
Platform: Visual Studio 2019 v16.11.2, Windows 10
Project: haewon.shon_CS300_1
Author: Haewon Shon (haewon.shon@digipen.edu, 180002920)
Creation date: September 3, 2021
End Header --------------------------------------------------------*/

#pragma once
#include <filesystem>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glew.h>
#include <stdexcept>

class Shader
{
public:
    ~Shader() { Delete(); }
    void Init(const std::string& shaderName,
        const std::filesystem::path& vsPath,
        const std::filesystem::path& fsPath,
        const std::filesystem::path& gsPath = "",
        const std::filesystem::path& tcsPath = "",
        const std::filesystem::path& tesPath = "");
    void Delete() noexcept;

    void Enable() const noexcept;
    void Disable() const noexcept;

    void SetUniform(const char* name, bool value) const noexcept;
    void SetUniform(const char* name, int value) const noexcept;
    void SetUniform(const char* name, float value) const noexcept;
    void SetUniform(const char* name, const glm::vec3& vec) const noexcept;
    void SetUniform(const char* name, const glm::vec4& vec) const noexcept;
    void SetUniform(const char* name, const glm::mat4& mat) const noexcept;

    inline const std::string& GetName() const noexcept { return name; }
    inline const GLuint GetID() const noexcept { return shaderID; }
private:
    
    GLuint CompileShader(const std::filesystem::path& shaderPath, GLenum shaderType);
    int GetUniformLocation(const char* name) const noexcept;

    GLuint shaderID;
    std::string name;
};

class ShaderException : public std::exception
{
public:
    ShaderException(Shader* shader, const std::string& text);
    const char* what();
private:
    std::string shaderName;
    std::string errorMsg;
};