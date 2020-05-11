#ifndef _H_CROSSFADETARGET_
#define _H_CROSSFADETARGET_

#include "Pose.h"
#include "Clip.h"

struct CrossFadeTarget {
	Pose mPose;
	Clip* mClip;
	float mTime;
	float mDuration;
	float mElapsed;

	inline CrossFadeTarget() : mClip(0), mTime(0.0f), mDuration(0.0f), mElapsed(0.0f) { }
	inline CrossFadeTarget(Clip* target, Pose& pose, float duration) : mClip(target), mTime(target->GetStartTime()), mPose(pose), mDuration(duration), mElapsed(0.0f) { }
};


#endif