#ifndef _H_UNIFORM_
#define _H_UNIFORM_

#include <vector>

template <typename T>
class Uniform {
private:
	Uniform();
	Uniform(const Uniform&);
	Uniform& operator=(const Uniform&);
	~Uniform();
public:
	static void Set(unsigned int slot, const T& value);
	static void Set(unsigned int slot, T* inputArray, unsigned int arrayLength);
	static void Set(unsigned int slot, std::vector<T>& inputArray);
};

#endif