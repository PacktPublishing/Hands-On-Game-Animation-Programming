#ifndef _H_MAT2_
#define _H_MAT2_

#define MAT2_EPSILON 0.000001f

struct mat2 {
	union {
		float v[4];
		struct {
			float right[2];
			float up[2];
		};
		struct {
			//            row 1     row 2    
			/* column 1 */float xx; float xy;
			/* column 2 */float yx; float yy;
		};
		struct {
			float c0r0; float c0r1;
			float c1r0; float c1r1;
		};
		struct {
			float r0c0; float r1c0;
			float r0c1; float r1c1;
		};
	};
	inline mat2() :
		xx(1), xy(0),
		yx(0), yy(1) {}
	inline mat2(float *fv) :
		xx(fv[0]), xy(fv[1]),
		yx(fv[3]), yy(fv[4]) { }
	inline mat2(
		float _00, float _01,
		float _10, float _11) :
		xx(_00), xy(_01),
		yx(_10), yy(_11) { }
};

mat2 operator*(const mat2 &a, const mat2 &b);
mat2 operator*(const mat2& m, float f);

bool operator==(const mat2& a, const mat2& b);
bool operator!=(const mat2& a, const mat2& b);

mat2 transposed(const mat2 &m);
float cut(const mat2& m, int r, int c);

mat2 minor(const mat2& m);
mat2 cofactor(const mat2& m);

float determinant(const mat2& m);
mat2 adjugate(const mat2& m);

mat2 inverse(const mat2& m);

#endif // !_H_MAT2_