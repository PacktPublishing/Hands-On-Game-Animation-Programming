#pragma warning(disable : 26451)
#define _CRT_SECURE_NO_WARNINGS
#include "Chapter11Sample01.h"
#include "GLTFLoader.h"
#include "Uniform.h"
#include "glad.h"
#include "Mesh.h"

void Chapter11Sample01::Initialize() {
	cgltf_data* gltf = LoadGLTFFile("Assets/Woman.gltf");
	mSkeleton = LoadSkeleton(gltf);

	mMeshes = LoadMeshes(gltf);
	mClips = LoadAnimationClips(gltf);
	FreeGLTFFile(gltf);

	mStaticShader = new Shader("Shaders/static.vert", "Shaders/lit.frag");
	mSkinnedShader = new Shader("Shaders/preskinned.vert", "Shaders/lit.frag");
	mDiffuseTexture = new Texture("Assets/Woman.png");

	mRestPoseVisual = new DebugDraw();
	mRestPoseVisual->FromPose(mSkeleton.GetRestPose());
	mRestPoseVisual->UpdateOpenGLBuffers();

	mCurrentClip = 0;
	mCurrentPose = mSkeleton.GetRestPose();

	mCurrentPoseVisual = new DebugDraw();
	mCurrentPoseVisual->FromPose(mCurrentPose);
	mCurrentPoseVisual->UpdateOpenGLBuffers();

	// For the UI
	mNumUIClips = (unsigned int)mClips.size();
	mUIClipNames = new char* [mNumUIClips];
	for (unsigned int i = 0; i < mNumUIClips; ++i) {
		std::string& clipName = mClips[i].GetName();
		unsigned int nameLength = (unsigned int)clipName.length();
		mUIClipNames[i] = new char[nameLength + 1];
		memset(mUIClipNames[i], 0, sizeof(char) * (nameLength + 1));
		strcpy(mUIClipNames[i], clipName.c_str());
	}
	mSkinningNames = new char* [3];
	mSkinningNames[0] = new char[5];
	mSkinningNames[1] = new char[4];
	mSkinningNames[2] = new char[4];
	memset(mSkinningNames[0], 0, sizeof(char) * 5);
	memset(mSkinningNames[1], 0, sizeof(char) * 4);
	memset(mSkinningNames[2], 0, sizeof(char) * 4);
	sprintf(mSkinningNames[0], "None");
	sprintf(mSkinningNames[1], "CPU");
	sprintf(mSkinningNames[2], "GPU");
	mShowRestPose = false;
	mShowCurrentPose = false;
	mSkinType = SkinningType::GPU;
}

void Chapter11Sample01::Update(float deltaTime) {
	mPlaybackTime = mClips[mCurrentClip].Sample(mCurrentPose, mPlaybackTime + deltaTime);
	mCurrentPoseVisual->FromPose(mCurrentPose);

	std::vector<mat4>& invBindPose = mSkeleton.GetInvBindPose();
	mCurrentPose.GetMatrixPalette(mPosePalette);
	for (unsigned int i = 0, size = (unsigned int)mPosePalette.size(); i < size; ++i) {
		mPosePalette[i] = mPosePalette[i] * invBindPose[i];
	}

	if (mSkinType == SkinningType::CPU) {
		for (unsigned int i = 0, size = (unsigned int)mMeshes.size(); i < size; ++i) {
			mMeshes[i].CPUSkin(mPosePalette);
		}
	}
}

void Chapter11Sample01::Render(float inAspectRatio) {
	mat4 projection = perspective(60.0f, inAspectRatio, 0.01f, 1000.0f);
	mat4 view = lookAt(vec3(0, 5, 7), vec3(0, 3, 0), vec3(0, 1, 0));
	mat4 mvp = projection * view; // No model

	Shader* shader = mStaticShader;
	if (mSkinType == SkinningType::GPU) {
		shader = mSkinnedShader;
	}

	shader->Bind();
	Uniform<mat4>::Set(shader->GetUniform("model"), mat4());
	Uniform<mat4>::Set(shader->GetUniform("view"), view);
	Uniform<mat4>::Set(shader->GetUniform("projection"), projection);
	Uniform<vec3>::Set(shader->GetUniform("light"), vec3(1, 1, 1));

	if (mSkinType == SkinningType::GPU) {
		Uniform<mat4>::Set(shader->GetUniform("pose"), mPosePalette);
	}

	mDiffuseTexture->Set(shader->GetUniform("tex0"), 0);
	for (unsigned int i = 0, size = (unsigned int)mMeshes.size(); i < size; ++i) {
		if (mSkinType == SkinningType::GPU) {
			mMeshes[i].Bind(shader->GetAttribute("position"), shader->GetAttribute("normal"), shader->GetAttribute("texCoord"), shader->GetAttribute("weights"), shader->GetAttribute("joints"));
			mMeshes[i].Draw();
			mMeshes[i].UnBind(shader->GetAttribute("position"), shader->GetAttribute("normal"), shader->GetAttribute("texCoord"), shader->GetAttribute("weights"), shader->GetAttribute("joints"));
		}
		else {
			mMeshes[i].Bind(shader->GetAttribute("position"), shader->GetAttribute("normal"), shader->GetAttribute("texCoord"), -1, -1);
			mMeshes[i].Draw();
			mMeshes[i].UnBind(shader->GetAttribute("position"), shader->GetAttribute("normal"), shader->GetAttribute("texCoord"), -1, -1);
		}

		
	}
	mDiffuseTexture->UnSet(0);
	shader->UnBind();

	glDisable(GL_DEPTH_TEST);
	if (mShowRestPose) {
		mRestPoseVisual->Draw(DebugDrawMode::Lines, vec3(1, 0, 0), mvp);
	}

	if (mShowCurrentPose) {
		mCurrentPoseVisual->UpdateOpenGLBuffers();
		mCurrentPoseVisual->Draw(DebugDrawMode::Lines, vec3(0, 0, 1), mvp);
	}
	
	glEnable(GL_DEPTH_TEST);
}

void Chapter11Sample01::Shutdown() {
	delete mRestPoseVisual;
	delete mCurrentPoseVisual;
	delete mStaticShader;
	delete mDiffuseTexture;
	delete mSkinnedShader;
	mClips.clear();
	mMeshes.clear();

	for (unsigned int i = 0; i < mNumUIClips; ++i) {
		delete[] mUIClipNames[i];
	}
	delete[] mUIClipNames;
	mNumUIClips = 0;
}

void Chapter11Sample01::ImGui(nk_context* ctx) {
	nk_begin(ctx, "Chapter 9, Sample 1", nk_rect(5.0f, 5.0f, 300.0f, 140.0f), NK_WINDOW_BORDER | NK_WINDOW_NO_SCROLLBAR);

	static const float layout[] = { 75, 200 };
	nk_layout_row(ctx, NK_STATIC, 25, 2, layout);

	nk_label(ctx, "Animation:", NK_TEXT_LEFT);
	int selected = nk_combo(ctx, (const char**)mUIClipNames, mNumUIClips, (int)mCurrentClip, 25, nk_vec2(200, 200));
	if ((unsigned int)selected != mCurrentClip) {
		mCurrentPose = mSkeleton.GetRestPose();
		mCurrentClip = (unsigned int)selected;
	}

	nk_label(ctx, "Playback:", NK_TEXT_LEFT);

	float startTime = mClips[mCurrentClip].GetStartTime();
	float duration = mClips[mCurrentClip].GetDuration();
	float progress = (mPlaybackTime - startTime) / duration;
	size_t prog = (size_t)(progress * 200.0f);
	nk_progress(ctx, &prog, 200, NK_FIXED);

	nk_label(ctx, "Skinning:", NK_TEXT_LEFT);
	int skinIndex = 0;
	if (mSkinType == SkinningType::CPU) {
		skinIndex = 1;
	}
	else if (mSkinType == SkinningType::GPU) {
		skinIndex = 2;
	}
	selected = nk_combo(ctx, (const char**)mSkinningNames, 3, (int)skinIndex, 25, nk_vec2(200, 200));
	if (skinIndex != selected) {
		// Need to reset so positions is in the buffers
		for (unsigned int i = 0, size = (unsigned int)mMeshes.size(); i < size; ++i) {
			mMeshes[i].UpdateOpenGLBuffers();
		}

		if (selected == 0) {
			mSkinType = SkinningType::None;
		}
		else if (selected == 1) {
			mSkinType = SkinningType::CPU;
		}
		else {
			mSkinType = SkinningType::GPU;
		}
	}

	nk_layout_row_dynamic(ctx, 20, 1);

	int show = (int)mShowRestPose;
	if (nk_checkbox_label(ctx, "Show Rest Pose", &show)) {
		mShowRestPose = (bool)show;
	}

	show = (int)mShowCurrentPose;
	if (nk_checkbox_label(ctx, "Show Current Pose", &show)) {
		mShowCurrentPose = (bool)show;
	}

	nk_end(ctx);
}
