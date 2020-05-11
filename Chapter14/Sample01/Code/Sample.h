#ifndef _H_SAMPLE_
#define _H_SAMPLE_

#include "Application.h"
#include <vector>
#include "Pose.h"
#include "Clip.h"
#include "Skeleton.h"
#include "Mesh.h"
#include "Texture.h"
#include "Shader.h"
#include <vector>

class Sample : public Application {
protected:
	Texture* mDiffuseTexture;
	Shader* mLBSShader;
	Shader* mDQShader;
	std::vector<Mesh> mMeshes;
	Skeleton mSkeleton;
	Pose mCurrentPose;

	std::vector<DualQuaternion> mDqPosePalette;
	std::vector<DualQuaternion> mDqInvBindPalette;
	std::vector<mat4> mLbPosePalette;
	std::vector<mat4> mLbInvBindPalette;

	std::vector<Clip> mClips;
	unsigned int mCurrentClip;
	float mPlaybackTime;
public:
	void Initialize();
	void Update(float deltaTime);
	void Render(float inAspectRatio);
	void Shutdown();
};

#endif