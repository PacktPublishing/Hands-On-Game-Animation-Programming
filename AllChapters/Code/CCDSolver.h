#ifndef _H_CCDSOLVER_
#define _H_CCDSOLVER_

#include "Transform.h"
#include <vector>

class CCDSolver {
protected:
	std::vector<Transform> mIKChain;
	unsigned int mNumSteps;
	float mThreshold;
public:
	CCDSolver();

	unsigned int Size();
	void Resize(unsigned int newSize);
	Transform& operator[](unsigned int index);
	Transform GetLocalTransform(unsigned int index);
	void SetLocalTransform(unsigned int index, const Transform& t);
	Transform GetGlobalTransform(unsigned int index);

	unsigned int GetNumSteps();
	void SetNumSteps(unsigned int numSteps);

	float GetThreshold();
	void SetThreshold(float value);

	bool Solve(const Transform& target);
	bool Solve(const vec3& target);
};

#endif