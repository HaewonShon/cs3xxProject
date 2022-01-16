/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Demo.cpp
Purpose: Application source for the program
Language: C++17, Visual Studio 2019
Platform: Visual Studio 2019 v16.11.2, Windows 10
Project: haewon.shon_CS300_1
Author: Haewon Shon (haewon.shon@digipen.edu, 180002920)
Creation date: August 29, 2021
End Header --------------------------------------------------------*/

#include "Demo.h"
#include <SDL.h>
#include <stdexcept>
#include <glew.h>
#include <sstream>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_impl_sdl.h>

#include <iostream>

void Demo::Init()
{
    SDL_Init(SDL_INIT_VIDEO);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

    window = SDL_CreateWindow("CS350 Graphics Demo", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
        1280, 720, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

    if (window == nullptr)           
    {
        throw std::runtime_error{ "Failed to create window" };
    }

    glContext = SDL_GL_CreateContext(window);
    if (glContext == nullptr)
    {
        throw std::runtime_error{ "Failed to create openGL context" };
    }

    GLenum initResult = glewInit();
    if (initResult != GLEW_OK)
    {
        std::ostringstream msg;
        msg << "Failed to initialize glew : " << glewGetErrorString(initResult);
        throw std::runtime_error{ msg.str() };
    }

    timestamp = SDL_GetTicks();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glDepthFunc(GL_LESS);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplSDL2_InitForOpenGL(window, glContext);
    ImGui_ImplOpenGL3_Init("#version 400");

    SDL_GL_SetSwapInterval(1);

    isRunning = true;
}

void Demo::Update() noexcept
{
    SDL_Event event{ 0 };
    while (SDL_PollEvent(&event) != 0)
    {
        switch (event.type)
        {
        case SDL_QUIT:
            isRunning = false;
            break;
        case SDL_KEYDOWN:
            switch (event.key.keysym.scancode)
            {
            case SDL_SCANCODE_A:
                currentLevel->UpdateCamA();
                break;
            case SDL_SCANCODE_S:
                currentLevel->UpdateCamS();
                break;
            case SDL_SCANCODE_D:
                currentLevel->UpdateCamD();
                break;
            case SDL_SCANCODE_W:
                currentLevel->UpdateCamW();
                break;
            }
            break;
        }
    }

    uint32_t now = SDL_GetTicks();
    float dt = (float)(now - timestamp) / 1000.f;
    timestamp = now;

    currentLevel->Update(dt);
}

void Demo::Draw() noexcept
{
    currentLevel->Draw();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(window);
    ImGui::NewFrame();

    currentLevel->DrawGUI();

    ImGui::Render();
    SDL_GL_MakeCurrent(window, glContext);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(window);
}