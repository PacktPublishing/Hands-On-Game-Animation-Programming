

#include "mat4.h"
#include <iostream>
#include <cmath>

#define M4D(aRow, bCol) \
	a.v[0 * 4 + (aRow)] * b.v[(bCol) * 4 + 0] + \
	a.v[1 * 4 + (aRow)] * b.v[(bCol) * 4 + 1] + \
	a.v[2 * 4 + (aRow)] * b.v[(bCol) * 4 + 2] + \
	a.v[3 * 4 + (aRow)] * b.v[(bCol) * 4 + 3]

mat4 operator*(const mat4 &a, const mat4 &b) {
	return mat4(
		M4D(0, 0), M4D(1, 0), M4D(2, 0), M4D(3, 0),
		M4D(0, 1), M4D(1, 1), M4D(2, 1), M4D(3, 1),
		M4D(0, 2), M4D(1, 2), M4D(2, 2), M4D(3, 2),
		M4D(0, 3), M4D(1, 3), M4D(2, 3), M4D(3, 3)
	);
}

mat4 operator*(const mat4& m, float f) {
	return mat4(
		m.xx * f, m.xy * f, m.xz * f, m.xw * f,
		m.yx * f, m.yy * f, m.yz * f, m.yw * f,
		m.zx * f, m.zy * f, m.zz * f, m.zw * f,
		m.tx * f, m.ty * f, m.tz * f, m.tw * f
	);
}

bool operator==(const mat4& a, const mat4& b) {
	for (int i = 0; i < 16; ++i) {
		if (fabsf(a.v[i] - b.v[i]) > MAT4_EPSILON) {
			return false;
		}
	}
	return true;
}

bool operator!=(const mat4& a, const mat4& b) {
	return !(a == b);
}

mat4 transposed(const mat4 &m) {
	return mat4(
		m.xx, m.yx, m.zx, m.tx,
		m.xy, m.yy, m.zy, m.ty,
		m.xz, m.yz, m.zz, m.tz,
		m.xw, m.yw, m.zw, m.tw
	);
}

mat3 cut(const mat4& m, int r, int c) {
	mat3 result;
	int target = 0;

	for (int row = 0; row < 4; ++row) {
		for (int col = 0; col < 4; ++col) {
			if (row == r || col == c) {
				continue;
			}
			int source = col * 4 + row; // remember, clumn major

			result.v[target++] = m.v[source];
		}
	}

	return result;
}

mat4 minor(const mat4& m) {
	mat4 result;

	for (int row = 0; row < 4; ++row) {
		for (int col = 0; col < 4; ++col) {
			int index = col * 4 + row; // remember, clumn major
			mat3 sub_matrix = cut(m, row, col);
			
			result.v[index] = determinant(sub_matrix);
		}
	}

	return result;
}

mat4 cofactor(const mat4& m) {
	mat4 matrix_of_minors = minor(m);
	mat4 result;
	
	for (int row = 0; row < 4; ++row) {
		for (int col = 0; col < 4; ++col) {
			int index = col * 4 + row; // remember, clumn major
			float power = (float)(row + col);

			result.v[index] = matrix_of_minors.v[index] * (float)pow(-1, power);
		}
	}

	return result;
}


float determinant(const mat4& m) {
	mat4 cofactor_matrix = cofactor(m);
	float det = // First row, matrix is column major
		m.v[0] * cofactor_matrix.v[0] +
		m.v[4] * cofactor_matrix.v[4] +
		m.v[8] * cofactor_matrix.v[8] +
		m.v[12] * cofactor_matrix.v[12];
	return det;
}

mat4 adjugate(const mat4& m) {
	mat4 cofactor_matrix = cofactor(m);
	mat4 result = transposed(cofactor_matrix);
	return result;
}


mat4 inverse(const mat4& m) {
	float det = determinant(m);

	if (det < 0.0000001f && det > -0.0000001f) {
		std::cout << "ERROR: Can't invert 4x4 matrix with no determinant\n";
		return mat4();
	}

	mat4 adjugate_matrix = adjugate(m);
	float inv_det = 1.0f / det;

	mat4 result = adjugate_matrix * inv_det;
	return result;
}