/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: README.txt
Purpose: README instruction for the CS300 assignment 3
Language: text file
Platform: Notepad
Project: haewon.shon_CS300_3
Author: Haewon Shon (haewon.shon@digipen.edu, 180002920)
Creation date: 12/7/2021
End Header --------------------------------------------------------*/ 

a. How to use parts of your user interface that is NOT specified in the assignment description.
- Functionality about lighting, global variables are included in 'Scene Control' and 'Lights' tab in GUI.

b. Any assumption that you make on how to use the application that, if violated, might cause the application to fail.
- nothing

c. Which part of the assignment has been completed?
Skybox, environment mapping with skybox(including reflection, refraction, fresnel), combine it with phong shading

d. Which part of the assignment has NOT been completed (not done, not working, etc.) and explanation on why those parts are not completed?
I combined fresnel term with phong shading but not sure if it is correct.

e. Where the relevant source codes (both C++ and shaders) for the assignment are located. Specify the file path (folder name), file name, function name (or line number).
Drawing skybox : Levels/Scene1.cpp line 161~173, in Draw()
Render to texture : Levels/Scene1.cpp line 883 SetupFBO() and line 911 RenderToTexture()
Envrionment mapping : Levels/Scene1.cpp line 287~ in Draw(), Shaders/EnvironmenTalMapping.frag

f. Which machine did you test your application on.
OS : Windows 10
GPU : GeForce RTX 2060 Max-Q
OpenGL Driver : Geforce Driver 457.63

g. The number of hours you spent on the assignment, on a weekly basis
- For 20 hours per week, for last week of the submission and final week

h. Any other useful information pertaining to the application 
- nothing
