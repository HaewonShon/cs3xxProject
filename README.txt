/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: README.txt
Purpose: README instruction for the CS350 assignment 2
Language: text file
Project: haewon.shon_CS350_2
Author: Haewon Shon (haewon.shon@digipen.edu, 180002920)
Creation date: 3/6/2022
End Header --------------------------------------------------------*/ 

a. How to use parts of your user interface that is NOT specified in the assignment description.
Can control the camera using WASD and right-mouse button drag.
Can add primitives using GUI. Colliding primitives will be displayed with same color. (has issue so testing with 2 primitives only will show correct result.)

b. Any assumption that you make on how to use the application that, if violated, might cause the application to fail.
giving unreasonable values when creating primitive will fail the program. 

c. Which part of the assignment has been completed?
object loading, rendering with shaders(The project has files from previous assingment)
Implement collision testing functions
Camera implementation
Displaying primitives

d. Which part of the assignment has NOT been completed (not done, not working, etc.) and explanation on why those parts are not completed?
none.

e. Where the relevant source codes (both C++ and shaders) for the assignment are located. Specify the file path (folder name), file name, function name (or line number).
Collision test functions : intersection.cpp/h
Camera : Scene.cpp line 530~
Displaying primitives
- for each primitive : has file (ex. AABB.cpp/h)
- Drawing : GeometryScene.cpp/h

f. Which machine did you test your application on.
OS : Windows 10
GPU : GeForce RTX 2060 Max-Q
OpenGL Driver : Geforce Driver 457.63

g. The number of hours you spent on the assignment, on a weekly basis
week 8 ~ 4 hours
week 9 ~ 6 hours

h. Any other useful information pertaining to the application 
none.
