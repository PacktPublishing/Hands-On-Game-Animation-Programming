#ifndef _H_ANIM_TEXTURE_
#define _H_ANIM_TEXTURE_

#include "vec4.h"
#include "quat.h"

class AnimTexture {
protected:
	float* mData;
	unsigned int mSize;
	unsigned int mHandle;
public:
	AnimTexture();
	AnimTexture(const AnimTexture&);
	AnimTexture& operator=(const AnimTexture&);
	AnimTexture(const char* path);
	AnimTexture(unsigned int size);
	~AnimTexture();

	void Load(const char* path);
	void Save(const char* path);
	void UploadTextureDataToGPU();

	unsigned int Size();
	void Resize(unsigned int newSize);
	float* GetData();

	void Set(unsigned int uniformIndex, unsigned int textureIndex);
	void UnSet(unsigned int textureIndex);
	unsigned int GetHandle();

	void SetTexel(unsigned int x, unsigned int y, const vec3& v);
	void SetTexel(unsigned int x, unsigned int y, const quat& q);
	vec4 GetTexel(unsigned int x, unsigned int y);
};

#endif