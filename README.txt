/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: README.txt
Purpose: README instruction for the CS300 assignment 2
Language: text file
Platform: Visual Studio 2019 v16.11.2, Windows 10
Project: haewon.shon_CS300_2
Author: Haewon Shon (haewon.shon@digipen.edu, 180002920)
Creation date: 11/7/2021
End Header --------------------------------------------------------*/ 

a. How to use parts of your user interface that is NOT specified in the assignment description.
Can control object scale/rotation in Object section in GUI.
Can move camera's position in Scene Control section.

b. Any assumption that you make on how to use the application that, if violated, might cause the application to fail.

If the program tries to make a model from certain object files(ex. rhino.obj, starwars1.obj) it will crash, since my program cannot import them properly.
Also some texture files might break the program, for non-ppm files or ppm file with different contents.

c. Which part of the assignment has been completed?

3 Shaders - Phong lighting, Phong shading, Blinn shading with uniform blocks, with global variables
Scene setup with rotating lights, with 3 types of light, their own ambient/diffuse/specular color
3 scenarios required in the assignment
Rendering with textures for material - including uv calculation with position/normal
Modifying light property with GUI

d. Which part of the assignment has NOT been completed (not done, not working, etc.) and explanation on why those parts are not completed?

Could not implemented GPU uv calculation for Blinn Shading and Phong Shading. They are a lot different compared to provided sample.

e. Where the relevant source codes (both C++ and shaders) for the assignment are located. Specify the file path (folder name), file name, function name (or line number).

Phong Lighting : Shaders/PhongLighting.vert, Shaders/PhongLighting.frag
Phong Shading : Shaders/PhongShading.vert, Shaders/PhongShading.frag
Blinn Shaindg : Shaders/BlinnShading.frag

Scene light setup : Demo/Scene1.cpp line 49(initial setup), Demo/Scene1.cpp line 461(setting using GUI)
light property : Demo/LightSphere.h line 28~, including global variables, controlled by GUI : Demo/Scene1.cpp line 427~
light type support : Shaders/PhongLighting.vert line  52~, Shaders/PhongShading.frag line  49~, Shaders/BlinnShading.frag line 49~

Scenario : Demo/Scene1.cpp line 517~
Using uniform block : Setup for Demo/Scene1.cpp line 73~, Buffering for Demo/Scene1.cpp line 665~

Texture reading : Demo/Texture.cpp line 23~
UV calculation on CPU : Demo/Mesh.cpp line 370~

f. Which machine did you test your application on.
OS : Windows 10
GPU : GeForce RTX 2060 Max-Q
OpenGL Driver : Geforce Driver 457.63

g. The number of hours you spent on the assignment, on a weekly basis
Week 10 : ~30 hours

h. Any other useful information pertaining to the application 
none.

[CPU/GPU UV Calcuation comparision]
Tested with proivded sample.
It seems they has no big difference but for planer UV mapping the part near to the corner of the cubemap(|x| similiar to |y|) it seems they have some differences.