#include "TransformTrack.h"

template TTransformTrack<VectorTrack, QuaternionTrack>;
template TTransformTrack<FastVectorTrack, FastQuaternionTrack>;

template <typename VTRACK, typename QTRACK>
TTransformTrack<VTRACK, QTRACK>::TTransformTrack() {
	mId = 0;
}

template <typename VTRACK, typename QTRACK>
unsigned int TTransformTrack<VTRACK, QTRACK>::GetId() {
	return mId;
}

template <typename VTRACK, typename QTRACK>
void TTransformTrack<VTRACK, QTRACK>::SetId(unsigned int id) {
	mId = id;
}

template <typename VTRACK, typename QTRACK>
VTRACK& TTransformTrack<VTRACK, QTRACK>::GetPositionTrack() {
	return mPosition;
}

template <typename VTRACK, typename QTRACK>
QTRACK& TTransformTrack<VTRACK, QTRACK>::GetRotationTrack() {
	return mRotation;
}

template <typename VTRACK, typename QTRACK>
VTRACK& TTransformTrack<VTRACK, QTRACK>::GetScaleTrack() {
	return mScale;
}

template <typename VTRACK, typename QTRACK>
bool TTransformTrack<VTRACK, QTRACK>::IsValid() {
	return mPosition.Size() > 1 || mRotation.Size() > 1 || mScale.Size() > 1;
}

template <typename VTRACK, typename QTRACK>
float TTransformTrack<VTRACK, QTRACK>::GetStartTime() {
	float result = 0.0f;
	bool isSet = false;

	if (mPosition.Size() > 1) {
		result = mPosition.GetStartTime();
		isSet = true;
	}
	if (mRotation.Size() > 1) {
		float rotationStart = mRotation.GetStartTime();
		if (rotationStart < result || !isSet) {
			result = rotationStart;
			isSet = true;
		}
	}
	if (mScale.Size() > 1) {
		float scaleStart = mScale.GetStartTime();
		if (scaleStart < result || !isSet) {
			result = scaleStart;
			isSet = true;
		}
	}

	return result;
}

template <typename VTRACK, typename QTRACK>
float TTransformTrack<VTRACK, QTRACK>::GetEndTime() {
	float result = 0.0f;
	bool isSet = false;

	if (mPosition.Size() > 1) {
		result = mPosition.GetEndTime();
		isSet = true;
	}
	if (mRotation.Size() > 1) {
		float rotationEnd = mRotation.GetEndTime();
		if (rotationEnd > result || !isSet) {
			result = rotationEnd;
			isSet = true;
		}
	}
	if (mScale.Size() > 1) {
		float scaleEnd = mScale.GetEndTime();
		if (scaleEnd > result || !isSet) {
			result = scaleEnd;
			isSet = true;
		}
	}

	return result;
}

template <typename VTRACK, typename QTRACK>
Transform TTransformTrack<VTRACK, QTRACK>::Sample(const Transform& ref,
	float time, bool looping) {
	Transform result = ref; // Assign default values
	if (mPosition.Size() > 1) { // Only assign if animated
		result.position = mPosition.Sample(time, looping);
	}
	if (mRotation.Size() > 1) { // Only assign if animated
		result.rotation = mRotation.Sample(time, looping);
	}
	if (mScale.Size() > 1) { // Only assign if animated
		result.scale = mScale.Sample(time, looping);
	}
	return result;
}

FastTransformTrack OptimizeTransformTrack(TransformTrack& input) {
	FastTransformTrack result;

	result.SetId(input.GetId());
	result.GetPositionTrack() = OptimizeTrack<vec3, 3>(input.GetPositionTrack());
	result.GetRotationTrack() = OptimizeTrack<quat, 4>(input.GetRotationTrack());
	result.GetScaleTrack() = OptimizeTrack<vec3, 3>(input.GetScaleTrack());

	return result;
}