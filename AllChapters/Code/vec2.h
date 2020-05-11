#ifndef _H_VEC2_
#define _H_VEC2_

template<typename T>
struct TVec2 {
	union {
		struct {
			T x;
			T y;
		};
		T v[2];
	};
	inline TVec2() : x(T(0)), y(T(0)) { }
	inline TVec2(T _x, T _y) :
		x(_x), y(_y) { }
	inline TVec2(T* fv) :
		x(fv[0]), y(fv[1]) { }
};

typedef TVec2<float> vec2;
typedef TVec2<int> ivec2;
typedef TVec2<unsigned int> uivec2;

#endif