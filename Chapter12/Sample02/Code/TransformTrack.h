#ifndef _H_TRANSFORMTRACK_
#define _H_TRANSFORMTRACK_

#include "Track.h"
#include "Transform.h"

template <typename VTRACK, typename QTRACK>
class TTransformTrack {
protected:
	unsigned int mId;
	VTRACK mPosition;
	QTRACK mRotation;
	VTRACK mScale;
public:
	TTransformTrack();
	unsigned int GetId();
	void SetId(unsigned int id);
	VTRACK& GetPositionTrack();
	QTRACK& GetRotationTrack();
	VTRACK& GetScaleTrack();
	float GetStartTime();
	float GetEndTime();
	bool IsValid();
	Transform Sample(const Transform& ref, float time, bool looping);
};

typedef TTransformTrack<VectorTrack, QuaternionTrack> TransformTrack;
typedef TTransformTrack<FastVectorTrack, FastQuaternionTrack> FastTransformTrack;

FastTransformTrack OptimizeTransformTrack(TransformTrack& input);

#endif