#ifndef _H_SAMPLE_
#define _H_SAMPLE_

#include "Application.h"
#include "Skeleton.h"
#include "Pose.h"
#include "Clip.h"
#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"

struct AnimationInstance {
	unsigned int mClip;
	float mTime;
	Pose mPose;
};

class Sample : public Application {
protected:
	Shader* mShader;
	Texture* mTexture;
	std::vector<Mesh> mMeshes;
	std::vector<Clip> mClips;
	Skeleton mSkeleton;
	Pose mPose;
	std::vector<mat4> mPosePalette;
	std::vector<mat4> mSkinPalette;

	AnimationInstance mA;
	AnimationInstance mB;
	float mBlendTime;
	bool mInvertBlend;
public:
	void Initialize();
	void Update(float inDeltaTime);
	void Render(float inAspectRatio);
	void Shutdown();
};

#endif