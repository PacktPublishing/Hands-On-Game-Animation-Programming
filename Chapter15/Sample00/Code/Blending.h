#ifndef _H_BLENDING_
#define _H_BLENDING_

#include "Pose.h"
#include "Clip.h"
#include "Skeleton.h"

bool IsInHierarchy(Pose& pose, unsigned int parent, unsigned int search);
void Blend(Pose& output, Pose& a, Pose& b, float t, int blendroot);

Pose MakeAdditivePose(Skeleton& skeleton, Clip& clip);
void Add(Pose& output, Pose& inPose, Pose& addPose, Pose& additiveBasePose, int blendroot);


#endif