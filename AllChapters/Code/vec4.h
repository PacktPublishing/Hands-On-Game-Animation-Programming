#ifndef _H_VEC4_
#define _H_VEC4_

template<typename T>
struct TVec4 {
	union {
		struct {
			T x;
			T y;
			T z;
			T w;
		};
		T v[4];
	};
	inline TVec4() : x((T)0), y((T)0), z((T)0), w((T)0) { }
	inline TVec4(T _x, T _y, T _z, T _w) :
		x(_x), y(_y), z(_z), w(_w) { }
	inline TVec4(T* fv) :
		x(fv[0]), y(fv[1]), z(fv[2]), w(fv[3]) { }
};

typedef TVec4<float> vec4;
typedef TVec4<int> ivec4;
typedef TVec4<unsigned int> uivec4;

#endif