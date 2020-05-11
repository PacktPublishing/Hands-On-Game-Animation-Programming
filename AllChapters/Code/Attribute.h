#ifndef _H_ATTRIBUTE_
#define _H_ATTRIBUTE_

#include <vector>

template<typename T>
class Attribute {
protected:
	unsigned int mHandle;
	unsigned int mCount;
private:
	Attribute(const Attribute& other);
	Attribute& operator=(const Attribute& other);
	void SetAttribPointer(unsigned int slot);
public:
	Attribute();
	~Attribute();

	void Set(T* inputArray, unsigned int arrayLength);
	void Set(std::vector<T>& input);

	void BindTo(unsigned int slot);
	void UnBindFrom(unsigned int slot);

	unsigned int Count();
	unsigned int GetHandle();
};


#endif