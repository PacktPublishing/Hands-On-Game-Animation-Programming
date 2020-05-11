#ifndef _H_MAT3_
#define _H_MAT3_

#include "mat2.h"

#define MAT3_EPSILON 0.000001f

struct mat3 {
	union {
		float v[9];
		struct {
			float right[3];
			float up[3];
			float forward[3];
		};
		struct {
			//            row 1     row 2     row 3    
			/* column 1 */float xx; float xy; float xz;
			/* column 2 */float yx; float yy; float yz;
			/* column 3 */float zx; float zy; float zz;
		};
		struct {
			float c0r0; float c0r1; float c0r2;
			float c1r0; float c1r1; float c1r2;
			float c2r0; float c2r1; float c2r2;
		};
		struct {
			float r0c0; float r1c0; float r2c0;
			float r0c1; float r1c1; float r2c1;
			float r0c2; float r1c2; float r2c2;
		};
	};
	inline mat3() :
		xx(1), xy(0), xz(0),
		yx(0), yy(1), yz(0),
		zx(0), zy(0), zz(1) {}
	inline mat3(float *fv) :
		xx(fv[0]), xy(fv[1]), xz(fv[2]),
		yx(fv[3]), yy(fv[4]), yz(fv[5]),
		zx(fv[6]), zy(fv[7]), zz(fv[8]) { }
	inline mat3(
		float _00, float _01, float _02,
		float _10, float _11, float _12,
		float _20, float _21, float _22) :
		xx(_00), xy(_01), xz(_02),
		yx(_10), yy(_11), yz(_12),
		zx(_20), zy(_21), zz(_22) { }
};

mat3 operator*(const mat3 &a, const mat3 &b);
mat3 operator*(const mat3& m, float f);

bool operator==(const mat3& a, const mat3& b);
bool operator!=(const mat3& a, const mat3& b);

mat3 transposed(const mat3 &m);
mat2 cut(const mat3& m, int r, int c);

mat3 minor(const mat3& m);
mat3 cofactor(const mat3& m);

float determinant(const mat3& m);
mat3 adjugate(const mat3& m);

mat3 inverse(const mat3& m);

#endif