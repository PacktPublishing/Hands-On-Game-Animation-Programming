#include "Track.h"
#include <iostream>

template Track<float, 1>;
template Track<vec3, 3>;
template Track<quat, 4>;

template FastTrack<float, 1>;
template FastTrack<vec3, 3>;
template FastTrack<quat, 4>;

namespace TrackHelpers {
	inline float Interpolate(float a, float b, float t) {
		return a + (b - a) * t;
	}

	inline vec3 Interpolate(const vec3& a, const vec3& b, float t) {
		return lerp(a, b, t);
	}

	inline quat Interpolate(const quat& a, const quat& b, float t) {
		quat result = mix(a, b, t);
		if (dot(a, b) < 0.0f) {
			result = mix(a, -b, t);
		}
		return normalized(result);
	}

	inline float AdjustHermiteResult(float f) {
		return f;
	}

	inline vec3 AdjustHermiteResult(const vec3& v) {
		return v;
	}

	inline quat AdjustHermiteResult(const quat& q) {
		return normalized(q);
	}

	inline void Neighborhood(const float& a, float& b) { }
	inline void Neighborhood(const vec3& a, const vec3& b) { }
	inline void Neighborhood(const quat& a, quat& b) { 
		if (dot(a, b) < 0.0f) {
			b = b * -1.0f;
		}
	}
};

template<typename T, int N>
Track<T, N>::Track() {
	mInterpolation = Interpolation::Linear;
}

template<typename T, int N>
T Track<T, N>::Sample(float time, bool looping) {
	if (mInterpolation == Interpolation::Constant) {
		return SampleConstant(time, looping);
	}
	else if (mInterpolation == Interpolation::Linear) {
		return SampleLinear(time, looping);
	}
	return SampleCubic(time, looping);
}

template<typename T, int N>
Frame<N>& Track<T, N>::operator[](unsigned int index) {
	return mFrames[index];
}

template<typename T, int N>
void Track<T, N>::Resize(unsigned int size) {
	mFrames.resize(size);
}

template<typename T, int N>
unsigned int Track<T, N>::Size() {
	return (unsigned int)mFrames.size();
}

template<typename T, int N>
Interpolation Track<T, N>::GetInterpolation() {
	return mInterpolation;
}

template<typename T, int N>
void Track<T, N>::SetInterpolation(Interpolation interpolation) {
	mInterpolation = interpolation;
}

template<typename T, int N>
T Track<T, N>::Hermite(float t, const T& p1, const T& s1, const T& _p2, const T& s2) {
	float tt = t * t;
	float ttt = tt * t;

	T p2 = _p2;
	TrackHelpers::Neighborhood(p1, p2);

	float h1 = 2.0f * ttt - 3.0f * tt + 1.0f;
	float h2 = -2.0f * ttt + 3.0f * tt;
	float h3 = ttt - 2.0f * tt + t;
	float h4 = ttt - tt;

	T result = (p1 * h1) + (p2 * h2) + (s1 * h3) + (s2 * h4);
	return TrackHelpers::AdjustHermiteResult(result);
}

template<typename T, int N>
int Track<T, N>::FrameIndex(float time, bool looping) {
	unsigned int size = (unsigned int)mFrames.size();
	if (size <= 1) {
		return -1;
	}

	if (looping) {
		float startTime = mFrames[0].mTime;
		float endTime = mFrames[size - 1].mTime;
		float duration = endTime - startTime;

		if (duration < 0.0f) {
			std::cout << "Warning, frame has negative duration!\n";
			duration = fabsf(duration);
		}

		time = fmodf(time - startTime, endTime - startTime);
		if (time < 0.0f) {
			time += endTime - startTime;
		}
		time = time + startTime;

		
	}
	else {
		if (time <= mFrames[0].mTime) {
			return 0;
		}
		if (time >= mFrames[size - 2].mTime) {
			return (int)size - 2;
		}
	}

	for (int i = (int)size - 1; i >= 0; --i) {
		if (time >= mFrames[i].mTime) {
			return i;
		}
	}

	// Invalid code, we should not reach here!
	return -1;
}

template<typename T, int N>
float Track<T, N>::AdjustTimeToFitTrack(float time, bool looping) {
	unsigned int size = (unsigned int)mFrames.size();
	if (size <= 1) {
		return 0.0f;
	}

	float startTime = mFrames[0].mTime;
	float endTime = mFrames[size - 1].mTime;
	float duration = endTime - startTime;
	if (duration <= 0.0f) {
		return 0.0f;
	}
	if (looping) {
		time = fmodf(time - startTime, endTime - startTime);
		if (time < 0.0f) {
			time += endTime - startTime;
		}
		time = time + startTime;
	}
	else {
		if (time <= mFrames[0].mTime) {
			time = startTime;
		}
		if (time >= mFrames[size - 1].mTime) {
			time = endTime;
		}
	}

	return time;
}

template<> float Track<float, 1>::Cast(float* value) {
	return value[0];
}

template<> vec3 Track<vec3, 3>::Cast(float* value) {
	return vec3(value[0], value[1], value[2]);
}

template<> quat Track<quat, 4>::Cast(float* value) {
	quat r = quat(value[0], value[1], value[2], value[3]);
	return normalized(r);
}

template<typename T, int N>
T Track<T, N>::SampleConstant(float time, bool looping) {
	int frame = FrameIndex(time, looping);
	if (frame < 0 || frame >= (int)mFrames.size()) {
		return T();
	}

	return Cast(&mFrames[frame].mValue[0]);
}

template<typename T, int N>
T Track<T, N>::SampleLinear(float time, bool looping) {
	int thisFrame = FrameIndex(time, looping);
	if (thisFrame < 0 || thisFrame >= (int)(mFrames.size() - 1)) {
		return T();
	}
	int nextFrame = thisFrame + 1;

	float trackTime = AdjustTimeToFitTrack(time, looping);
	float frameDelta = mFrames[nextFrame].mTime - mFrames[thisFrame].mTime;
	if (frameDelta <= 0.0f) {
		return T();
	}
	float t = (trackTime - mFrames[thisFrame].mTime) / frameDelta;

	T start = Cast(&mFrames[thisFrame].mValue[0]);
	T end = Cast(&mFrames[nextFrame].mValue[0]);

	return TrackHelpers::Interpolate(start, end, t);
}

template<typename T, int N>
T Track<T, N>::SampleCubic(float time, bool looping) {
	int thisFrame = FrameIndex(time, looping);
	if (thisFrame < 0 || thisFrame >= (int)(mFrames.size() - 1)) {
		return T();
	}
	int nextFrame = thisFrame + 1;

	float trackTime = AdjustTimeToFitTrack(time, looping);
	float frameDelta = mFrames[nextFrame].mTime - mFrames[thisFrame].mTime;
	if (frameDelta <= 0.0f) {
		return T();
	}
	float t = (trackTime - mFrames[thisFrame].mTime) / frameDelta;

	T point1 = Cast(&mFrames[thisFrame].mValue[0]);
	// We can't just use Cast here, since cast would normalize a quaternion
	// which would ruin the slope. We want a ray memory copy here.
	T slope1;// = mFrames[thisFrame].mOut * frameDelta;
	memcpy(&slope1, mFrames[thisFrame].mOut, N * sizeof(float));
	slope1 = slope1 * frameDelta;

	T point2 = Cast(&mFrames[nextFrame].mValue[0]);
	T slope2;// = mFrames[nextFrame].mIn * frameDelta;
	memcpy(&slope2, mFrames[nextFrame].mIn, N * sizeof(float));
	slope2 = slope2 * frameDelta;

	return Hermite(t, point1, slope1, point2, slope2);
}

template<typename T, int N>
float Track<T, N>::GetStartTime() {
	return mFrames[0].mTime;
}

template<typename T, int N>
float Track<T, N>::GetEndTime() {
	return mFrames[mFrames.size() - 1].mTime;
}

template<typename T, int N>
int FastTrack<T, N>::FrameIndex(float time, bool looping) {
	std::vector<Frame<N>>& frames = this->mFrames;

	unsigned int size = (unsigned int)frames.size();
	if (size <= 1) { return -1; }

	if (looping) {
		float startTime = frames[0].mTime;
		float endTime = frames[size - 1].mTime;
		float duration = endTime - startTime;
		time = fmodf(time - startTime, endTime - startTime);
		if (time < 0.0f) {
			time += endTime - startTime;
		}
		time = time + startTime;
	}
	else {
		if (time <= frames[0].mTime) {
			return 0;
		}
		if (time >= frames[size - 2].mTime) {
			return (int)size - 2;
		}
	}
	float duration = this->GetEndTime() - this->GetStartTime();
	unsigned int numSamples = 60 + (unsigned int)(duration * 60.0f);
	float t = time / duration;

	unsigned int index = (unsigned int)(t * (float)numSamples);
	if (index >= mSampledFrames.size()) {
		return -1;
	}
	return (int)mSampledFrames[index];
}

template<typename T, int N>
void FastTrack<T, N>::UpdateIndexLookupTable() {
	int numFrames = (int)this->mFrames.size();
	if (numFrames <= 1) {
		return;
	}

	float duration = this->GetEndTime() - this->GetStartTime();
	unsigned int numSamples = 60 + (unsigned int)(duration * 60.0f);
	mSampledFrames.resize(numSamples);
	for (unsigned int i = 0; i < numSamples; ++i) {
		float t = (float)i / (float)(numSamples - 1);
		float time = t * duration + this->GetStartTime();

		unsigned int frameIndex = 0;
		for (int j = numFrames - 1; j >= 0; --j) {
			if (time >= this->mFrames[j].mTime) {
				frameIndex = (unsigned int)j;
				if ((int)frameIndex >= numFrames - 2) {
					frameIndex = numFrames - 2;
				}
				break;
			}
		}
		mSampledFrames[i] = frameIndex;
	}
}


template FastTrack<float, 1> OptimizeTrack(Track<float, 1>& input);
template FastTrack<vec3, 3> OptimizeTrack(Track<vec3, 3>& input);
template FastTrack<quat, 4> OptimizeTrack(Track<quat, 4>& input);

template<typename T, int N>
FastTrack<T, N> OptimizeTrack(Track<T, N>& input) {
	FastTrack<T, N> result;

	result.SetInterpolation(input.GetInterpolation());
	unsigned int size = input.Size();
	result.Resize(size);
	for (unsigned int i = 0; i < size; ++i) {
		result[i] = input[i];
	}
	result.UpdateIndexLookupTable();

	return result;
}