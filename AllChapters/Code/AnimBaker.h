#ifndef _H_ANIMATIONBAKER_
#define _H_ANIMATIONBAKER_

#include "Clip.h"
#include "Skeleton.h"
#include "AnimTexture.h"

void BakeAnimationToTexture(Skeleton& skel, Clip& clip, AnimTexture& tex);

#endif