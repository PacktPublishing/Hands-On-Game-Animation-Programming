#ifndef _H_ANIMTEXTURE_
#define _H_ANIMTEXTURE_

#include "vec3.h"
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