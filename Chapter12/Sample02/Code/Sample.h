#ifndef _H_SAMPLE_
#define _H_SAMPLE_

#include "Application.h"
#include "Skeleton.h"
#include "Pose.h"
#include "Clip.h"
#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"
#include "CrossFadeController.h"
#include "CrossFadeTarget.h"

class Sample : public Application {
protected:
	Shader* mShader;
	Texture* mTexture;
	std::vector<Mesh> mMeshes;
	std::vector<Clip> mClips;
	Skeleton mSkeleton;
	std::vector<mat4> mPosePalette;

	CrossFadeController mFadeController;
	unsigned int mCurrentClip;
	float mFadeTimer;
public:
	void Initialize();
	void Update(float inDeltaTime);
	void Render(float inAspectRatio);
	void Shutdown();
};

#endif