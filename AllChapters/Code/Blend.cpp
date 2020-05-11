#include "Blending.h"
#include <iostream>

bool IsInHierarchy(Pose& pose, unsigned int parent, unsigned int search) {
	if (search == parent) {
		return true;
	}
	int p = pose.GetParent(search);

	while (p >= 0) {
		if (p == (int)parent) {
			return true;
		}
		p = pose.GetParent(p);
	}

	return false;
}

void Blend(Pose& output, Pose& a, Pose& b, float t, int blendroot) {
#if _DEBUG
	if (a.Size() != b.Size()) {
		std::cout << "Input poses have differing sizes\n";
	}
#endif 

	unsigned int numJoints = output.Size();
	for (unsigned int i = 0; i < numJoints; ++i) {
		if (blendroot >= 0) {
			if (!IsInHierarchy(output, (unsigned int)blendroot, i)) {
				continue;
			}
		}

		output.SetLocalTransform(i, mix(a.GetLocalTransform(i), b.GetLocalTransform(i), t));
	}
}

Pose MakeAdditivePose(Skeleton& skeleton, Clip& clip) {
	Pose result = skeleton.GetRestPose();
	clip.Sample(result, clip.GetStartTime());
	return result;
}

void Add(Pose& output, Pose& inPose, Pose& addPose, Pose& basePose, int blendroot) {
	unsigned int numJoints = addPose.Size();
	for (unsigned int i = 0; i < numJoints; ++i) {
		Transform input = inPose.GetLocalTransform(i);
		Transform additive = addPose.GetLocalTransform(i);
		Transform additiveBase = basePose.GetLocalTransform(i);

		if (blendroot >= 0 && !IsInHierarchy(addPose, blendroot, i)) {
			continue;
		}

		// outPose = inPose + (addPose - basePose)
		Transform result(
			input.position + (additive.position - additiveBase.position),
			normalized(input.rotation * (inverse(additiveBase.rotation) * additive.rotation)),
			input.scale + (additive.scale - additiveBase.scale)
		);
		output.SetLocalTransform(i, result);
	}
}