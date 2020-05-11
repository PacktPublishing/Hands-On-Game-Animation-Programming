#include "AnimTexture.h"
#include "glad.h"
#include <cstring>
#include <fstream>
#include <istream>
#include <ostream>
#include <iostream>

AnimTexture::AnimTexture() {
	mData = 0;
	mSize = 0;
	glGenTextures(1, &mHandle);
}

AnimTexture::AnimTexture(const AnimTexture& other) {
	mData = 0;
	mSize = 0;
	glGenTextures(1, &mHandle);
	*this = other;
}

AnimTexture& AnimTexture::operator=(const AnimTexture& other) {
	if (this == &other) {
		return *this;
	}

	mSize = other.mSize;
	if (mData != 0) {
		delete[] mData;
	}
	mData = 0;
	if (mSize != 0) {
		mData = new float[mSize * mSize * 4];
		memcpy(mData, other.mData, sizeof(float) * (mSize * mSize * 4));
	}
	return *this;
}

AnimTexture::~AnimTexture() {
	if (mData != 0) {
		delete[] mData;
	}
	glDeleteTextures(1, &mHandle);
}

void AnimTexture::Save(const char* path) {
	std::ofstream file;
	file.open(path, std::ios::out | std::ios::binary);
	if (!file.is_open()) {
		std::cout << "Couldn't open " << path << " to write to\n";
	}

	file << mSize;
	if (mSize != 0) {
		file.write((char*)mData, sizeof(float) * (mSize * mSize * 4));
	}
	file.close();
}

void AnimTexture::Load(const char* path) {
	std::ifstream file;
	file.open(path, std::ios::in | std::ios::binary);
	if (!file.is_open()) {
		std::cout << "Couldn't open " << path << " to read from\n";
	}

	file >> mSize;
	mData = new float[mSize * mSize * 4];
	file.read((char*)mData, sizeof(float) * (mSize * mSize * 4));
	file.close();
	UploadTextureDataToGPU();
}

void AnimTexture::UploadTextureDataToGPU() {
	glBindTexture(GL_TEXTURE_2D, mHandle);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, mSize, mSize, 0, GL_RGBA, GL_FLOAT, mData);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);
}

unsigned int AnimTexture::Size() {
	return mSize;
}

unsigned int AnimTexture::GetHandle() {
	return mHandle;
}

float* AnimTexture::GetData() {
	return mData;
}

void AnimTexture::Resize(unsigned int newSize) {
	if (mData != 0) {
		delete[] mData;
	}
	mSize = newSize;
	mData = new float[mSize * mSize * 4];
}

void AnimTexture::Set(unsigned int uniformIndex, unsigned int textureIndex) {
	glActiveTexture(GL_TEXTURE0 + textureIndex);
	glBindTexture(GL_TEXTURE_2D, mHandle);
	glUniform1i(uniformIndex, textureIndex);
}

void AnimTexture::UnSet(unsigned int textureIndex) {
	glActiveTexture(GL_TEXTURE0 + textureIndex);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE0);
}

void AnimTexture::SetTexel(unsigned int x, unsigned int y, const vec3& v) {
	unsigned int index = (y * mSize * 4) + (x * 4);

	mData[index + 0] = v.x;
	mData[index + 1] = v.y;
	mData[index + 2] = v.z;
	mData[index + 3] = 0.0f;
}

void AnimTexture::SetTexel(unsigned int x, unsigned int y, const quat& q) {
	unsigned int index = (y * mSize * 4) + (x * 4);

	mData[index + 0] = q.x;
	mData[index + 1] = q.y;
	mData[index + 2] = q.z;
	mData[index + 3] = q.w;
}

vec4 AnimTexture::GetTexel(unsigned int x, unsigned int y) {
	unsigned int index = (y * mSize * 4) + (x * 4);

	return vec4(
		mData[index + 0],
		mData[index + 1],
		mData[index + 2],
		mData[index + 3]
	);
}