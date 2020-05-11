#ifndef _H_VEC3_
#define _H_VEC3_

#define VEC3_EPSILON 0.000001f

struct vec3 {
	union {
		struct {
			float x;
			float y;
			float z;
		};
		float v[3];
	};
	inline vec3() : x(0.0f), y(0.0f), z(0.0f) { }
	inline vec3(float _x, float _y, float _z) :
		x(_x), y(_y), z(_z) { }
	inline vec3(float* fv) :
		x(fv[0]), y(fv[1]), z(fv[2]) { }
};

vec3 operator+(const vec3& l, const vec3& r);
vec3 operator-(const vec3& l, const vec3& r);
vec3 operator*(const vec3& v, float f);
vec3 operator*(const vec3& l, const vec3& r);
float dot(const vec3& l, const vec3& r);
float lenSq(const vec3& v);
float len(const vec3& v);
void normalize(vec3& v);
vec3 normalized(const vec3& v);
float angle(const vec3& l, const vec3& r);
vec3 project(const vec3& a, const vec3& b);
vec3 reject(const vec3& a, const vec3& b);
vec3 reflect(const vec3& a, const vec3& b);
vec3 cross(const vec3& l, const vec3& r);
vec3 lerp(const vec3& s, const vec3& e, float t);
vec3 slerp(const vec3& s, const vec3& e, float t);
vec3 nlerp(const vec3& s, const vec3& e, float t);
bool operator==(const vec3& l, const vec3& r);
bool operator!=(const vec3& l, const vec3& r);

#endif
