#ifndef _H_CHAPTER12SAMPLE02_
#define _H_CHAPTER12SAMPLE02_

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
#include "SamplePoseInstance.h"
#include "Blending.h"
#include "CrossFadeController.h"

class Chapter12Sample02 : public Application {
protected:
	Texture* mDiffuseTexture;
	Shader* mStaticShader;
	Shader* mSkinnedShader;
	std::vector<Mesh> mMeshes;
	std::vector<mat4> mPosePalette;
	Skeleton mSkeleton;
	std::vector<Clip> mClips;

	DebugDraw* mBindPoseVisual;
	DebugDraw* mRestPoseVisual;
	DebugDraw* mCurrentPoseVisual;

	CrossFadeController mController;
	unsigned int mNextAnim;
	float mFadeTime;

	char** mUIClipNames;
	char** mSkinningNames;
	unsigned int mNumUIClips;
	bool mShowRestPose;
	bool mShowCurrentPose;
	bool mShowBindPose;
	bool mShowMesh;
	SkinningType mSkinType;
public:
	void Initialize();
	void Update(float deltaTime);
	void Render(float inAspectRatio);
	void Shutdown();
	void ImGui(nk_context* inContext);
};

#endif