#include "Sample.h"

#define GIZMO_SIZE 0.25f
#define QUAT_DEG2RAD 0.0174533f

void Sample::Initialize() {
	mSolver.Resize(6);
	mSolver.SetLocalTransform(0, Transform(vec3(), angleAxis(90.0f * QUAT_DEG2RAD, vec3(1, 0, 0)), vec3(1, 1, 1)));
	mSolver.SetLocalTransform(1, Transform(vec3(0, 0, 1.0f), quat(), vec3(1, 1, 1)));
	mSolver.SetLocalTransform(2, Transform(vec3(0, 0, 1.5f), quat(), vec3(1, 1, 1)));
	mSolver.SetLocalTransform(3, Transform(vec3(0, 0, 0.5f), angleAxis(90.0f * QUAT_DEG2RAD, vec3(0, 1, 0)), vec3(1, 1, 1)));
	mSolver.SetLocalTransform(4, Transform(vec3(0, 0, 0.5f), quat(), vec3(1, 1, 1)));
	mSolver.SetLocalTransform(5, Transform(vec3(0, 0, 0.5f), quat(), vec3(1, 1, 1)));

	mTarget.position = vec3(1, -2, 0);

	VectorTrack& path = mTargetPath.GetPositionTrack();
	path.Resize(14);
	SetFrame(path, 0, 0.0f, vec3(1, -2, 0) * 0.5f);
	SetFrame(path, 1, 1.0f, vec3(1, 2, 0) * 0.5f);
	SetFrame(path, 2, 2.0f, vec3(1, 4, 0) * 0.5f);
	SetFrame(path, 3, 3.0f, vec3(3, 4, 0) * 0.5f);
	SetFrame(path, 4, 4.0f, vec3(5, 4, 0) * 0.5f);
	SetFrame(path, 5, 5.0f, vec3(5, 4, 2) * 0.5f);
	SetFrame(path, 6, 6.0f, vec3(5, 4, 4) * 0.5f);
	SetFrame(path, 7, 7.0f, vec3(3, 4, 4) * 0.5f);
	SetFrame(path, 8, 8.0f, vec3(3, 2, 4) * 0.5f);
	SetFrame(path, 9, 9.0f, vec3(3, 2, 2) * 0.5f);
	SetFrame(path, 10, 10.0f, vec3(1, 2, 2) * 0.5f);
	SetFrame(path, 11, 11.0f, vec3(1, 0, 2) * 0.5f);
	SetFrame(path, 12, 12.0f, vec3(1, -2, 2) * 0.5f);
	SetFrame(path, 13, 13.0f, vec3(1, -2, 0) * 0.5f);

	mSolverLines = new DebugDraw();
	mSolverPoints = new DebugDraw();
	mTargetVisual[0] = new DebugDraw(2);
	mTargetVisual[1] = new DebugDraw(2);
	mTargetVisual[2] = new DebugDraw(2);

	mCamPitch = 45.0f;
	mCamYaw = 60.0f;
	mCamDist = 7.0f;
	mPlayTime = 0.0f;
}

void Sample::SetFrame(VectorTrack& track, int index, float time, const vec3& value) {
	track[index].mValue[0] = value.x;
	track[index].mValue[1] = value.y;
	track[index].mValue[2] = value.z;
	track[index].mTime = time;
}

void Sample::Update(float deltaTime) {
	mPlayTime += deltaTime;
	if (mPlayTime > mTargetPath.GetEndTime()) {
		mPlayTime -= mTargetPath.GetEndTime();
	}
	mTarget = mTargetPath.Sample(mTarget, mPlayTime, true);
	mSolver.Solve(mTarget);
}

void Sample::Render(float inAspectRatio) {
	vec3 cameraPos(
		mCamDist * cosf(mCamYaw * QUAT_DEG2RAD) * sinf(mCamPitch * QUAT_DEG2RAD),
		mCamDist * cosf(mCamPitch * QUAT_DEG2RAD),
		mCamDist * sinf(mCamYaw * QUAT_DEG2RAD) * sinf(mCamPitch * QUAT_DEG2RAD)
	);

	mat4 projection = perspective(60.0f, inAspectRatio, 0.01f, 1000.0f);
	mat4 view = lookAt(cameraPos, vec3(0, 0, 0), vec3(0, 1, 0));
	mat4 mvp = projection * view; // No model

	mSolverLines->LinesFromIKSolver(mSolver);
	mSolverPoints->PointsFromIKSolver(mSolver);
	mSolverLines->UpdateOpenGLBuffers();
	mSolverPoints->UpdateOpenGLBuffers();
	mSolverLines->Draw(DebugDrawMode::Lines, vec3(1, 0, 1), mvp);
	mSolverPoints->Draw(DebugDrawMode::Points, vec3(1, 0, 1), mvp);

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

void Sample::Shutdown() {
	delete mSolverLines;
	delete mSolverPoints;
	delete mTargetVisual[0];
	delete mTargetVisual[1];
	delete mTargetVisual[2];
}