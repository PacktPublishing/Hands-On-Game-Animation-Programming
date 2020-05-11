#include "Skeleton.h"

Skeleton::Skeleton() { }

Skeleton::Skeleton(const Pose& rest, const Pose& bind, const std::vector<std::string>& names) {
	Set(rest, bind, names);
}

void Skeleton::Set(const Pose& rest, const Pose& bind, const std::vector<std::string>& names) {
	mRestPose = rest;
	mBindPose = bind;
	mJointNames = names;
	UpdateInverseBindPose();
}

void Skeleton::UpdateInverseBindPose() {
	unsigned int size = mBindPose.Size();
	mInvBindPose.resize(size);

	for (unsigned int i = 0; i < size; ++i) {
		Transform world = mBindPose.GetGlobalTransform(i);
#if 1
		mInvBindPose[i] = inverse(transformToMat4(world));
#else
		// This is a bit safer and more numerically stable
		mInvBindPose[i] = transformToMat4(inverse(world));
#endif
	}
}

void Skeleton::GetInvBindPose(std::vector<DualQuaternion>& outInvBndPose) {
	unsigned int size = mBindPose.Size();
	outInvBndPose.resize(size);

	for (unsigned int i = 0; i < size; ++i) {
		DualQuaternion world = mBindPose.GetGlobalDualQuaternion(i);
		outInvBndPose[i] = conjugate(world);
	}
}

Pose& Skeleton::GetBindPose() {
	return mBindPose;
}

Pose& Skeleton::GetRestPose() {
	return mRestPose;
}

std::vector<mat4>& Skeleton::GetInvBindPose() {
	return mInvBindPose;
}

std::vector<std::string>& Skeleton::GetJointNames() {
	return mJointNames;
}

std::string& Skeleton::GetJointName(unsigned int index) {
	return mJointNames[index];
}