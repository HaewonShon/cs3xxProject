/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: README.txt
Purpose: README instruction for the CS350 assignment 1
Language: text file
Project: haewon.shon_CS350_3
Author: Haewon Shon (haewon.shon@digipen.edu, 180002920)
Creation date: 2/6/2022
End Header --------------------------------------------------------*/ 

a. How to use parts of your user interface that is NOT specified in the assignment description.
Can control lights and global variables with GUI-Scene Control section.
Can move camera's lookat direction by dragging while holding mouse right click.

b. Any assumption that you make on how to use the application that, if violated, might cause the application to fail.
Reading obj files without normals might fail the program.

c. Which part of the assignment has been completed?
Phong shading with deferred rendering and forward rendering for debug objects(normals).
Obj format file loading
First-person camera
G-buffer target rendering
toggling depth copying

d. Which part of the assignment has NOT been completed (not done, not working, etc.) and explanation on why those parts are not completed?
none.

e. Where the relevant source codes (both C++ and shaders) for the assignment are located. Specify the file path (folder name), file name, function name (or line number).

Hybrid Rendering : Shader- Shader/gBuffer.vert&frag, Shader/FSQ.vert/frag.
                   Gbuffer Setup : Scene.cpp line 568~
                   Deferred Rendering : Scene.cpp line 439~
                   Forward Rendering : Scene.cpp line 494~
Obj format file loading : AssimpModel.cpp
First-person camera : Scene.cpp line 533~
G-buffer target rendering : Shader/FSQ.frag line 122~
toggling depth copying : Scene.cpp line 142

f. Which machine did you test your application on.
OS : Windows 10
GPU : GeForce RTX 2060 Max-Q
OpenGL Driver : Geforce Driver 457.63

g. The number of hours you spent on the assignment, on a weekly basis
Week 4 : ~3 hours
Week 5 : ~20 hours

h. Any other useful information pertaining to the application 
none.