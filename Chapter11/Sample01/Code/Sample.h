#ifndef _H_CHAPTER11SAMPLE01_
#define _H_CHAPTER11SAMPLE01_

#include "Application.h"
#include <vector>
#include "Pose.h"
#include "Clip.h"
#include "Skeleton.h"
#include "Texture.h"
#include "Mesh.h"
#include <vector>
#include "Shader.h"

struct AnimationInstance {
	Pose mAnimatedPose;
	std::vector <mat4> mPosePalette;
	unsigned int mClip;
	float mPlayback;
	Transform mModel;

	inline AnimationInstance() : mClip(0), mPlayback(0.0f) { }
};

class Sample : public Application {
protected:
	Texture* mDiffuseTexture;
	Shader* mStaticShader;
	Shader* mSkinnedShader;
	Skeleton mSkeleton;
	std::vector<Clip> mClips;

	std::vector<Mesh> mCPUMeshes;
	std::vector<Mesh> mGPUMeshes;
	AnimationInstance mCPUInstance;
	AnimationInstance mGPUInstance;
public:
	void Initialize();
	void Update(float deltaTime);
	void Render(float inAspectRatio);
	void Shutdown();
};

#endif