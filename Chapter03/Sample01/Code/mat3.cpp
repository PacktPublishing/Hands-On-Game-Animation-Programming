

#include "mat3.h"
#include <cmath>
#include <iostream>

#define M3D(aRow, bCol) \
	a.v[0 * 3 + (aRow)] * b.v[(bCol) * 3 + 0] + \
	a.v[1 * 3 + (aRow)] * b.v[(bCol) * 3 + 1] + \
	a.v[2 * 3 + (aRow)] * b.v[(bCol) * 3 + 2]

mat3 operator*(const mat3 &a, const mat3 &b) {
	return mat3(
		M3D(0, 0), M3D(1, 0), M3D(2, 0),
		M3D(0, 1), M3D(1, 1), M3D(2, 1),
		M3D(0, 2), M3D(1, 2), M3D(2, 2)
	);
}

mat3 operator*(const mat3& m, float f) {
	return mat3(
		m.xx * f, m.xy * f, m.xz * f,
		m.yx * f, m.yy * f, m.yz * f,
		m.zx * f, m.zy * f, m.zz * f
	);
}

bool operator==(const mat3& a, const mat3& b) {
	for (int i = 0; i < 9; ++i) {
		if (fabsf(a.v[i] - b.v[i]) > MAT3_EPSILON) {
			return false;
		}
	}
	return true;
}

bool operator!=(const mat3& a, const mat3& b) {
	return !(a == b);
}

mat3 transposed(const mat3 &m) {
	return mat3(
		m.xx, m.yx, m.zx,
		m.xy, m.yy, m.zy,
		m.xz, m.yz, m.zz
	);
}

mat2 cut(const mat3& m, int r, int c) {
	mat2 result;
	int target = 0;

	for (int row = 0; row < 3; ++row) {
		for (int col = 0; col < 3; ++col) {
			if (row == r || col == c) {
				continue;
			}
			int source = col * 3 + row; // remember, clumn major

			result.v[target++] = m.v[source];
		}
	}

	return result;
}

mat3 minor(const mat3& m) {
	mat3 result;

	for (int row = 0; row < 3; ++row) {
		for (int col = 0; col < 3; ++col) {
			int index = col * 3 + row; // remember, clumn major
			mat2 sub_matrix = cut(m, row, col);

			result.v[index] = determinant(sub_matrix);
		}
	}

	return result;
}

mat3 cofactor(const mat3& m) {
	mat3 matrix_of_minors = minor(m);
	mat3 result;

	for (int row = 0; row < 3; ++row) {
		for (int col = 0; col < 3; ++col) {
			int index = col * 3 + row; // remember, clumn major
			float power = (float)(row + col);

			result.v[index] = matrix_of_minors.v[index] * (float)pow(-1, power);
		}
	}

	return result;
}

float determinant(const mat3& m) {
	mat3 cofactor_matrix = cofactor(m);
	float det = // First row, matrix is column major
		m.v[0] * cofactor_matrix.v[0] +
		m.v[3] * cofactor_matrix.v[3] +
		m.v[6] * cofactor_matrix.v[6];
	return det;
}

mat3 adjugate(const mat3& m) {
	mat3 cofactor_matrix = cofactor(m);
	mat3 result = transposed(cofactor_matrix);
	return result;
}

mat3 inverse(const mat3& m) {
	float det = determinant(m);

	if (det < 0.0000001f && det > -0.0000001f) {
		std::cout << "ERROR: Can't invert 3x3 matrix with no determinant\n";
		return mat3();
	}

	mat3 adjugate_matrix = adjugate(m);
	float inv_det = 1.0f / det;

	mat3 result = adjugate_matrix * inv_det;
	return result;
}