#ifndef _H_POSE_
#define _H_POSE_

#include <vector>
#include "Transform.h"

class Pose {
protected:
	std::vector<Transform> mJoints;
	std::vector<int> mParents;
public:
	Pose();
	Pose(const Pose& p);
	Pose& operator=(const Pose& p);
	Pose(unsigned int numJoints);
	void Resize(unsigned int size);
	unsigned int Size();
	Transform GetLocalTransform(unsigned int index);
	void SetLocalTransform(unsigned int index, const Transform& transform);
	Transform GetGlobalTransform(unsigned int index);
	Transform operator[](unsigned int index);
	void GetMatrixPalette(std::vector<mat4>& out);
	int GetParent(unsigned int index);
	void SetParent(unsigned int index, int parent);

	bool operator==(const Pose& other);
	bool operator!=(const Pose& other);
};

#endif // !_H_POSE_
