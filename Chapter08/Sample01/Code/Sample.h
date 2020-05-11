#ifndef _H_Sample_
#define _H_Sample_

#include "Application.h"
#include "DebugDraw.h"
#include "Track.h"
#include <vector>

class Sample : public Application {
protected:
	std::vector<ScalarTrack> mScalarTracks;
	std::vector<bool> mScalarTracksLooping;

	DebugDraw* mScalarTrackLines;
	DebugDraw* mHandleLines;
	DebugDraw* mHandlePoints;
	DebugDraw* mReferenceLines;
private:
	ScalarFrame MakeFrame(float time, float value);
	ScalarFrame MakeFrame(float time, float in, float value, float out);
	VectorFrame MakeFrame(float time, const vec3& value);
	VectorFrame MakeFrame(float time, const vec3& in, const vec3& value, const vec3& out);
	QuaternionFrame MakeFrame(float time, const quat& value);
	QuaternionFrame MakeFrame(float time, const quat& in, const quat& out, const quat& value);
	ScalarTrack MakeScalarTrack(Interpolation interp, int numFrames, ...);
	VectorTrack MakeVectorTrack(Interpolation interp, int numFrames, ...);
	QuaternionTrack MakeQuaternionTrack(Interpolation interp, int numFrames, ...);
public:
	void Initialize();
	void Render(float inAspectRatio);
	void Shutdown();
};

#endif