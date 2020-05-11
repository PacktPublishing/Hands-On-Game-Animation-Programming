#ifndef _H_INTERSECTIONS_
#define _H_INTERSECTIONS_

#include "vec3.h"
#include "Mesh.h"
#include <cmath>
#include <vector>

struct Ray {
	vec3 origin;
	vec3 direction;

	inline Ray() : direction(vec3(0, -1, 0)) { }
	inline Ray(const vec3& o) : origin(o), direction(vec3(0, -1, 0)) { }
	inline Ray(const vec3& o, const vec3& d) : origin(o), direction(d) { }
};

struct Triangle {
	vec3 v0;
	vec3 v1;
	vec3 v2;
	vec3 normal;

	inline Triangle() { }
	inline Triangle(const vec3& _v0, const vec3& _v1, const vec3& _v2) : v0(_v0), v1(_v1), v2(_v2) {
		normal = normalized(cross(v1 - v0, v2 - v0));
	}
};

bool RaycastTriangle(const Ray& ray, const Triangle& triangle, vec3& hitPoint);
std::vector<Triangle> MeshToTriangles(Mesh& mesh);
std::vector<Triangle> MeshesToTriangles(std::vector<Mesh>& mesh);

#endif