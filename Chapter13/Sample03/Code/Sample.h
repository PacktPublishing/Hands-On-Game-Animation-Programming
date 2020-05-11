#ifndef _H_SAMPLE_
#define _H_SAMPLE_

#include "Application.h"
#include "DebugDraw.h"
#include <vector>
#include "Pose.h"
#include "Clip.h"
#include "Skeleton.h"
#include "Mesh.h"
#include "Texture.h"
#include <vector>
#include "Intersections.h"
#include "IKLeg.h"

class Sample : public Application {
protected:
	Texture* mCourseTexture;
	std::vector<Mesh> mIKCourse;
	std::vector<Triangle> mTriangles;

	VectorTrack mMotionTrack;
	float mWalkingTime;

	Transform mModel;
	std::vector<Mesh> mMeshes;
	Pose mCurrentPose;
	std::vector<mat4> mPosePalette;
	float mSinkIntoGround;

	IKLeg* mLeftLeg;
	IKLeg* mRightLeg;
	float mToeLength;
	float mLastModelY;

	Texture* mDiffuseTexture;
	Shader* mStaticShader;
	Shader* mSkinnedShader;
	Skeleton mSkeleton;
	std::vector<Clip> mClips;
	unsigned int mCurrentClip;
	float mPlaybackTime;
	DebugDraw* mCurrentPoseVisual;

	bool mShowIKPose;
	bool mShowCurrentPose;
	float mTimeMod;
	bool mDepthTest;
	bool mShowMesh;
	bool mShowEnvironment;
public:
	void Initialize();
	void Update(float deltaTime);
	void Render(float inAspectRatio);
	void Shutdown();
};

#endif