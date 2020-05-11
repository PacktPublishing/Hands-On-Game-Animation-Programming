#include "IKLeg.h"

IKLeg::IKLeg(Skeleton& skeleton, const std::string& hip, const std::string& knee, const std::string& ankle, const std::string& toe) {
	mLineVisuals = new DebugDraw();
	mPointVisuals = new DebugDraw();

	mSolver.Resize(3);
	mPointVisuals->Resize(3);
	mLineVisuals->Resize(4);

	mAnkleToGroundOffset = 0.0f;

	mHipIndex = mKneeIndex = mAnkleIndex = mToeIndex = 0;
	for (unsigned int i = 0, size = skeleton.GetRestPose().Size(); i < size; ++i) {
		std::string& jointName = skeleton.GetJointName(i);
		if (jointName == hip) {
			mHipIndex = i;
		}
		else if (jointName == knee) {
			mKneeIndex = i;
		}
		else if (jointName == ankle) {
			mAnkleIndex = i;
		}
		else if (jointName == toe) {
			mToeIndex = i;
		}
	}
}

IKLeg::IKLeg() {
	mAnkleToGroundOffset = 0.0f;
	mLineVisuals = new DebugDraw();
	mPointVisuals = new DebugDraw();

	mSolver.Resize(3);
	mPointVisuals->Resize(3);
	mLineVisuals->Resize(4);
}

IKLeg::IKLeg(const IKLeg& other) {
	mLineVisuals = new DebugDraw();
	mPointVisuals = new DebugDraw();

	mAnkleToGroundOffset = 0.0f;
	mSolver.Resize(3);
	mPointVisuals->Resize(3);
	mLineVisuals->Resize(4);

	*this = other;
}

IKLeg& IKLeg::operator=(const IKLeg& other) {
	if (this == &other) {
		return *this;
	}

	mSolver = other.mSolver;
	mAnkleToGroundOffset = other.mAnkleToGroundOffset;
	mHipIndex = other.mHipIndex;
	mKneeIndex = other.mKneeIndex;
	mAnkleIndex = other.mAnkleIndex;
	mToeIndex = other.mToeIndex;

	return *this;
}

IKLeg::~IKLeg() {
	delete mLineVisuals;
	delete mPointVisuals;
}

void IKLeg::SolveForLeg(const Transform& model, Pose& pose, const vec3& ankleTargetPosition) {
	mSolver.SetLocalTransform(0, combine(model, pose.GetGlobalTransform(mHipIndex)));
	mSolver.SetLocalTransform(1, pose.GetLocalTransform(mKneeIndex));
	mSolver.SetLocalTransform(2, pose.GetLocalTransform(mAnkleIndex));
	mIKPose = pose;

	Transform target(ankleTargetPosition + vec3(0, 1, 0) * mAnkleToGroundOffset, quat(), vec3(1, 1, 1));
	mSolver.Solve(target);

	Transform rootWorld = combine(model, pose.GetGlobalTransform(pose.GetParent(mHipIndex)));
	mIKPose.SetLocalTransform(mHipIndex, combine(inverse(rootWorld), mSolver.GetLocalTransform(0)));
	mIKPose.SetLocalTransform(mKneeIndex, mSolver.GetLocalTransform(1));
	mIKPose.SetLocalTransform(mAnkleIndex, mSolver.GetLocalTransform(2));

	mLineVisuals->LinesFromIKSolver(mSolver);
	mPointVisuals->PointsFromIKSolver(mSolver);
}

Pose& IKLeg::GetAdjustedPose() {
	return mIKPose;
}

ScalarTrack& IKLeg::GetTrack() {
	return mPinTrack;
}

unsigned int IKLeg::Hip() {
	return mHipIndex;
}

unsigned int IKLeg::Knee() {
	return mKneeIndex;
}

unsigned int IKLeg::Ankle() {
	return mAnkleIndex;
}

unsigned int IKLeg::Toe() {
	return mToeIndex;
}

void IKLeg::Draw(const mat4& mv, const vec3& legColor) {
	mLineVisuals->UpdateOpenGLBuffers();
	mPointVisuals->UpdateOpenGLBuffers();
	mLineVisuals->Draw(DebugDrawMode::Lines, legColor, mv);
	mPointVisuals->Draw(DebugDrawMode::Points, legColor, mv);
}

void IKLeg::SetAnkleOffset(float offset) {
	mAnkleToGroundOffset = offset;
}