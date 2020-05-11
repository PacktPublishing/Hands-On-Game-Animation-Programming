#ifndef _H_CLIP_
#define _H_CLIP_

#include <vector>
#include <string>
#include "TransformTrack.h"
#include "Pose.h"

class Clip {
protected:
	std::vector<TransformTrack> mTracks;
	std::string mName;
	float mStartTime;
	float mEndTime;
	bool mLooping;
protected:
	float AdjustTimeToFitRange(float inTime);
public:
	Clip();
	unsigned int GetIdAtIndex(unsigned int index);
	void SetIdAtIndex(unsigned int index, unsigned int id);
	unsigned int Size();
	float Sample(Pose& outPose, float inTime);
	TransformTrack& operator[](unsigned int index);
	void RecalculateDuration();
	std::string& GetName();
	void SetName(const std::string& inNewName);
	float GetDuration();
	float GetStartTime();
	float GetEndTime();
	bool GetLooping();
	void SetLooping(bool inLooping);
};

#endif 