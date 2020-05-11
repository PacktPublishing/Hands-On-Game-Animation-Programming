#ifndef _H_GLTFLOADER_
#define _H_GLTFLOADER_

#include "cgltf.h"
#include "Pose.h"
#include "Clip.h"
#include <vector>
#include <string>

cgltf_data* LoadGLTFFile(const char* path);
void FreeGLTFFile(cgltf_data* handle);

Pose LoadRestPose(cgltf_data* data);
std::vector<std::string> LoadJointNames(cgltf_data* data);
std::vector<Clip> LoadAnimationClips(cgltf_data* data);

#endif
