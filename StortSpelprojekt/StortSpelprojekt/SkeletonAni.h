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
	dx::SimpleMath::Matrix& Lerp(float elapsedTime, std::vector<Bone>& keys);
	
public:
	SkeletonAni();
	std::vector<dx::XMFLOAT4X4>& Makeglobal(float elapsedTime, const DirectX::XMMATRIX& globalParent, std::vector<Bone>& keys);
	std::string GetRootName();
	unsigned int GetNrOfBones() const;
	void SetUpOffsetsFromMatrices(std::vector<SkeletonOffsetsHeader>& offsets);
	std::vector<Bone>& GetRootKeyJoints();
	void SetUpIDMapAndFrames(std::map<std::string, unsigned int> boneIDMap, float fps, float aniLenght);
	void SetUpKeys(std::string boneName, std::vector<SkeletonKeysHeader>& keys);
	std::map<std::string, unsigned int>& GetBoneIDMap();
	std::vector<std::vector<Bone>>& GetKeyFrames();
	std::vector<dx::SimpleMath::Matrix>& GetOffsets();
	float GetFPS();
	float GetAniLength();
	void SetOffsetsDirect(std::vector<dx::SimpleMath::Matrix>& directOffsets);
	void SetKeyFramesDirect(std::vector<std::vector<Bone>>& directKeys);
	

};

