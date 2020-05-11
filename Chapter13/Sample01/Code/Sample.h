#ifndef _H_SAMPLE_
#define _H_SAMPLE_

#include "Application.h"
#include "DebugDraw.h"
#include <vector>
#include <vector>
#include "TransformTrack.h"

class Sample : public Application {
protected:
	Transform mTarget;
	CCDSolver mSolver;
	DebugDraw* mSolverLines;
	DebugDraw* mSolverPoints;
	DebugDraw* mTargetVisual[3];
	TransformTrack mTargetPath;
	float mPlayTime;
	float mCamPitch;
	float mCamYaw;
	float mCamDist;
protected:
	void SetFrame(VectorTrack& track, int index, float time, const vec3& value);
public:
	void Initialize();
	void Update(float deltaTime);
	void Render(float inAspectRatio);
	void Shutdown();
};

#endif