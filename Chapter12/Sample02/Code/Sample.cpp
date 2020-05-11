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

	mFadeController.SetSkeleton(mSkeleton);
	mFadeController.Play(&mClips[0]);
	mFadeController.Update(0.0f);
	mFadeController.GetCurrentPose().GetMatrixPalette(mPosePalette);

	mFadeTimer = 3.0f;
	mCurrentClip = 0;
}

void Sample::Shutdown() {
	mClips.clear();
	mMeshes.clear();
	delete mShader;
	delete mTexture;
}

void Sample::Update(float dt) {
	mFadeController.Update(dt);

	mFadeTimer -= dt;
	if (mFadeTimer < 0.0f) {
		mFadeTimer = 3.0f;

		unsigned int clip = mCurrentClip;
		while (clip == mCurrentClip) {
			clip = rand() % mClips.size();
		}
		mCurrentClip = clip;

		mFadeController.FadeTo(&mClips[clip], 0.5f);
	}

	mFadeController.GetCurrentPose().GetMatrixPalette(mPosePalette);
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