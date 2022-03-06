/* Start Header -------------------------------------------------------
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Intersections.h
Purpose: Intersect check functions
Language: C++17, Visual Studio 2019
Platform: Visual Studio 2019 v16.11.2, Windows 10
Project: haewon.shon_CS350_2
Author: Haewon Shon (haewon.shon@digipen.edu, 180002920)
Creation date: Feb 25, 2022
End Header --------------------------------------------------------*/
#pragma once

#include "Sphere.h"
#include "AABB.h"
#include "Triangle.h"
#include "Ray.h"
#include "Plane.h"

// basic intersections
bool SphereSphere(const Sphere& s1, const Sphere& s2);
bool SphereAABB(const AABB& aabb, const Sphere& s);
bool SphereAABB(const Sphere& s, const AABB& aabb);
bool AABBAABB(const AABB& aabb1, const AABB& aabb2);

// point-base intersections
bool PointSphere(const Point3D& p, const Sphere& s);
bool PointAABB(const Point3D& p, const AABB& aabb);
bool PointTriangle(const Point3D& p, const Triangle& t);
bool PointPlane(const Point3D& p, const Plane& plane);

// ray-base intersections
bool RaySphere(const Ray& r, const Sphere& s, float& t1, float& t2);
bool RayAABB(const Ray& r, const AABB& aabb, float& t);
bool RayTriangle(const Ray& r, const Triangle& triangle, float& t);
bool RayPlane(const Ray& r, const Plane& p, float& t);

// plane-base intersections
bool PlaneSphere(const Plane& p, const Sphere& s);
bool PlaneAABB(const Plane& p, const AABB& aabb);