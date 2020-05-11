

#pragma warning(disable : 28251)
#pragma warning(disable : 28159)
#include <iostream>
#if _DEBUG
#include <crtdbg.h>
#endif

#include "mat2.h"
#include "mat3.h"
#include "mat4.h"

using std::cout;

#pragma comment( linker, "/subsystem:console" )

int main(int argc, char** argv) {
#if _DEBUG
	_CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_DEBUG);
#endif

	mat4 test = mat4(
		0.928477f, 0, -0.371391f, 0,
		-0.0996546f, 0.963328f, -0.249136f, 0,
		0.357771f, 0.268328f, 0.894427f, 0,
		4, 3, 10, 1);
	mat4 inv = inverse(test);

	mat4 identity = test * inv;

	if (identity != mat4()) {
		std::cout << "ERROR, expected inverse * matrix to be identity\n";
	}
	else {
		std::cout << "4x4 inverse found\n";
	}

	return 0;
}