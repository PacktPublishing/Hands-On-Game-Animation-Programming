#include "Sample.h"
#include "GLTFLoader.h"
#include "Blending.h"
#include "Uniform.h"

void Sample::Initialize() {
	cgltf_data* gltf = LoadGLTFFile("Assets/Woman.gltf");
	mSkeleton = LoadSkeleton(gltf);
	mClips = LoadAnimationClips(gltf);
	mMeshes = LoadMeshes(gltf);
	FreeGLTFFile(gltf);

	mShader = new Shader("Shaders/skinned.vert", "Shaders/lit.frag");
	mTexture = new Texture("Assets/Woman.png");

	mAdditiveTime = 0.0f;
	mAdditiveDirection = 1.0f;

	mClip = 0;
	mAdditiveIndex = 0;
	for (unsigned int i = 0, size = (unsigned int)mClips.size(); i < size; ++i) {
		if (mClips[i].GetName() == "Lean_Left") {
			mAdditiveIndex = i;
		}
		if (mClips[i].GetName() == "Walking") {
			mClip = i;
		}
	}

	mAdditiveBase = MakeAdditivePose(mSkeleton, mClips[mAdditiveIndex]);
	mClips[mAdditiveIndex].SetLooping(false);

	mCurrentPose = mSkeleton.GetRestPose(); 
	mAddPose = mSkeleton.GetRestPose();
	mPlaybackTime = 0.0f;
}

void Sample::Shutdown() {
	mClips.clear();
	mMeshes.clear();
	delete mShader;
	delete mTexture;
}

void Sample::Update(float dt) {
	mAdditiveTime += dt * mAdditiveDirection;

	if (mAdditiveTime < 0.0f) {
		mAdditiveTime = 0.0f;
		mAdditiveDirection *= -1.0f;
	}

	if (mAdditiveTime > 1.0f) {
		mAdditiveTime = 1.0f;
		mAdditiveDirection *= -1.0f;
	}

	mPlaybackTime = mClips[mClip].Sample(mCurrentPose, mPlaybackTime + dt);
	float time = mClips[mAdditiveIndex].GetStartTime() + (mClips[mAdditiveIndex].GetDuration() * mAdditiveTime);
	mClips[mAdditiveIndex].Sample(mAddPose, time);
	Add(mCurrentPose, mCurrentPose, mAddPose, mAdditiveBase, -1);

	mCurrentPose.GetMatrixPalette(mPosePalette);
}

void Sample::Render(float aspect) {
	mat4 model;
	mat4 view = lookAt(vec3(0, 3, 7), vec3(0, 3, 0), vec3(0, 1, 0));
	mat4 projection = perspective(60.0f, aspect, 0.01f, 1000.0f);

	mShader->Bind();
	Uniform<mat4>::Set(mShader->GetUniform("model"), model);
	Uniform<mat4>::Set(mShader->GetUniform("view"), view);
	Uniform<mat4>::Set(mShader->GetUniform("projection"), projection);
	Uniform<mat4>::Set(mShader->GetUniform("pose"), mPosePalette);
	Uniform<mat4>::Set(mShader->GetUniform("invBindPose"), mSkeleton.GetInvBindPose());
	Uniform<vec3>::Set(mShader->GetUniform("light"), vec3(1, 1, 1));
	mTexture->Set(mShader->GetUniform("tex0"), 0);
	for (unsigned int i = 0, size = (unsigned int)mMeshes.size(); i < size; ++i) {
		mMeshes[i].Bind(mShader->GetAttribute("position"), mShader->GetAttribute("normal"), mShader->GetAttribute("texCoord"), mShader->GetAttribute("weights"), mShader->GetAttribute("joints"));
		mMeshes[i].Draw();
		mMeshes[i].UnBind(mShader->GetAttribute("position"), mShader->GetAttribute("normal"), mShader->GetAttribute("texCoord"), mShader->GetAttribute("weights"), mShader->GetAttribute("joints"));
	}
	mTexture->UnSet(0);

	mShader->UnBind();
}