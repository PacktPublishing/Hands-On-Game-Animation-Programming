#include "AnimBaker.h"

void BakeAnimationToTexture(Skeleton& skel, Clip& clip, AnimTexture& tex) {
	Pose& bindPose = skel.GetBindPose();
	Pose samplePose = bindPose;
	unsigned int texWidth = tex.Size();
	for (unsigned int x = 0; x < texWidth; ++x) {
		float t = (float)x / (float)(texWidth - 1);
		float time = clip.GetStartTime() + clip.GetDuration() * t;
		clip.Sample(samplePose, time);

		for (unsigned int y = 0; y < bindPose.Size() * 3; y += 3) {
			Transform transform = samplePose.GetGlobalTransform(y / 3);
			tex.SetTexel(x, y + 0, transform.position);
			tex.SetTexel(x, y + 1, transform.rotation);
			tex.SetTexel(x, y + 2, transform.scale);
		}
	}
	tex.UploadTextureDataToGPU();
}