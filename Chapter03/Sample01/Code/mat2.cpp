

#include "mat2.h"
#include <cmath>
#include <iostream>

#define M2D(aRow, bCol) \
	a.v[0 * 2 + (aRow)] * b.v[(bCol) * 2 + 0] + \
	a.v[1 * 2 + (aRow)] * b.v[(bCol) * 2 + 1] 

mat2 operator*(const mat2 &a, const mat2 &b) {
	return mat2(
		M2D(0, 0), M2D(1, 0),
		M2D(0, 1), M2D(1, 1)
	);
}

mat2 operator*(const mat2& m, float f) {
	return mat2(
		m.xx * f, m.xy * f,
		m.yx * f, m.yy * f
	);
}

bool operator==(const mat2& a, const mat2& b) {
	for (int i = 0; i < 4; ++i) {
		if (fabsf(a.v[i] - b.v[i]) > MAT2_EPSILON) {
			return false;
		}
	}
	return true;
}

bool operator!=(const mat2& a, const mat2& b) {
	return !(a == b);
}

mat2 transposed(const mat2 &m) {
	return mat2(
		m.xx, m.yx,
		m.xy, m.yy
	);
}

float cut(const mat2& m, int r, int c) {
	for (int row = 0; row < 2; ++row) {
		for (int col = 0; col < 2; ++col) {
			if (row == r || col == c) {
				continue;
			}
			int source = col * 2 + row; // remember, clumn major
			return m.v[source];
		}
	}

	std::cout << "ERROR: Can't cut scalar from 2x2 matrix\n";
	return 0.0f;
}

mat2 minor(const mat2& m) {
	mat2 result;

	for (int row = 0; row < 2; ++row) {
		for (int col = 0; col < 2; ++col) {
			int index = col * 2 + row; // remember, clumn major
			result.v[index] = cut(m, row, col);
		}
	}

	return result;
}

mat2 cofactor(const mat2& m) {
	mat2 matrix_of_minors = minor(m);
	mat2 result;

	for (int row = 0; row < 2; ++row) {
		for (int col = 0; col < 2; ++col) {
			int index = col * 2 + row; // remember, clumn major
			float power = (float)(row + col);

			result.v[index] = matrix_of_minors.v[index] * (float)pow(-1, power);
		}
	}

	return result;
}

float determinant(const mat2& m) {
	// This could simplify to:
	// m.xx * m.yy - m.xy * m.yx

	mat2 cofactor_matrix = cofactor(m);
	float det = // First row, matrix is column major
		m.v[0] * cofactor_matrix.v[0] +
		m.v[2] * cofactor_matrix.v[2];
	return det;
}

mat2 adjugate(const mat2& m) {
	mat2 cofactor_matrix = cofactor(m);
	mat2 result = transposed(cofactor_matrix);
	return result;
}

mat2 inverse(const mat2& m) {
	float det = determinant(m);

	if (det < 0.0000001f && det > -0.0000001f) {
		std::cout << "ERROR: Can't invert 2x2 matrix with no determinant\n";
		return mat2();
	}

	mat2 adjugate_matrix = adjugate(m);
	float inv_det = 1.0f / det;

	mat2 result = adjugate_matrix * inv_det;
	return result;
}