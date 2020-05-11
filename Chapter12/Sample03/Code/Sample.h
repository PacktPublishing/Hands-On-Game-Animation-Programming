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

	unsigned int mClip;
	unsigned int mAdditiveIndex;

	float mPlaybackTime;
	float mAdditiveTime;
	float mAdditiveDirection;

	Pose mCurrentPose;
	Pose mAddPose;
	Pose mAdditiveBase;
public:
	void Initialize();
	void Update(float inDeltaTime);
	void Render(float inAspectRatio);
	void Shutdown();
};

#endif