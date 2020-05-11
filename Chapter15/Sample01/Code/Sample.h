#ifndef _H_CHAPTER15SAMPLE01_
#define _H_CHAPTER15SAMPLE01_

#include "Application.h"
#include <vector>
#include "Pose.h"
#include "Clip.h"
#include "Skeleton.h"
#include "Mesh.h"
#include "Texture.h"
#include "AnimTexture.h"
#include <vector>
#include "Crowd.h"

class Sample : public Application {
protected:
	Texture* mDiffuseTexture;
	Shader* mCrowdShader;
	std::vector<Mesh> mMeshes;
	Skeleton mSkeleton;
	std::vector<Clip> mClips;
	std::vector<AnimTexture> mTextures;
	std::vector<Crowd> mCrowds;
protected:
	void SetCrowdSize(unsigned int size);
public:
	void Initialize();
	void Update(float deltaTime);
	void Render(float inAspectRatio);
	void Shutdown();
};

#endif