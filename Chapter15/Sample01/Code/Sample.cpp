#define _CRT_SECURE_NO_WARNINGS
#include "Sample.h"
#include "GLTFLoader.h"
#include "Uniform.h"
#include "glad.h"
#include "AnimBaker.h"

#include <io.h> 
#define access _access_s

void Sample::Initialize() {
	cgltf_data* gltf = LoadGLTFFile("Assets/Woman.gltf");
	mMeshes = LoadMeshes(gltf);
	mSkeleton = LoadSkeleton(gltf);
	mClips = LoadAnimationClips(gltf);
	FreeGLTFFile(gltf);

	mCrowdShader = new Shader("Shaders/crowd.vert", "Shaders/lit.frag");
	mDiffuseTexture = new Texture("Assets/Woman.png");

	unsigned int numClips = (unsigned int)mClips.size();
	mTextures.resize(numClips);
	mCrowds.resize(numClips);
	for (unsigned int i = 0; i < numClips; ++i) {
		std::string fileName = "Assets/";
		fileName += mClips[i].GetName();
		fileName += ".animTex";
		bool fileExists = access(fileName.c_str(), 0) == 0;

		if (fileExists) {
			mTextures[i].Load(fileName.c_str());
		}
		else {
			mTextures[i].Resize(512);
			BakeAnimationToTexture(mSkeleton, mClips[i], mTextures[i]);
			mTextures[i].Save(fileName.c_str());
		}
	}

	SetCrowdSize(20);
}

void Sample::SetCrowdSize(unsigned int size) {
	std::vector<vec3> occupied;
	unsigned int numCrowds = (unsigned int)mCrowds.size();
	for (unsigned int i = 0; i < numCrowds; ++i) {
		mCrowds[i].Resize(20);
		mCrowds[i].RandomizeTimes(mClips[i]);
		mCrowds[i].RandomizePositions(occupied, vec3(-40, 0, -80.0f), vec3(40, 0, 30.0f), 2.0f);
	}
}

void Sample::Update(float deltaTime) {
	unsigned int numCrowds = (unsigned int)mCrowds.size();
	for (unsigned int i = 0; i < numCrowds; ++i) {
		mCrowds[i].Update(deltaTime, mClips[i], mTextures[i].Size());
	}
}

void Sample::Render(float inAspectRatio) {
	mat4 projection = perspective(60.0f, inAspectRatio, 0.01f, 1000.0f);
	mat4 view = lookAt(vec3(0, 15, 40), vec3(0, 3, 0), vec3(0, 1, 0));
	mat4 mvp = projection * view; // No model

	mCrowdShader->Bind();
	Uniform<mat4>::Set(mCrowdShader->GetUniform("view"), view);
	Uniform<mat4>::Set(mCrowdShader->GetUniform("projection"), projection);
	Uniform<vec3>::Set(mCrowdShader->GetUniform("light"), vec3(1, 1, 1));

	Uniform<mat4>::Set(mCrowdShader->GetUniform("invBindPose"), mSkeleton.GetInvBindPose());
	mDiffuseTexture->Set(mCrowdShader->GetUniform("tex0"), 0);

	unsigned int numCrowds = (unsigned int)mCrowds.size();
	for (unsigned int c = 0; c < numCrowds; ++c) {
		mTextures[c].Set(mCrowdShader->GetUniform("animTex"), 1);
		mCrowds[c].SetUniforms(mCrowdShader);
		for (unsigned int i = 0, size = (unsigned int)mMeshes.size(); i < size; ++i) {
			mMeshes[i].Bind(mCrowdShader->GetAttribute("position"), mCrowdShader->GetAttribute("normal"), mCrowdShader->GetAttribute("texCoord"), mCrowdShader->GetAttribute("weights"), mCrowdShader->GetAttribute("joints"));
			mMeshes[i].DrawInstanced(mCrowds[c].Size());
			mMeshes[i].UnBind(mCrowdShader->GetAttribute("position"), mCrowdShader->GetAttribute("normal"), mCrowdShader->GetAttribute("texCoord"), mCrowdShader->GetAttribute("weights"), mCrowdShader->GetAttribute("joints"));
		}
		mTextures[c].UnSet(1);
	}

	mDiffuseTexture->UnSet(0);
	mCrowdShader->UnBind();
}

void Sample::Shutdown() {
	delete mDiffuseTexture;
	delete mCrowdShader;
	mMeshes.clear();
	mClips.clear();
	mTextures.clear();
	mCrowds.clear();
}