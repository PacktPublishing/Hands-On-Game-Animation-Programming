#define _CRT_SECURE_NO_WARNINGS
#include "Chapter09Sample01.h"
#include "GLTFLoader.h"

void Chapter09Sample01::Initialize() {
	cgltf_data* gltf = LoadGLTFFile("Assets/Woman.gltf");
	mRestPose = LoadRestPose(gltf);
	mClips = LoadAnimationClips(gltf);
	FreeGLTFFile(gltf);

	mRestPoseVisual = new DebugDraw();
	mRestPoseVisual->FromPose(mRestPose);
	mRestPoseVisual->UpdateOpenGLBuffers();

	mCurrentClip = 0;
	mCurrentPose = mRestPose;

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
		memset(mUIClipNames[i], 0, nameLength + 1);
		strcpy(mUIClipNames[i], clipName.c_str());
	}
	mShowRestPose = true;
	mShowCurrentPose = true;
}

void Chapter09Sample01::Update(float deltaTime) {
	mPlaybackTime = mClips[mCurrentClip].Sample(mCurrentPose, mPlaybackTime + deltaTime);
	mCurrentPoseVisual->FromPose(mCurrentPose);
}

void Chapter09Sample01::Render(float inAspectRatio) {
	mat4 projection = perspective(60.0f, inAspectRatio, 0.01f, 1000.0f);
	mat4 view = lookAt(vec3(0, 4, -7), vec3(0, 4, 0), vec3(0, 1, 0));
	mat4 mvp = projection * view; // No model

	if (mShowRestPose) {
		mRestPoseVisual->Draw(DebugDrawMode::Lines, vec3(1, 0, 0), mvp);
	}

	if (mShowCurrentPose) {
		mCurrentPoseVisual->UpdateOpenGLBuffers();
		mCurrentPoseVisual->Draw(DebugDrawMode::Lines, vec3(0, 0, 1), mvp);
	}
}

void Chapter09Sample01::Shutdown() {
	delete mRestPoseVisual;
	delete mCurrentPoseVisual;
	mClips.clear();

	for (unsigned int i = 0; i < mNumUIClips; ++i) {
		delete[] mUIClipNames[i];
	}
	delete[] mUIClipNames;
	mNumUIClips = 0;
}

void Chapter09Sample01::ImGui(nk_context* ctx) {
	nk_begin(ctx, "Chapter 9, Sample 1", nk_rect(5.0f, 5.0f, 300.0f, 115.0f), NK_WINDOW_BORDER | NK_WINDOW_NO_SCROLLBAR);

	static const float layout[] = { 75, 200 };
	nk_layout_row(ctx, NK_STATIC, 25, 2, layout);

	nk_label(ctx, "Animation:", NK_TEXT_LEFT);
	int selected = nk_combo(ctx, (const char**)mUIClipNames, mNumUIClips, (int)mCurrentClip, 25, nk_vec2(200, 200));
	if ((unsigned int)selected != mCurrentClip) {
		mCurrentPose = mRestPose;
		mCurrentClip = (unsigned int)selected;
	}

	nk_label(ctx, "Playback:", NK_TEXT_LEFT);

	float startTime = mClips[mCurrentClip].GetStartTime();
	float duration = mClips[mCurrentClip].GetDuration();
	float progress = (mPlaybackTime - startTime) / duration;
	size_t prog = (size_t)(progress * 200.0f);
	nk_progress(ctx, &prog, 200, NK_FIXED);

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
