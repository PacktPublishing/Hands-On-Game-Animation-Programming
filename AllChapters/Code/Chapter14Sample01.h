#ifndef _H_CHAPTER14SAMPLE01_
#define _H_CHAPTER14SAMPLE01_

#include "Application.h"
#include "DebugDraw.h"
#include <vector>
#include "Pose.h"
#include "Clip.h"
#include "Skeleton.h"
#include "Mesh.h"
#include "Texture.h"
#include "SkinningType.h"
#include <vector>

class Chapter14Sample01 : public Application {
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

	char** mSkinningNames;
	int mSkinningMethod;
public:
	void Initialize();
	void Update(float deltaTime);
	void Render(float inAspectRatio);
	void Shutdown();
	void ImGui(nk_context* inContext);
};

#endif