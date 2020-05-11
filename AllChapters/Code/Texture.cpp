#include "Texture.h"
#include "glad.h"
#include "stb_image.h"

Texture::Texture() { 
	mWidth = 0;
	mHeight = 0;
	mChannels = 0;
	glGenTextures(1, &mHandle);
}

Texture::Texture(const char* path) {
	glGenTextures(1, &mHandle);
	Load(path);
}

Texture::~Texture() { 
	glDeleteTextures(1, &mHandle);
}


void Texture::Load(const char* path) { 
	glBindTexture(GL_TEXTURE_2D, mHandle);

	int width, height, channels;
	unsigned char* data = stbi_load(path, &width, &height, &channels, 4);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);

	mWidth = width;
	mHeight = height;
	mChannels = channels;
}

void Texture::Set(unsigned int uniformIndex, unsigned int textureIndex) {
	glActiveTexture(GL_TEXTURE0 + textureIndex);
	glBindTexture(GL_TEXTURE_2D, mHandle);
	glUniform1i(uniformIndex, textureIndex); // Bind uniform X to sampler Y
}

void Texture::UnSet(unsigned int textureIndex) {
	glActiveTexture(GL_TEXTURE0 + textureIndex);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE0);
}

unsigned int Texture::GetHandle() { 
	return mHandle;
}