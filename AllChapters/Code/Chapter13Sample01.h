#ifndef _H_CHAPTER13SAMPLE01_
#define _H_CHAPTER13SAMPLE01_

#include "Application.h"
#include "DebugDraw.h"
#include "CCDSolver.h"
#include <vector>
#include <vector>

class Chapter13Sample01 : public Application {
protected:
	Transform mTarget;
	CCDSolver mSolver;
	DebugDraw* mCCDLines;
	DebugDraw* mCCDPoints;
	DebugDraw* mTargetVisual[3];
	float mCamPitch;
	float mCamYaw;
	float mCamDist;
public:
	void Initialize();
	void Update(float deltaTime);
	void Render(float inAspectRatio);
	void Shutdown();
	void ImGui(nk_context* inContext);
};

#endif