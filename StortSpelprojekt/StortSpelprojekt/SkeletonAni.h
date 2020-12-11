#pragma once

#include "Buffers.h"

#define CLAMP(x, low, high)  (((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))



struct Bone
{
	unsigned int index;
	float frame;
	std::string name;
	std::string parentName;
	dx::SimpleMath::Quaternion rotationQuaternion;
	dx::SimpleMath::Vector3 translationVector;
	
};


class SkeletonAni
{
private:
	float length, fps, currentFrame, animationTime, t;
	unsigned int firstIndex, secondIndex;
	std::map<std::string, unsigned int> boneIDMap;
	std::vector<std::vector<Bone>> keyBones;
	std::vector<dx::SimpleMath::Matrix> offsetM;
	UINT counter = 0;
	dx::SimpleMath::Matrix transM;
	dx::SimpleMath::Matrix rotQM;
	dx::SimpleMath::Vector3 transV;
	dx::SimpleMath::Quaternion rotQ;
	dx::SimpleMath::Matrix RT;
	std::vector<dx::XMFLOAT4X4> bones;
	unsigned int ftIndex;
	
public:
	SkeletonAni();
	std::vector<dx::XMFLOAT4X4>& Makeglobal(float elapsedTime, const DirectX::XMMATRIX& globalParent, std::vector<Bone>& keys);
	std::string GetRootName();
	unsigned int GetNrOfBones() const;
	void SetUpOffsetsFromMatrices(std::vector<SkeletonOffsetsHeader>& offsets);

	std::vector<Bone>* GetRootKeyJoints();
	void SetUpIDMapAndFrames(std::map<std::string, unsigned int> boneIDMap, float fps, float aniLenght);
	void SetUpKeys(std::string boneName, std::vector<SkeletonKeysHeader>& keys);
	std::map<std::string, unsigned int>& GetBoneIDMap();
	std::vector<std::vector<Bone>>& GetKeyFrames();
	std::vector<dx::SimpleMath::Matrix>& GetOffsets();
	//float GetCurrentFrame() { return this->currentFrame; }
	float GetFPS();
	float GetAniLength();
	void SetOffsetsDirect(std::vector<dx::SimpleMath::Matrix>& directOffsets);
	void SetKeyFramesDirect(std::vector<std::vector<Bone>>& directKeys);

	//BLEND GREJJS VIKTOR
	void SetQuaternionsDirect(dx::SimpleMath::Quaternion directQuat);
	void SetTransVector(dx::SimpleMath::Vector3 transVec);

	//BLENDA STRUCTS MED FILIPS CURSED KOD
	void MergeKeys(std::vector<std::vector<Bone>> keys, float factor, float size);
	void MergeOffsets(std::vector<dx::SimpleMath::Matrix> offset, float factor, float i);
	void SetBlendAnimLength(float animLength, float factor);
	void SetBlendFPS(float fps, float factor);

	std::vector<dx::XMFLOAT4X4> GetBones() { return this->bones; }
	dx::SimpleMath::Matrix& Lerp(float elapsedTime, std::vector<Bone>& keys);

	dx::SimpleMath::Quaternion sLerpQuaternions(dx::SimpleMath::Quaternion quat, dx::SimpleMath::Quaternion quat2);
	void FindChildren(float elapsedTime, const DirectX::XMMATRIX& globalParent, std::vector<Bone>& keys, std::map<std::string, unsigned int>& map, int startJoint, int endJoint);
	
};

