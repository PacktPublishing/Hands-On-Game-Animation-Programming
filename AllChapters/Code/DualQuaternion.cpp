#include "DualQuaternion.h"
#include <cmath>

DualQuaternion operator+(const DualQuaternion& l, const DualQuaternion& r) {
	return DualQuaternion(l.real + r.real, l.dual + r.dual);
}

DualQuaternion operator*(const DualQuaternion& dq, float f) {
	return DualQuaternion(dq.real * f, dq.dual * f);
}

bool operator==(const DualQuaternion& l, const DualQuaternion& r) {
	return l.real == r.real && l.dual == r.dual;
}

bool operator!=(const DualQuaternion& l, const DualQuaternion& r) {
	return l.real != r.real || l.dual != r.dual;
}

// Remember, multiplication order is left to right. 
// This is the opposite of matrix and quaternion multiplication order
DualQuaternion operator*(const DualQuaternion& l, const DualQuaternion& r) {
	DualQuaternion lhs = normalized(l);
	DualQuaternion rhs = normalized(r);

	return DualQuaternion(lhs.real * rhs.real, lhs.real * rhs.dual + lhs.dual * rhs.real);
}

float dot(const DualQuaternion& l, const DualQuaternion& r) {
	return dot(l.real, r.real);
}

DualQuaternion conjugate(const DualQuaternion& dq) {
	return DualQuaternion(conjugate(dq.real), conjugate(dq.dual));
}

DualQuaternion normalized(const DualQuaternion& dq) {
	float magSq = dot(dq.real, dq.real);
	if (magSq < 0.000001f) {
		return DualQuaternion();
	}
	float invMag = 1.0f / sqrtf(magSq);

	return DualQuaternion(dq.real * invMag, dq.dual * invMag);
}

void normalize(DualQuaternion& dq) {
	float magSq = dot(dq.real, dq.real);
	if (magSq < 0.000001f) {
		return;
	}
	float invMag = 1.0f / sqrtf(magSq);

	dq.real = dq.real * invMag;
	dq.dual = dq.dual * invMag;
}

DualQuaternion transformToDualQuat(const Transform& t) {
	quat d(t.position.x, t.position.y, t.position.z, 0);

	quat qr = t.rotation;
	quat qd = qr * d * 0.5f;

	return DualQuaternion(qr, qd);
}

Transform dualQuatToTransform(const DualQuaternion& dq) {
	Transform result;

	result.rotation = dq.real;

	quat d = conjugate(dq.real) * (dq.dual * 2.0f);
	result.position = vec3(d.x, d.y, d.z);

	return result;
}

vec3 transformVector(const DualQuaternion& dq, const vec3& v) {
	return dq.real * v;
}

vec3 transformPoint(const DualQuaternion& dq, const vec3& v) {
	quat d = conjugate(dq.real) * (dq.dual * 2.0f);
	vec3 t = vec3(d.x, d.y, d.z);

	return dq.real * v + t;
}