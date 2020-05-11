#include "FABRIKSolver.h"

FABRIKSolver::FABRIKSolver() {
	mNumSteps = 15;
	mThreshold = 0.00001f;
}

unsigned int FABRIKSolver::GetNumSteps() {
	return mNumSteps;
}

void FABRIKSolver::SetNumSteps(unsigned int numSteps) {
	mNumSteps = numSteps;
}

float FABRIKSolver::GetThreshold() {
	return mThreshold;
}

void FABRIKSolver::SetThreshold(float value) {
	mThreshold = value;
}

unsigned int FABRIKSolver::Size() {
	return mIKChain.size();
}

void FABRIKSolver::Resize(unsigned int newSize) {
	mIKChain.resize(newSize);
	mWorldChain.resize(newSize);
	mLengths.resize(newSize);
}

Transform FABRIKSolver::GetLocalTransform(unsigned int index) {
	return mIKChain[index];
}

void FABRIKSolver::SetLocalTransform(unsigned int index, const Transform& t) {
	mIKChain[index] = t;
}

Transform FABRIKSolver::GetGlobalTransform(unsigned int index) {
	unsigned int size = (unsigned int)mIKChain.size();
	Transform world = mIKChain[index];
	for (int i = (int)index - 1; i >= 0; --i) {
		world = combine(mIKChain[i], world);
	}
	return world;
}

void FABRIKSolver::IKChainToWorld() {
	unsigned int size = Size();
	for (unsigned int i = 0; i < size; ++i) {
		Transform world = GetGlobalTransform(i);
		mWorldChain[i] = world.position;

		if (i >= 1) {
			vec3 prev = mWorldChain[i - 1];
			mLengths[i] = len(world.position - prev);
		}
	}
	if (size > 0) {
		mLengths[0] = 0.0f;
	}
}

void FABRIKSolver::WorldToIKChain() {
	unsigned int size = Size();
	if (size == 0) { return; }

	for (unsigned int i = 0; i < size - 1; ++i) {
		Transform world = GetGlobalTransform(i);
		Transform next = GetGlobalTransform(i + 1);
		vec3 position = world.position;
		quat rotation = world.rotation;

		vec3 toNext = next.position - position;
		toNext = inverse(rotation) * toNext;

		vec3 toDesired = mWorldChain[i + 1] - position;
		toDesired = inverse(rotation) * toDesired;

		quat delta = fromTo(toNext, toDesired);
		mIKChain[i].rotation = delta * mIKChain[i].rotation;
	}
}

void FABRIKSolver::IterateBackward(const vec3& goal) {
	int size = (int)Size();
	if (size > 0) {
		mWorldChain[size - 1] = goal;
	}

	for (int i = size - 2; i >= 0; --i) {
		vec3 direction = normalized(mWorldChain[i] - mWorldChain[i + 1]);
		vec3 offset = direction * mLengths[i + 1];
		mWorldChain[i] = mWorldChain[i + 1] + offset;
	}
}

void FABRIKSolver::IterateForward(const vec3& base) {
	unsigned int size = Size();
	if (size > 0) {
		mWorldChain[0] = base;
	}

	for (unsigned int i = 1; i < size; ++i) {
		vec3 direction = normalized(mWorldChain[i] - mWorldChain[i - 1]);
		vec3 offset = direction * mLengths[i];
		mWorldChain[i] = mWorldChain[i - 1] + offset;
	}
}

bool FABRIKSolver::Solve(const Transform& target) {
	unsigned int size = Size();
	if (size == 0) { return false; }
	unsigned int last = size - 1;
	float thresholdSq = mThreshold * mThreshold;

	IKChainToWorld();
	vec3 goal = target.position;
	vec3 base = mWorldChain[0];

	for (unsigned int i = 0; i < mNumSteps; ++i) {
		vec3 effector = mWorldChain[last];
		if (lenSq(goal - effector) < thresholdSq) {
			WorldToIKChain();
			return true;
		}

		IterateBackward(goal);
		IterateForward(base);
	}

	WorldToIKChain();
	vec3 effector = GetGlobalTransform(last).position;
	if (lenSq(goal - effector) < thresholdSq) {
		return true;
	}

	return false;
}
