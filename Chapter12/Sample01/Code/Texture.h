#ifndef _H_TEXTURE_
#define _H_TEXTURE_

class Texture {
protected:
	unsigned int mWidth;
	unsigned int mHeight;
	unsigned int mChannels;
	unsigned int mHandle;
private:
	Texture(const Texture& other);
	Texture& operator=(const Texture& other);
public:
	Texture();
	Texture(const char* path);
	~Texture();

	void Load(const char* path);

	void Set(unsigned int uniformIndex, unsigned int textureIndex);
	void UnSet(unsigned int textureIndex);
	unsigned int GetHandle();
};

#endif