#include "Chapter13Sample01.h"

#define GIZMO_SIZE 0.25f

void Chapter13Sample01::Initialize() {
	mSolver.Resize(3);
	mSolver[0].rotation = angleAxis(90.0f * QUAT_DEG2RAD, vec3(1, 0, 0));
	mSolver[1].position = vec3(0, 0, 1.0f);
	mSolver[2].position = vec3(0, 0, 1.5f);;

	mTarget.position = vec3(1, -2, 0);

	mCCDLines = new DebugDraw();
	mCCDPoints = new DebugDraw();
	mTargetVisual[0] = new DebugDraw(2);
	mTargetVisual[1] = new DebugDraw(2);
	mTargetVisual[2] = new DebugDraw(2);

	mCamPitch = 45.0f;
	mCamYaw = 60.0f;
	mCamDist = 7.0f;
}

void Chapter13Sample01::Update(float deltaTime) {
	mSolver.Solve(mTarget);
}

void Chapter13Sample01::Render(float inAspectRatio) {
	vec3 cameraPos(
		mCamDist * cosf(mCamYaw * QUAT_DEG2RAD) * sinf(mCamPitch * QUAT_DEG2RAD),
		mCamDist * cosf(mCamPitch * QUAT_DEG2RAD),
		mCamDist * sinf(mCamYaw * QUAT_DEG2RAD) * sinf(mCamPitch * QUAT_DEG2RAD)
	);

	mat4 projection = perspective(60.0f, inAspectRatio, 0.01f, 1000.0f);
	mat4 view = lookAt(cameraPos, vec3(0, 0, 0), vec3(0, 1, 0));
	mat4 mvp = projection * view; // No model

	mCCDLines->LinesFromIKSolver(mSolver);
	mCCDPoints->PointsFromIKSolver(mSolver);
	mCCDLines->UpdateOpenGLBuffers();
	mCCDPoints->UpdateOpenGLBuffers();
	mCCDLines->Draw(DebugDrawMode::Lines, vec3(1, 0, 1), mvp);
	mCCDPoints->Draw(DebugDrawMode::Points, vec3(1, 0, 1), mvp);

	(*mTargetVisual[0])[0] = mTarget.position + vec3(GIZMO_SIZE, 0, 0);
	(*mTargetVisual[1])[0] = mTarget.position + vec3(0, GIZMO_SIZE, 0);
	(*mTargetVisual[2])[0] = mTarget.position + vec3(0, 0, GIZMO_SIZE);
	(*mTargetVisual[0])[1] = mTarget.position - vec3(GIZMO_SIZE, 0, 0);
	(*mTargetVisual[1])[1] = mTarget.position - vec3(0, GIZMO_SIZE, 0);
	(*mTargetVisual[2])[1] = mTarget.position - vec3(0, 0, GIZMO_SIZE);
	mTargetVisual[0]->UpdateOpenGLBuffers();
	mTargetVisual[1]->UpdateOpenGLBuffers();
	mTargetVisual[2]->UpdateOpenGLBuffers();
	mTargetVisual[0]->Draw(DebugDrawMode::Lines, vec3(1, 0, 0), mvp);
	mTargetVisual[1]->Draw(DebugDrawMode::Lines, vec3(0, 1, 0), mvp);
	mTargetVisual[2]->Draw(DebugDrawMode::Lines, vec3(0, 0, 1), mvp);
}

void Chapter13Sample01::Shutdown() {
	delete mCCDLines;
	delete mCCDPoints;
	delete mTargetVisual[0];
	delete mTargetVisual[1];
	delete mTargetVisual[2];
}

void Chapter13Sample01::ImGui(nk_context* ctx) {
	nk_begin(ctx, "Chapter 9, Sample 1", nk_rect(5.0f, 5.0f, 300.0f, 180.0f), NK_WINDOW_BORDER | NK_WINDOW_NO_SCROLLBAR);

	static const float layout[] = { 75, 200 };
	nk_layout_row(ctx, NK_STATIC, 25, 2, layout);

	nk_label(ctx, "Target X:", NK_TEXT_LEFT);
	nk_slider_float(ctx, -5.0f, &mTarget.position.x, 5.0f, 0.001f);
	nk_label(ctx, "Target Y:", NK_TEXT_LEFT);
	nk_slider_float(ctx, -5.0f, &mTarget.position.y, 5.0f, 0.001f);
	nk_label(ctx, "Target Z:", NK_TEXT_LEFT);
	nk_slider_float(ctx, -5.0f, &mTarget.position.z, 5.0f, 0.001f);

	nk_label(ctx, "Pitch:", NK_TEXT_LEFT);
	nk_slider_float(ctx, 10.0f, &mCamPitch, 170.0f, 0.001f);
	nk_label(ctx, "Yaw:", NK_TEXT_LEFT);
	nk_slider_float(ctx, 0.0f, &mCamYaw, 360.0f, 0.001f);
	nk_label(ctx, "Distance:", NK_TEXT_LEFT);
	nk_slider_float(ctx, 1.0f, &mCamDist, 15.0f, 0.001f);

	nk_end(ctx);
}