#define _CRT_SECURE_NO_WARNINGS
#include "Sample.h"
#include "GLTFLoader.h"
#include "Uniform.h"
#include "glad.h"
#include "Blending.h"
#include <iostream>

void GetScalarValues(std::vector<float>& outScalars, unsigned int inComponentCount, const cgltf_accessor& inAccessor) {
	outScalars.resize(inAccessor.count * inComponentCount);
	for (cgltf_size i = 0; i < inAccessor.count; ++i) {
		cgltf_accessor_read_float(&inAccessor, i, &outScalars[i * inComponentCount], inComponentCount);
	}
}

void StaticMeshFromAttribute(Mesh& outMesh, cgltf_attribute& attribute) {
	cgltf_attribute_type attribType = attribute.type;
	cgltf_accessor& accessor = *attribute.data;

	unsigned int componentCount = 0;
	if (accessor.type == cgltf_type_vec2) {
		componentCount = 2;
	}
	else if (accessor.type == cgltf_type_vec3) {
		componentCount = 3;
	}
	else if (accessor.type == cgltf_type_vec4) {
		componentCount = 4;
	}
	else {
		std::cout << "Unknown data type\n";
		return;
	}

	std::vector<float> values;
	GetScalarValues(values, componentCount, accessor);
	unsigned int acessorCount = (unsigned int)accessor.count;

	std::vector<vec3>& positions = outMesh.GetPosition();
	std::vector<vec3>& normals = outMesh.GetNormal();
	std::vector<vec2>& texCoords = outMesh.GetTexCoord();

	for (unsigned int i = 0; i < acessorCount; ++i) {
		int index = i * componentCount;
		switch (attribType) {
		case cgltf_attribute_type_position:
			positions.push_back(vec3(values[index + 0], values[index + 1], values[index + 2]));
			break;
		case cgltf_attribute_type_normal:
		{
			vec3 normal = vec3(values[index + 0], values[index + 1], values[index + 2]);
			if (lenSq(normal) < 0.000001f) {
				normal = vec3(0, 1, 0);
			}
			normals.push_back(normalized(normal));
		}
		break;
		case cgltf_attribute_type_texcoord:
			texCoords.push_back(vec2(values[index + 0], values[index + 1]));
			break;
		}
	}
}

std::vector<Mesh> LoadStaticMeshes(cgltf_data* data) {
	std::vector<Mesh> result;
	cgltf_node* nodes = data->nodes;
	unsigned int nodeCount = (unsigned int)data->nodes_count;

	for (unsigned int i = 0; i < nodeCount; ++i) {
		cgltf_node* node = &nodes[i];
		if (node->mesh == 0) {
			continue;
		}

		unsigned int numPrims = (unsigned int)node->mesh->primitives_count;
		for (unsigned int j = 0; j < numPrims; ++j) {
			result.push_back(Mesh());
			Mesh& mesh = result[result.size() - 1];

			cgltf_primitive* primitive = &node->mesh->primitives[j];

			unsigned int numAttributes = (unsigned int)primitive->attributes_count;
			for (unsigned int k = 0; k < numAttributes; ++k) {
				cgltf_attribute* attribute = &primitive->attributes[k];
				StaticMeshFromAttribute(mesh, *attribute);
			}

			if (primitive->indices != 0) {
				unsigned int indexCount = (unsigned int)primitive->indices->count;
				std::vector<unsigned int>& indices = mesh.GetIndices();
				indices.resize(indexCount);

				for (unsigned int k = 0; k < indexCount; ++k) {
					indices[k] = (unsigned int)cgltf_accessor_read_index(primitive->indices, k);
				}
			}

			mesh.UpdateOpenGLBuffers();
		}
	}

	return result;
}

void Sample::Initialize() {
	mLastModelY = 0.0f;

	mMotionTrack.Resize(5);
	mMotionTrack.SetInterpolation(Interpolation::Linear);
	mMotionTrack[0].mTime = 0.0f;
	mMotionTrack[0].mValue[0] = 0; mMotionTrack[0].mValue[2] = 1;
	mMotionTrack[1].mTime = 1.0f;
	mMotionTrack[1].mValue[0] = 0; mMotionTrack[1].mValue[2] = 10;
	mMotionTrack[2].mTime = 3.0f;
	mMotionTrack[2].mValue[0] = 22; mMotionTrack[2].mValue[2] = 10;
	mMotionTrack[3].mTime = 4.0f;
	mMotionTrack[3].mValue[0] = 22; mMotionTrack[3].mValue[2] = 2;
	mMotionTrack[4].mTime = 6.0f;
	mMotionTrack[4].mValue[0] = 0; mMotionTrack[4].mValue[2] = 1;

	cgltf_data* gltf = LoadGLTFFile("Assets/Woman.gltf");
	mMeshes = LoadMeshes(gltf);
	mSkeleton = LoadSkeleton(gltf);
	mClips = LoadAnimationClips(gltf);
	FreeGLTFFile(gltf);

	gltf = LoadGLTFFile("Assets/IKCourse.gltf");
	mIKCourse = LoadStaticMeshes(gltf);
	FreeGLTFFile(gltf);
	mCourseTexture = new Texture("Assets/uv.png");
	mTriangles = MeshesToTriangles(mIKCourse);

	mStaticShader = new Shader("Shaders/static.vert", "Shaders/lit.frag");
	mSkinnedShader = new Shader("Shaders/skinned.vert", "Shaders/lit.frag");
	mDiffuseTexture = new Texture("Assets/Woman.png");

	mLeftLeg = new IKLeg(mSkeleton, "LeftUpLeg", "LeftLeg", "LeftFoot", "LeftToeBase");
	mLeftLeg->SetAnkleOffset(0.2f);
	mRightLeg = new IKLeg(mSkeleton, "RightUpLeg", "RightLeg", "RightFoot", "RightToeBase");
	mRightLeg->SetAnkleOffset(0.2f);

	ScalarTrack& rightTrack = mRightLeg->GetTrack();
	rightTrack.SetInterpolation(Interpolation::Cubic);
	rightTrack.Resize(4);
	rightTrack[0].mValue[0] = 1;
	rightTrack[1].mValue[0] = 0;
	rightTrack[2].mValue[0] = 0;
	rightTrack[3].mValue[0] = 1;
	rightTrack[0].mTime = 0.0f;
	rightTrack[1].mTime = 0.3f;
	rightTrack[2].mTime = 0.7f;
	rightTrack[3].mTime = 1.0f;

	ScalarTrack& leftTrack = mLeftLeg->GetTrack();
	leftTrack.SetInterpolation(Interpolation::Cubic);
	leftTrack.Resize(4);
	leftTrack[0].mValue[0] = 0;
	leftTrack[1].mValue[0] = 1;
	leftTrack[2].mValue[0] = 1;
	leftTrack[3].mValue[0] = 0;
	leftTrack[0].mTime = 0.0f;
	leftTrack[1].mTime = 0.4f;
	leftTrack[2].mTime = 0.6f;
	leftTrack[3].mTime = 1.0f;

	mCurrentClip = 0;
	mCurrentPose = mSkeleton.GetRestPose();

	mCurrentPoseVisual = new DebugDraw();
	mCurrentPoseVisual->FromPose(mCurrentPose);
	mCurrentPoseVisual->UpdateOpenGLBuffers();

	// For the UI
	for (unsigned int i = 0, size = (unsigned int)mClips.size(); i < size; ++i) {
		std::string& clipName = mClips[i].GetName();
		if (clipName == "Walking") {
			mCurrentClip = i;
		}
		unsigned int nameLength = (unsigned int)clipName.length();
	}

	mShowIKPose = false;
	mShowCurrentPose = false;

	mShowEnvironment = true;
	mShowMesh = true;
	mDepthTest = false;
	mTimeMod = 1.0f;
	mSinkIntoGround = 0.15f;

	// Start the character clamped to the ground. Move down a little bit so it's not perfectly up
	Ray groundRay(vec3(mModel.position.x, 11, mModel.position.z));
	unsigned int numTriangles = (unsigned int)mTriangles.size();
	vec3 hitPoint;
	for (unsigned int i = 0; i < numTriangles; ++i) {
		if (RaycastTriangle(groundRay, mTriangles[i], hitPoint)) {
			mModel.position = hitPoint;
			break;
		}
	}
	mModel.position.y -= mSinkIntoGround;
	mLastModelY = mModel.position.y;

	mToeLength = 0.3f;
}

void Sample::Update(float deltaTime) {
	float rayHeight = 2.1f;
	unsigned int numTriangles = (unsigned int)mTriangles.size();
	deltaTime = deltaTime * mTimeMod;
	vec3 hitPoint;

	// Increment time and sample the animation clip that moves the model on the level rails
	// The Y position is a lie, it's a trackt hat only makes sense from an ortho top view
	mWalkingTime += deltaTime * 0.3f;
	while (mWalkingTime > 6.0f) { mWalkingTime -= 6.0f; }

	// Figure out the X and Z position of the model in world spcae
	// Keep the Y position the same as last frame for both to properly orient the model
	float lastYPosition = mModel.position.y;
	vec3 currentPosition = mMotionTrack.Sample(mWalkingTime, true);
	vec3 nextPosition = mMotionTrack.Sample(mWalkingTime + 0.1f, true);
	currentPosition.y = lastYPosition;
	nextPosition.y = lastYPosition;
	mModel.position = currentPosition;

	// Figure out the forward direction of the model in world spcae
	vec3 direction = normalized(nextPosition - currentPosition);
	quat newDirection = lookRotation(direction, vec3(0, 1, 0));
	if (dot(mModel.rotation, newDirection) < 0.0f) {
		newDirection = newDirection * -1.0f;
	}
	mModel.rotation = nlerp(mModel.rotation, newDirection, deltaTime * 10.0f);
	vec3 characterForward = mModel.rotation * vec3(0, 0, 1);

	// Figure out the Y position of the model in world spcae
	Ray groundRay(vec3(mModel.position.x, 11, mModel.position.z));
	for (unsigned int i = 0; i < numTriangles; ++i) {
		if (RaycastTriangle(groundRay, mTriangles[i], hitPoint)) {
			// Sink the model a little bit into the ground to avoid hyper extending it's legs
			mModel.position = hitPoint - vec3(0, mSinkIntoGround, 0);
			break;
		}
	}

	// Sample the current animation, update the pose visual and figure out where the left
	// and right leg are in their up/down animation cycle
	mPlaybackTime = mClips[mCurrentClip].Sample(mCurrentPose, mPlaybackTime + deltaTime);
	mCurrentPoseVisual->FromPose(mCurrentPose);
	float normalizedTime = (mPlaybackTime - mClips[mCurrentClip].GetStartTime()) / mClips[mCurrentClip].GetDuration();
	if (normalizedTime < 0.0f) { std::cout << "should not be < 0\n"; normalizedTime = 0.0f; }
	if (normalizedTime > 1.0f) { std::cout << "should not be < 0\n"; normalizedTime = 1.0f; }
	float leftMotion = mLeftLeg->GetTrack().Sample(normalizedTime, true);
	float rightMotion = mRightLeg->GetTrack().Sample(normalizedTime, true);

	// Construct a ray for the left ankle, store the world position and the predictive position
	// of the ankle. This is in case the raycasts below don't hit anything.
	vec3 worldLeftAnkle = combine(mModel, mCurrentPose.GetGlobalTransform(mLeftLeg->Ankle())).position;
	Ray leftAnkleRay(worldLeftAnkle + vec3(0, 2, 0));
	vec3 predictiveLeftAnkle = worldLeftAnkle;

	// Construct a ray for the right ankle, store the world position and the predictive position
	// of the ankle. This is in case the raycasts below don't hit anything.
	vec3 worldRightAnkle = combine(mModel, mCurrentPose.GetGlobalTransform(mRightLeg->Ankle())).position;
	Ray rightAnkleRay(worldRightAnkle + vec3(0, 2, 0));
	vec3 predictiveRightAnkle = worldRightAnkle;

	// Perform some raycasts for the feet, these are done in world space and
	// will define the IK based target points. For each ankle, we need to know
	// the current position (raycast from knee height to the sole of the foot height)
	// and the predictive position (infinate ray cast). The target point will be
	// between these two goals
	vec3 groundReference = mModel.position;
	for (unsigned int i = 0; i < numTriangles; ++i) {
		if (RaycastTriangle(leftAnkleRay, mTriangles[i], hitPoint)) {
			if (lenSq(hitPoint - leftAnkleRay.origin) < rayHeight * rayHeight) {
				worldLeftAnkle = hitPoint;

				if (hitPoint.y < groundReference.y) {
					groundReference = hitPoint - vec3(0, mSinkIntoGround, 0);
				}
			}
			predictiveLeftAnkle = hitPoint;
		}
		if (RaycastTriangle(rightAnkleRay, mTriangles[i], hitPoint)) {
			if (lenSq(hitPoint - rightAnkleRay.origin) < rayHeight * rayHeight) {
				worldRightAnkle = hitPoint;

				if (hitPoint.y < groundReference.y) {
					groundReference = hitPoint - vec3(0, mSinkIntoGround, 0);
				}
			}
			predictiveRightAnkle = hitPoint;
		}
	}

	// Lerp the Y position of the mode over a small period of time
	// Just to avoid poping
	mModel.position.y = mLastModelY;
	mModel.position = lerp(mModel.position, groundReference, deltaTime * 10.0f);
	mLastModelY = mModel.position.y;

	// Lerp between fully clamped to the ground, and somewhat clamped to the ground based on the 
	// current phase of the walk cycle
	worldLeftAnkle = lerp(worldLeftAnkle, predictiveLeftAnkle, leftMotion);
	worldRightAnkle = lerp(worldRightAnkle, predictiveRightAnkle, rightMotion);

	// Now that we know the position of the model, as well as the ankle we can solve the feet.
	mLeftLeg->SolveForLeg(mModel, mCurrentPose, worldLeftAnkle/*, worldLeftToe*/);
	mRightLeg->SolveForLeg(mModel, mCurrentPose, worldRightAnkle/*, worldRightToe*/);
	// Apply the solved feet
	Blend(mCurrentPose, mCurrentPose, mLeftLeg->GetAdjustedPose(), 1, mLeftLeg->Hip());
	Blend(mCurrentPose, mCurrentPose, mRightLeg->GetAdjustedPose(), 1, mRightLeg->Hip());

	// The toes are still wrong, let's fix those. First, construct some rays for the toes
	Transform leftAnkleWorld = combine(mModel, mCurrentPose.GetGlobalTransform(mLeftLeg->Ankle()));
	Transform rightAnkleWorld = combine(mModel, mCurrentPose.GetGlobalTransform(mRightLeg->Ankle()));

	vec3 worldLeftToe = combine(mModel, mCurrentPose.GetGlobalTransform(mLeftLeg->Toe())).position;
	vec3 leftToeTarget = worldLeftToe;
	vec3 predictiveLeftToe = worldLeftToe;

	vec3 worldRightToe = combine(mModel, mCurrentPose.GetGlobalTransform(mRightLeg->Toe())).position;
	vec3 rightToeTarget = worldRightToe;
	vec3 predictiveRightToe = worldRightToe;

	vec3 origin = leftAnkleWorld.position;
	origin.y = worldLeftToe.y;
	Ray leftToeRay(origin + characterForward * mToeLength + vec3(0, 1, 0));
	origin = rightAnkleWorld.position;
	origin.y = worldRightToe.y;
	Ray rightToeRay = Ray(origin + characterForward * mToeLength + vec3(0, 1, 0));

	// Next, see if the toes hit anything
	float ankleRayHeight = 1.1f;
	for (unsigned int i = 0; i < numTriangles; ++i) {
		if (RaycastTriangle(leftToeRay, mTriangles[i], hitPoint)) {
			if (lenSq(hitPoint - leftToeRay.origin) < ankleRayHeight * ankleRayHeight) {
				leftToeTarget = hitPoint;
			}
			predictiveLeftToe = hitPoint;
		}
		if (RaycastTriangle(rightToeRay, mTriangles[i], hitPoint)) {
			if (lenSq(hitPoint - rightToeRay.origin) < ankleRayHeight * ankleRayHeight) {
				rightToeTarget = hitPoint;
			}
			predictiveRightToe = hitPoint;
		}
	}

	// Place the toe target at the right location
	leftToeTarget = lerp(leftToeTarget, predictiveLeftToe, leftMotion);
	rightToeTarget = lerp(rightToeTarget, predictiveRightToe, rightMotion);

	// If the left or right toe hit, adjust the ankle rotation approrpaiteley
	vec3 leftAnkleToCurrentToe = worldLeftToe - leftAnkleWorld.position;
	vec3 leftAnkleToDesiredToe = leftToeTarget - leftAnkleWorld.position;
	if (dot(leftAnkleToCurrentToe, leftAnkleToDesiredToe) > 0.00001f) {
		quat ankleRotator = fromTo(leftAnkleToCurrentToe, leftAnkleToDesiredToe);
		Transform ankleLocal = mCurrentPose.GetLocalTransform(mLeftLeg->Ankle());

		quat worldRotatedAnkle = leftAnkleWorld.rotation * ankleRotator;
		quat localRotatedAnkle = worldRotatedAnkle * inverse(leftAnkleWorld.rotation);

		ankleLocal.rotation = localRotatedAnkle * ankleLocal.rotation;
		mCurrentPose.SetLocalTransform(mLeftLeg->Ankle(), ankleLocal);
	}

	vec3 rightAnkleToCurrentToe = worldRightToe - rightAnkleWorld.position;
	vec3 rightAnkleToDesiredToe = rightToeTarget - rightAnkleWorld.position;
	if (dot(rightAnkleToCurrentToe, rightAnkleToDesiredToe) > 0.00001f) {
		quat ankleRotator = fromTo(rightAnkleToCurrentToe, rightAnkleToDesiredToe);
		Transform ankleLocal = mCurrentPose.GetLocalTransform(mRightLeg->Ankle());

		quat worldRotatedAnkle = rightAnkleWorld.rotation * ankleRotator;
		quat localRotatedAnkle = worldRotatedAnkle * inverse(rightAnkleWorld.rotation);

		ankleLocal.rotation = localRotatedAnkle * ankleLocal.rotation;
		mCurrentPose.SetLocalTransform(mRightLeg->Ankle(), ankleLocal);
	}

	// Update the matrix palette for skinning
	mCurrentPose.GetMatrixPalette(mPosePalette);
}

void Sample::Render(float inAspectRatio) {
	mat4 projection = perspective(60.0f, inAspectRatio, 0.01f, 1000.0f);
	mat4 view = lookAt(vec3(mModel.position.x, 0, mModel.position.z) + vec3(0, 5, 10), vec3(mModel.position.x, 0, mModel.position.z) + vec3(0, 3, 0), vec3(0, 1, 0));
	mat4 model = transformToMat4(mModel);
	mat4 mvp = projection * view * model;
	mat4 vp = projection * view;

	if (mShowMesh) {
		Shader* characterShader = mSkinnedShader;

		characterShader->Bind();
		Uniform<mat4>::Set(characterShader->GetUniform("model"), model);
		Uniform<mat4>::Set(characterShader->GetUniform("view"), view);
		Uniform<mat4>::Set(characterShader->GetUniform("projection"), projection);
		Uniform<vec3>::Set(characterShader->GetUniform("light"), vec3(1, 1, 1));
		Uniform<mat4>::Set(characterShader->GetUniform("pose"), mPosePalette);
		Uniform<mat4>::Set(characterShader->GetUniform("invBindPose"), mSkeleton.GetInvBindPose());

		mDiffuseTexture->Set(characterShader->GetUniform("tex0"), 0);
		for (unsigned int i = 0, size = (unsigned int)mMeshes.size(); i < size; ++i) {
			mMeshes[i].Bind(characterShader->GetAttribute("position"), characterShader->GetAttribute("normal"), characterShader->GetAttribute("texCoord"), characterShader->GetAttribute("weights"), characterShader->GetAttribute("joints"));
			mMeshes[i].Draw();
			mMeshes[i].UnBind(characterShader->GetAttribute("position"), characterShader->GetAttribute("normal"), characterShader->GetAttribute("texCoord"), characterShader->GetAttribute("weights"), characterShader->GetAttribute("joints"));
		}
		mDiffuseTexture->UnSet(0);
		characterShader->UnBind();
	}

	if (mShowEnvironment) {
		Shader* environmentShader = mStaticShader;
		environmentShader->Bind();
		Uniform<mat4>::Set(environmentShader->GetUniform("model"), mat4());
		Uniform<mat4>::Set(environmentShader->GetUniform("view"), view);
		Uniform<mat4>::Set(environmentShader->GetUniform("projection"), projection);
		Uniform<vec3>::Set(environmentShader->GetUniform("light"), vec3(1, 1, 1));
		mCourseTexture->Set(environmentShader->GetUniform("tex0"), 0);
		for (unsigned int i = 0, size = (unsigned int)mIKCourse.size(); i < size; ++i) {
			mIKCourse[i].Bind(environmentShader->GetAttribute("position"), environmentShader->GetAttribute("normal"), environmentShader->GetAttribute("texCoord"), -1, -1);
			mIKCourse[i].Draw();
			mIKCourse[i].UnBind(environmentShader->GetAttribute("position"), environmentShader->GetAttribute("normal"), environmentShader->GetAttribute("texCoord"), -1, -1);
		}
		mCourseTexture->UnSet(0);
		environmentShader->UnBind();
	}

	if (!mDepthTest) {
		glDisable(GL_DEPTH_TEST);
	}

	if (mShowCurrentPose) {
		mCurrentPoseVisual->UpdateOpenGLBuffers();
		mCurrentPoseVisual->Draw(DebugDrawMode::Lines, vec3(0, 0, 1), mvp);
	}

	if (mShowIKPose) {
		mLeftLeg->Draw(vp, vec3(1, 0, 0));
		mRightLeg->Draw(vp, vec3(0, 1, 0));
	}

	if (!mDepthTest) {
		glEnable(GL_DEPTH_TEST);
	}
}

void Sample::Shutdown() {
	delete mCurrentPoseVisual;
	delete mStaticShader;
	delete mDiffuseTexture;
	delete mSkinnedShader;
	delete mCourseTexture;
	delete mLeftLeg;
	delete mRightLeg;
	mClips.clear();
	mMeshes.clear();
}