#ifndef _H_SAMPLE_POSE_INSTANCE_
#define _H_SAMPLE_POSE_INSTANCE_

struct SamplePoseInstance {
	Pose mPose;
	unsigned int mClip;
	float mPlayback;
	DebugDraw* mVisual;
	bool mShowPose;

	inline SamplePoseInstance() : mClip(0), mPlayback(0.0f), mVisual(0), mShowPose(false){ }
};

#endif