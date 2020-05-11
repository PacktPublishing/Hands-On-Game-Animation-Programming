#pragma warning(disable : 26451)
#define _CRT_SECURE_NO_WARNINGS
#include "Sample.h"
#include "GLTFLoader.h"
#include "Uniform.h"
#include "glad.h"
#include "Mesh.h"

void Sample::Initialize() {
	cgltf_data* gltf = LoadGLTFFile("Assets/Woman.gltf");
	mSkeleton = LoadSkeleton(gltf);
	mCPUMeshes = LoadMeshes(gltf);
	mGPUMeshes = LoadMeshes(gltf);
	mClips = LoadAnimationClips(gltf);
	FreeGLTFFile(gltf);

	mStaticShader = new Shader("Shaders/static.vert", "Shaders/lit.frag");
	mSkinnedShader = new Shader("Shaders/preskinned.vert", "Shaders/lit.frag");
	mDiffuseTexture = new Texture("Assets/Woman.png");

	unsigned int numUIClips = (unsigned int)mClips.size();
	for (unsigned int i = 0; i < numUIClips; ++i) {
		if (mClips[i].GetName() == "Running") {
			mGPUInstance.mClip = i;
		}
		else if (mClips[i].GetName() == "Walking") {
			mCPUInstance.mClip = i;
		}
	}

	mCPUInstance.mModel.position = vec3(-2, 0, 0);
	mGPUInstance.mModel.position = vec3(2, 0, 0);
	mCPUInstance.mAnimatedPose = mSkeleton.GetRestPose();
	mGPUInstance.mAnimatedPose = mSkeleton.GetRestPose();
	mCPUInstance.mPosePalette.resize(mCPUInstance.mAnimatedPose.Size());
}

void Sample::Update(float deltaTime) {
	mCPUInstance.mPlayback = mClips[mCPUInstance.mClip].Sample(mCPUInstance.mAnimatedPose, mCPUInstance.mPlayback + deltaTime);
	mGPUInstance.mPlayback = mClips[mGPUInstance.mClip].Sample(mGPUInstance.mAnimatedPose, mGPUInstance.mPlayback + deltaTime);

	mCPUInstance.mAnimatedPose.GetMatrixPalette(mCPUInstance.mPosePalette);
	mGPUInstance.mAnimatedPose.GetMatrixPalette(mGPUInstance.mPosePalette);

	for (unsigned int i = 0, size = (unsigned int)mCPUInstance.mPosePalette.size(); i < size; ++i) {
		mCPUInstance.mPosePalette[i] = mCPUInstance.mPosePalette[i] * mSkeleton.GetInvBindPose()[i];
		mGPUInstance.mPosePalette[i] = mGPUInstance.mPosePalette[i] * mSkeleton.GetInvBindPose()[i];
	}

	for (unsigned int i = 0, size = (unsigned int)mCPUMeshes.size(); i < size; ++i) {
		mCPUMeshes[i].CPUSkin(mCPUInstance.mPosePalette);
	}
}

void Sample::Render(float inAspectRatio) {
	mat4 projection = perspective(60.0f, inAspectRatio, 0.01f, 1000.0f);
	mat4 view = lookAt(vec3(0, 5, 7), vec3(0, 3, 0), vec3(0, 1, 0));

	// Draw CPU Skinned Model
	mat4 model = transformToMat4(mCPUInstance.mModel);
	mStaticShader->Bind();
	Uniform<mat4>::Set(mStaticShader->GetUniform("model"), model);
	Uniform<mat4>::Set(mStaticShader->GetUniform("view"), view);
	Uniform<mat4>::Set(mStaticShader->GetUniform("projection"), projection);
	Uniform<vec3>::Set(mStaticShader->GetUniform("light"), vec3(1, 1, 1));
	mDiffuseTexture->Set(mStaticShader->GetUniform("tex0"), 0);
	for (unsigned int i = 0, size = (unsigned int)mCPUMeshes.size(); i < size; ++i) {
		mCPUMeshes[i].Bind(mStaticShader->GetAttribute("position"), mStaticShader->GetAttribute("normal"), mStaticShader->GetAttribute("texCoord"), -1, -1);
		mCPUMeshes[i].Draw();
		mCPUMeshes[i].UnBind(mStaticShader->GetAttribute("position"), mStaticShader->GetAttribute("normal"), mStaticShader->GetAttribute("texCoord"), -1, -1);
	}
	mDiffuseTexture->UnSet(0);
	mStaticShader->UnBind();

	// Draw GPU Skinned Model
	model = transformToMat4(mGPUInstance.mModel);
	mSkinnedShader->Bind();
	Uniform<mat4>::Set(mSkinnedShader->GetUniform("model"), model);
	Uniform<mat4>::Set(mSkinnedShader->GetUniform("view"), view);
	Uniform<mat4>::Set(mSkinnedShader->GetUniform("projection"), projection);
	Uniform<vec3>::Set(mSkinnedShader->GetUniform("light"), vec3(1, 1, 1));

	Uniform<mat4>::Set(mSkinnedShader->GetUniform("animated"), mGPUInstance.mPosePalette);

	mDiffuseTexture->Set(mSkinnedShader->GetUniform("tex0"), 0);
	for (unsigned int i = 0, size = (unsigned int)mGPUMeshes.size(); i < size; ++i) {
		mGPUMeshes[i].Bind(mSkinnedShader->GetAttribute("position"), mSkinnedShader->GetAttribute("normal"), mSkinnedShader->GetAttribute("texCoord"), mSkinnedShader->GetAttribute("weights"), mSkinnedShader->GetAttribute("joints"));
		mGPUMeshes[i].Draw();
		mGPUMeshes[i].UnBind(mSkinnedShader->GetAttribute("position"), mSkinnedShader->GetAttribute("normal"), mSkinnedShader->GetAttribute("texCoord"), mSkinnedShader->GetAttribute("weights"), mSkinnedShader->GetAttribute("joints"));
	}
	mDiffuseTexture->UnSet(0);
	mSkinnedShader->UnBind();
}

void Sample::Shutdown() {
	delete mStaticShader;
	delete mDiffuseTexture;
	delete mSkinnedShader;
	mClips.clear();
	mCPUMeshes.clear();
	mGPUMeshes.clear();
}