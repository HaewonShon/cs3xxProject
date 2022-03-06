/* Start Header -------------------------------------------------------
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: BasicIntersection.cpp
Purpose: Intersect check functions
Language: C++17, Visual Studio 2019
Platform: Visual Studio 2019 v16.11.2, Windows 10
Project: haewon.shon_CS350_2
Author: Haewon Shon (haewon.shon@digipen.edu, 180002920)
Creation date: Feb 25, 2022
End Header --------------------------------------------------------*/
#pragma once

#include "Intersections.h"
#include <limits> // epsilon
#include <utility> // std::swap

// basic intersections
bool SphereSphere(const Sphere& s1, const Sphere& s2)
{
	return PointSphere(s2.position, Sphere(s1.position, s1.radius + s2.radius));
}

bool SphereAABB(const AABB& aabb, const Sphere& s)
{
	glm::vec3 nearestPoint{s.position.coordinate};

	if (nearestPoint.x > aabb.max.x) nearestPoint.x = aabb.max.x;
	else if (nearestPoint.x < aabb.min.x) nearestPoint.x = aabb.min.x;
	if (nearestPoint.y > aabb.max.y) nearestPoint.y = aabb.max.y;
	else if (nearestPoint.y < aabb.min.y) nearestPoint.y = aabb.min.y;
	if (nearestPoint.z > aabb.max.z) nearestPoint.z = aabb.max.z;
	else if (nearestPoint.z < aabb.min.z) nearestPoint.z = aabb.min.z;

	glm::vec3 difference = nearestPoint - s.position.coordinate;
	float distance_squared = difference.x * difference.x + difference.y * difference.y + difference.z * difference.z;
	return distance_squared <= s.radius * s.radius;
}

bool SphereAABB(const Sphere& s, const AABB& aabb)
{
	return SphereAABB(aabb, s);
}

bool AABBAABB(const AABB& aabb1, const AABB& aabb2)
{
	if (aabb1.max.x < aabb2.min.x || aabb2.max.x < aabb1.min.x)
		return false;
	if (aabb1.max.y < aabb2.min.y || aabb2.max.y < aabb1.min.y)
		return false;
	if (aabb1.max.z < aabb2.min.z || aabb2.max.z < aabb1.min.z)
		return false;
	return true;
}

// point-base intersections
bool PointSphere(const Point3D& p, const Sphere& s)
{
	return glm::distance(p.coordinate, s.position.coordinate) < s.radius;
}

bool PointAABB(const Point3D& p, const AABB& aabb)
{
	if (aabb.min.x > p.coordinate.x || aabb.max.x < p.coordinate.x) return false;
	if (aabb.min.y > p.coordinate.y || aabb.max.y < p.coordinate.y) return false;
	if (aabb.min.z > p.coordinate.z || aabb.max.z < p.coordinate.z) return false;
	return true;
}

bool PointTriangle(const Point3D& p, const Triangle& t)
{
	glm::vec3 v0 = p.coordinate - t.v3.coordinate;
	glm::vec3 v1 = t.v1.coordinate - t.v3.coordinate;
	glm::vec3 v2 = t.v2.coordinate - t.v3.coordinate;

	float a = glm::dot(v1, v1);
	float b = glm::dot(v2, v1);
	float c = glm::dot(v1, v2);
	float d = glm::dot(v2, v2);
	float e = glm::dot(v0, v1);
	float f = glm::dot(v0, v2);

	float det_abcd = a * d - b * c;
	float u = (e * d - b * f) / det_abcd;
	float v = (a * f - e * c) / det_abcd;
	float w = 1.f - u - v;

	if (u < 0.f || u > 1.f) return false;
	if (v < 0.f || v> 1.f) return false;
	if (w < 0.f || w> 1.f) return false;
	return true;
}

bool PointPlane(const Point3D& p, const Plane& plane)
{
	return (plane.normal.x * p.coordinate.x + plane.normal.y * p.coordinate.y + plane.normal.z * p.coordinate.z) == plane.normal.w;
}

// ray-base intersections
bool RaySphere(const Ray& r, const Sphere& s, float& t1, float& t2)
{
	glm::vec3 Cs_Ps = s.position.coordinate - r.start;

	//for quadratic equation at^2 + bt + c = 0
	float a = glm::dot(r.direction, r.direction);
	float b = -2 * glm::dot(Cs_Ps, r.direction);
	float c = glm::dot(Cs_Ps, Cs_Ps);

	float discriminant = b * b - 4 * a * c;
	if (std::abs(discriminant) < std::numeric_limits<float>().epsilon())
	{
		t1 = -b / (2 * a);
		t2 = -1.f; // not valid
		return true;
	}
	else if (discriminant < 0)
	{
		return false;
	}
	else
	{
		t1 = (-b + std::sqrt(discriminant)) / (2 * a);
		t2 = (-b - std::sqrt(discriminant)) / (2 * a);
		return true;
	}
}

bool RayAABB(const Ray& r, const AABB& aabb, float& t)
{
	glm::vec3 t_with_min = (aabb.min - r.start) / r.direction;
	glm::vec3 t_with_max = (aabb.max - r.start) / r.direction;

	if (r.direction.x < 0)
	{
		std::swap(t_with_min.x, t_with_max.x);
	}
	if (r.direction.y < 0)
	{
		std::swap(t_with_min.y, t_with_max.y);
	}
	if (r.direction.z < 0)
	{
		std::swap(t_with_min.z, t_with_max.z);
	}

	float t_min = std::fmax(std::fmax(t_with_min.x, t_with_min.y), t_with_min.z);
	float t_max = std::fmin(std::fmin(t_with_max.x, t_with_max.y), t_with_max.z);

	if (t_min > t_max) return false;

	t = t_min;
	return true;
}

bool RayTriangle(const Ray& r, const Triangle& triangle, float& t)
{
	// if does not hit plane - no intersection
	if (RayPlane(r, Plane(glm::cross(triangle.v2.coordinate - triangle.v1.coordinate, triangle.v3.coordinate - triangle.v1.coordinate), triangle.v2.coordinate), t) == false)
	{
		return false;
	}

	glm::vec3 P = r.start + r.direction * t;

	glm::vec3 v0 = P - triangle.v3.coordinate;
	glm::vec3 v1 = triangle.v1.coordinate - triangle.v3.coordinate;
	glm::vec3 v2 = triangle.v2.coordinate - triangle.v3.coordinate;

	float a = glm::dot(v1, v1);
	float b = glm::dot(v2, v1);
	float c = glm::dot(v1, v2);
	float d = glm::dot(v2, v2);
	float e = glm::dot(v0, v1);
	float f = glm::dot(v0, v2);

	float det_abcd = a * d - b * c;
	float u = (e * d - b * f) / det_abcd;
	float v = (a * f - e * c) / det_abcd;
	float w = 1.f - u - v;

	if (u < 0.f || u > 1.f) return false;
	if (v < 0.f || v>1.f) return false;
	if (w < 0.f || w>1.f) return false;
	return true;
}

bool RayPlane(const Ray& r, const Plane& plane, float& t)
{
	glm::vec3 n{ plane.normal };
	t = -glm::dot(n, r.start - glm::vec3(0.f, 0.f, plane.normal.w / plane.normal.z)) / glm::dot(n, r.direction);
	return t >= 0;
}

// plane-base intersections
bool PlaneSphere(const Plane& p, const Sphere& s)
{
	glm::vec3 normal{p.normal.x, p.normal.y, p.normal.z};
	// find projected point on plane
	glm::vec3 v = s.position.coordinate - glm::vec3(0.f, 0.f, p.normal.w / p.normal.z);
	float cos = glm::dot(v, normal) / (glm::length(v) * glm::length(normal));
	float distanceToPlane = glm::length(v) * cos;
	return distanceToPlane <= s.radius;
}

bool PlaneAABB(const Plane& p, const AABB& aabb)
{
	glm::vec3 AABBpoints[8] = {
		glm::vec3{aabb.min.x, aabb.min.y, aabb.min.z},
		glm::vec3{aabb.min.x, aabb.min.y, aabb.max.z},
		glm::vec3{aabb.min.x, aabb.max.y, aabb.min.z},
		glm::vec3{aabb.min.x, aabb.max.y, aabb.max.z},
		glm::vec3{aabb.max.x, aabb.min.y, aabb.min.z},
		glm::vec3{aabb.max.x, aabb.min.y, aabb.max.z},
		glm::vec3{aabb.max.x, aabb.max.y, aabb.min.z},
		glm::vec3{aabb.max.x, aabb.max.y, aabb.max.z},
	};

	bool isFront = (p.normal.x * AABBpoints[0].x + p.normal.y * AABBpoints[0].y + p.normal.z * AABBpoints[0].z - p.normal.w) > 0;
	if (isFront == true)
	{
		for (int i = 1; i < 8; ++i)
		{
			if ((p.normal.x * AABBpoints[i].x + p.normal.y * AABBpoints[i].y + p.normal.z * AABBpoints[i].z - p.normal.w) <= 0)
			{
				return false;
			}
		}
	}
	else
	{
		for (int i = 1; i < 8; ++i)
		{
			if ((p.normal.x * AABBpoints[i].x + p.normal.y * AABBpoints[i].y + p.normal.z * AABBpoints[i].z - p.normal.w) >= 0)
			{
				return false;
			}
		}
	}
	return true;
}