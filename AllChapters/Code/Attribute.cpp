#pragma warning(disable : 26451)
#include "Attribute.h"
#undef APIENTRY
#include "glad.h"

#include "vec2.h"
#include "vec3.h"
#include "vec4.h"
#include "quat.h"

template Attribute<int>;
template Attribute<float>;
template Attribute<vec2>;
template Attribute<vec3>;
template Attribute<vec4>;
template Attribute<ivec4>;
template Attribute<quat>;

template<typename T>
Attribute<T>::Attribute() {
	glGenBuffers(1, &mHandle);
	mCount = 0;
}

template<typename T>
Attribute<T>::~Attribute() {
	glDeleteBuffers(1, &mHandle);
}

template<typename T>
void Attribute<T>::Set(T* inputArray, unsigned int arrayLength) {
	mCount = arrayLength;
	unsigned int size = sizeof(T);

	glBindBuffer(GL_ARRAY_BUFFER, mHandle);
	glBufferData(GL_ARRAY_BUFFER, size * mCount, inputArray, GL_STREAM_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

template<typename T>
void Attribute<T>::Set(std::vector<T>& input) {
	Set(&input[0], (unsigned int)input.size());
}

template<typename T>
void Attribute<T>::BindTo(unsigned int slot) {
	glBindBuffer(GL_ARRAY_BUFFER, mHandle);
	glEnableVertexAttribArray(slot);
	SetAttribPointer(slot);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

template<typename T>
void Attribute<T>::UnBindFrom(unsigned int slot) {
	glBindBuffer(GL_ARRAY_BUFFER, mHandle);
	glDisableVertexAttribArray(slot);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

template<typename T>
unsigned int Attribute<T>::Count() {
	return mCount;
}

template<typename T>
unsigned int Attribute<T>::GetHandle() {
	return mHandle;
}

template<>
void Attribute<int>::SetAttribPointer(unsigned int slot) {
	glVertexAttribIPointer(slot, 1, GL_INT, 0, (void*)0);
}

template<>
void Attribute<ivec4>::SetAttribPointer(unsigned int slot) {
	glVertexAttribIPointer(slot, 4, GL_INT, 0, (void*)0);
}

template<>
void Attribute<float>::SetAttribPointer(unsigned int slot) {
	glVertexAttribPointer(slot, 1, GL_FLOAT, GL_FALSE, 0, (void*)0);
}

template<>
void Attribute<vec2>::SetAttribPointer(unsigned int slot) {
	glVertexAttribPointer(slot, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
}

template<>
void Attribute<vec3>::SetAttribPointer(unsigned int slot) {
	glVertexAttribPointer(slot, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
}

template<>
void Attribute<vec4>::SetAttribPointer(unsigned int slot) {
	glVertexAttribPointer(slot, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
}

template<>
void Attribute<quat>::SetAttribPointer(unsigned int slot) {
	glVertexAttribPointer(slot, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
}