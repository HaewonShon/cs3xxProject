/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Shader.cpp
Purpose: Source for the shader class including shader compiling and sending uniforms
Language: C++17, Visual Studio 2019
Platform: Visual Studio 2019 v16.11.2, Windows 10
Project: haewon.shon_CS300_1
Author: Haewon Shon (haewon.shon@digipen.edu, 180002920)
Creation date: September 3, 2021
End Header --------------------------------------------------------*/

#include "Shader.h"
#include <glew.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

constexpr unsigned int COMPILE_FAILED = 0xffffffff;

void Shader::Init(const std::string& shaderName,
    const std::filesystem::path& vsPath,
    const std::filesystem::path& fsPath,
    const std::filesystem::path& gsPath,
    const std::filesystem::path& tcsPath,
    const std::filesystem::path& tesPath)
try {
    {
        if (shaderID > 0)
        {
            Delete();
        }

        name = shaderName;

        // create shader program
        shaderID = glCreateProgram();
        if (shaderID <= 0)
        {
            throw ShaderException(this, "Failed to create program");
        }

        // create shaders
        GLint vShader = CompileShader(vsPath, GL_VERTEX_SHADER);
        GLint fShader = CompileShader(fsPath, GL_FRAGMENT_SHADER);
        GLint gShader = CompileShader(gsPath, GL_GEOMETRY_SHADER);
        GLint tcShader = CompileShader(tcsPath, GL_TESS_CONTROL_SHADER);
        GLint teShader = CompileShader(tesPath, GL_TESS_EVALUATION_SHADER);

        if (vShader == COMPILE_FAILED
            || fShader == COMPILE_FAILED
            || gShader == COMPILE_FAILED
            || tcShader == COMPILE_FAILED
            || teShader == COMPILE_FAILED)
        {
            glDeleteShader(vShader);
            glDeleteShader(fShader);
            glDeleteShader(gShader);
            glDeleteShader(tcShader);
            glDeleteShader(teShader);
            glDeleteProgram(shaderID);
            shaderID = 0;
            return;
        }

        // link shaders
        glLinkProgram(shaderID);

        glDeleteShader(vShader);
        glDeleteShader(fShader);
        glDeleteShader(gShader);
        glDeleteShader(tcShader);
        glDeleteShader(teShader);

        GLint isLinked;
        glGetShaderiv(shaderID, GL_LINK_STATUS, &isLinked);
        if (isLinked == GL_FALSE)
        {
            GLint logLength = 0;
            glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &logLength);
            std::vector<char> log(logLength);
            glGetShaderInfoLog(shaderID, logLength, nullptr, &log[0]);
            throw ShaderException(this, "Failed to Link program");
        }

        // check if the program is valid
        glValidateProgram(shaderID);
        GLint isValid;
        glGetShaderiv(shaderID, GL_LINK_STATUS, &isValid);
        if (isValid == GL_FALSE)
        {
            GLint logLength = 0;
            glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &logLength);
            std::vector<char> log(logLength);
            glGetShaderInfoLog(shaderID, logLength, nullptr, &log[0]);
            throw ShaderException(this, "Failed to validate program");
        }
    }
}
catch (ShaderException& e)
{
    std::cerr<<e.what()<<std::endl;
}

void Shader::Delete() noexcept
{
    glDeleteShader(shaderID);     
    shaderID = 0;
}

void Shader::Enable() const noexcept
{
    glUseProgram(shaderID);
}

void Shader::Disable() const noexcept
{
    glUseProgram(0);
}

void Shader::SetUniform(const char* uniformName, int value) const noexcept
{
    glUniform1i(GetUniformLocation(uniformName), value);
}

void Shader::SetUniform(const char* uniformName, float value) const noexcept
{
    glUniform1f(GetUniformLocation(uniformName), value);
}

void Shader::SetUniform(const char* uniformName, const glm::vec3& vec) const noexcept
{
    glUniform3f(GetUniformLocation(uniformName), vec.x, vec.y, vec.z);
}

void Shader::SetUniform(const char* uniformName, const glm::vec4& vec) const noexcept
{
    glUniform4f(GetUniformLocation(uniformName), vec.x, vec.y, vec.z, vec.w);
}

void Shader::SetUniform(const char* uniformName, const glm::mat4& mat) const noexcept
{
    glUniformMatrix4fv(GetUniformLocation(uniformName), 1, GL_FALSE, &(mat[0][0]));
}

GLuint Shader::CompileShader(const std::filesystem::path& shaderPath, GLenum shaderType)
{
    if (shaderPath == "")
    {
        return 0;
    }

    std::ifstream shaderFile("..\\Shaders\\" + shaderPath.string(), std::ios::in);
    if (!shaderFile.is_open())
    {
        throw ShaderException(this, shaderPath.string() + ": Failed to open ");
    }

    std::stringstream shaderStream;
    shaderStream << shaderFile.rdbuf();
    shaderFile.close();

    GLuint shader = glCreateShader(shaderType);
    if (shader <= 0)
    {
        throw ShaderException(this, shaderPath.string() + " Failed to create shader");
    }

    std::string str = shaderStream.str();
    GLchar const* shaderSource = str.c_str();
    glShaderSource(shader, 1, &shaderSource, nullptr);
    glCompileShader(shader);

    GLint isCompiled;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE)
    {
        GLint logLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
        std::vector<char> log(logLength);
        glGetShaderInfoLog(shader, logLength, nullptr, &log[0]);
        glDeleteShader(shader);
        throw ShaderException(this, shaderPath.string() + " Compile failed - " + std::string(log.data()));
    }
    glAttachShader(shaderID, shader);
    return shader;
}

int Shader::GetUniformLocation(const char* uniformName) const noexcept
{
    return glGetUniformLocation(shaderID, uniformName);
}

/////////////////////////////////////////////////////////////////////
///////                ShaderException                      /////////
/////////////////////////////////////////////////////////////////////

ShaderException::ShaderException(Shader* shader, const std::string& text)
    : std::exception(text.c_str()), shaderName(shader->GetName())
{

    errorMsg = "[" + shaderName + "] " + std::string(std::exception::what());
}

const char* ShaderException::what()
{
    return errorMsg.c_str();
}