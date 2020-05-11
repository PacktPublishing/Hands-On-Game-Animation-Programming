#define _CRT_SECURE_NO_WARNINGS
#include "Chapter14Sample01.h"
#include "GLTFLoader.h"
#include "Uniform.h"
#include "glad.h"
#include <iostream>

void Chapter14Sample01::Initialize() {
	cgltf_data* gltf = LoadGLTFFile("Assets/dq.gltf");
	mMeshes = LoadMeshes(gltf);
	mSkeleton = LoadSkeleton(gltf);
	mClips = LoadAnimationClips(gltf);
	FreeGLTFFile(gltf);

	mLBSShader = new Shader("Shaders/skinned.vert", "Shaders/lit.frag");
	mDQShader = new Shader("Shaders/dualquaternion.vert", "Shaders/lit.frag");
	mDiffuseTexture = new Texture("Assets/dq.png");

	mCurrentPose = mSkeleton.GetRestPose();
	
	mCurrentPose.GetDualQuaternionPalette(mDqPosePalette);
	mSkeleton.GetInvBindPose(mDqInvBindPalette);

	mCurrentPose.GetMatrixPalette(mLbPosePalette);
	mLbInvBindPalette = mSkeleton.GetInvBindPose();

	mCurrentClip = 0;
	mPlaybackTime = 0.0f;
	mSkinningMethod = 0;

	unsigned int dq_mem = (unsigned int)strlen("Dual Quaternion") + 1;
	unsigned int lb_mem = (unsigned int)strlen("Linear Blend") + 1;
	mSkinningNames = new char* [2];
	mSkinningNames[0] = new char[dq_mem];
	mSkinningNames[1] = new char[lb_mem];
	memset(mSkinningNames[0], 0, sizeof(char) * dq_mem);
	memset(mSkinningNames[1], 0, sizeof(char) * lb_mem);
	sprintf(mSkinningNames[0], "Dual Quaternion");
	sprintf(mSkinningNames[1], "Linear Blend");
}

void Chapter14Sample01::Update(float deltaTime) {
	mPlaybackTime = mClips[mCurrentClip].Sample(mCurrentPose, mPlaybackTime + deltaTime);
	mCurrentPose.GetDualQuaternionPalette(mDqPosePalette);
	mCurrentPose.GetMatrixPalette(mLbPosePalette);
}

void Chapter14Sample01::Render(float inAspectRatio) {
	mat4 projection = perspective(60.0f, inAspectRatio, 0.01f, 1000.0f);
	mat4 view = lookAt(vec3(0, 3, 14), vec3(0, 0, 0), vec3(0, 1, 0));
	mat4 mvp = projection * view; // No model

	Shader* shader = mDQShader;
	if (mSkinningMethod == 1) {
		shader = mLBSShader;
	}
	shader->Bind();
	Uniform<mat4>::Set(shader->GetUniform("model"), mat4());
	Uniform<mat4>::Set(shader->GetUniform("view"), view);
	Uniform<mat4>::Set(shader->GetUniform("projection"), projection);
	Uniform<vec3>::Set(shader->GetUniform("light"), vec3(1, 1, 1));
	if (mSkinningMethod == 0) {
		Uniform<DualQuaternion>::Set(shader->GetUniform("pose"), mDqPosePalette);
		Uniform<DualQuaternion>::Set(shader->GetUniform("invBindPose"), mDqInvBindPalette);
	}
	else {
		Uniform<mat4>::Set(shader->GetUniform("pose"), mLbPosePalette);
		Uniform<mat4>::Set(shader->GetUniform("invBindPose"), mLbInvBindPalette);
	}
	mDiffuseTexture->Set(shader->GetUniform("tex0"), 0);
	for (unsigned int i = 0, size = (unsigned int)mMeshes.size(); i < size; ++i) {
		mMeshes[i].Bind(shader->GetAttribute("position"), shader->GetAttribute("normal"), shader->GetAttribute("texCoord"), shader->GetAttribute("weights"), shader->GetAttribute("joints"));
		mMeshes[i].Draw();
		mMeshes[i].UnBind(shader->GetAttribute("position"), shader->GetAttribute("normal"), shader->GetAttribute("texCoord"), shader->GetAttribute("weights"), shader->GetAttribute("joints"));
	}
	mDiffuseTexture->UnSet(0);
	shader->UnBind();
}

void Chapter14Sample01::Shutdown() {
	delete mDiffuseTexture;
	delete mLBSShader;
	delete mDQShader;
	delete[] mSkinningNames[0];
	delete[] mSkinningNames[1];
	delete[] mSkinningNames;
	mMeshes.clear();
	mClips.clear();
}

void Chapter14Sample01::ImGui(nk_context* ctx) {
	nk_begin(ctx, "Chapter 9, Sample 1", nk_rect(5.0f, 5.0f, 300.0f, 40.0f), NK_WINDOW_BORDER | NK_WINDOW_NO_SCROLLBAR);

	static const float layout[] = { 90, 185 };
	nk_layout_row(ctx, NK_STATIC, 25, 2, layout);

	nk_label(ctx, "Blend Mode:", NK_TEXT_LEFT);
	mSkinningMethod = nk_combo(ctx, (const char**)mSkinningNames, 2, mSkinningMethod, 25, nk_vec2(200, 200));

	nk_end(ctx);
}
