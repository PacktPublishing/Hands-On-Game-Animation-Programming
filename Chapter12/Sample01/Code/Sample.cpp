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

	mPose = mSkeleton.GetRestPose();
	mPose.GetMatrixPalette(mPosePalette);
	mSkinPalette = mSkeleton.GetInvBindPose();

	mShader = new Shader("Shaders/skinned.vert", "Shaders/lit.frag");
	mTexture = new Texture("Assets/Woman.png");

	mA.mPose = mPose;
	mB.mPose = mPose;

	mBlendTime = 0.0f;
	mInvertBlend = false;

	mA.mClip = 0;
	mB.mClip = 1;
	for (unsigned int i = 0, size = (unsigned int)mClips.size(); i < size; ++i) {
		if (mClips[i].GetName() == "Walking") {
			mA.mClip = i;
			mA.mTime = mClips[i].GetStartTime();
		}
		else if (mClips[i].GetName() == "Running") {
			mB.mClip = i;
			mB.mTime = mClips[i].GetStartTime();
		}
	}
}

void Sample::Shutdown() {
	mClips.clear();
	mMeshes.clear();
	delete mShader;
	delete mTexture;
}

void Sample::Update(float dt) {
	mA.mTime = mClips[mA.mClip].Sample(mA.mPose, mA.mTime + dt);
	mB.mTime = mClips[mB.mClip].Sample(mB.mPose, mB.mTime + dt);

	float bt = mBlendTime;
	if (bt < 0.0f) { bt = 0.0f; }
	if (bt > 1.0f) { bt = 1.0f; }
	if (mInvertBlend) { bt = 1.0f - bt; }
	Blend(mPose, mA.mPose, mB.mPose, bt, -1);
	mPose.GetMatrixPalette(mPosePalette);

	mBlendTime += dt;
	if (mBlendTime >= 2.0f) {
		mBlendTime = 0.0f;
		mInvertBlend = !mInvertBlend;
		mPose = mSkeleton.GetRestPose();
	}
}

void Sample::Render(float aspect) {
	mat4 model;
	mat4 view = lookAt(vec3(0, 3, 5), vec3(0, 3, 0), vec3(0, 1, 0));
	mat4 projection = perspective(60.0f, aspect, 0.01f, 1000.0f);

	mShader->Bind();
	Uniform<mat4>::Set(mShader->GetUniform("model"), model);
	Uniform<mat4>::Set(mShader->GetUniform("view"), view);
	Uniform<mat4>::Set(mShader->GetUniform("projection"), projection);
	Uniform<mat4>::Set(mShader->GetUniform("pose"), mPosePalette);
	Uniform<mat4>::Set(mShader->GetUniform("invBindPose"), mSkinPalette);
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