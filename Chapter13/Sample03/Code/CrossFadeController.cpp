#include "CrossFadeController.h"
#include "Blending.h"

CrossFadeController::CrossFadeController() {
	mClip = 0;
	mTime = 0.0f;
	mWasSkeletonSet = false;
}

CrossFadeController::CrossFadeController(Skeleton& skeleton) {
	mClip = 0;
	mTime = 0.0f;
	SetSkeleton(skeleton);
}

void CrossFadeController::SetSkeleton(Skeleton& skeleton) {
	mSkeleton = skeleton;
	mPose = mSkeleton.GetRestPose();
	mWasSkeletonSet = true;
}

void CrossFadeController::Play(Clip* target) {
	mTargets.clear();
	mClip = target;
	mPose = mSkeleton.GetRestPose();
	mTime = target->GetStartTime();
}

void CrossFadeController::FadeTo(Clip* target, float fadeTime) {
	if (mClip == 0) {
		Play(target);
		return;
	}

	if (mTargets.size() >= 1) {
		if (mTargets[mTargets.size() - 1].mClip == target) {
			return;
		}
	}
	else {
		if (mClip == target) {
			return;
		}
	}

	mTargets.push_back(CrossFadeTarget(target, mSkeleton.GetRestPose(), fadeTime));
}

void CrossFadeController::Update(float dt) {
	if (mClip == 0 || !mWasSkeletonSet) {
		return;
	}

	unsigned int numTargets = mTargets.size();
	for (unsigned int i = 0; i < numTargets; ++i) {
		if (mTargets[i].mElapsed >= mTargets[i].mDuration) {
			mClip = mTargets[i].mClip;
			mTime = mTargets[i].mTime;
			mPose = mTargets[i].mPose;

			mTargets.erase(mTargets.begin() + i);
			break;
		}
	}
	numTargets = mTargets.size();
	mPose = mSkeleton.GetRestPose();
	mTime = mClip->Sample(mPose, mTime + dt);

	for (unsigned int i = 0; i < numTargets; ++i) {
		CrossFadeTarget& target = mTargets[i];
		target.mTime = target.mClip->Sample(target.mPose, target.mTime + dt);
		target.mElapsed += dt;
		float t = target.mElapsed / target.mDuration;
		if (t > 1.0f) { t = 1.0f; }

		Blend(mPose, mPose, target.mPose, t, -1);
	}
}

Pose& CrossFadeController::GetCurrentPose() {
	return mPose;
}

Clip* CrossFadeController::GetcurrentClip() {
	return mClip;
}
