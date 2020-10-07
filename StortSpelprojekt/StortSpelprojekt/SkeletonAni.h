#pragma once
#include <string>
#include <DirectXMath.h>
#include <map>
#include <ImportZWEB.h>
#include "Buffers.h"

#define CLAMP(x, low, high)  (((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))


//I have it here becaúse I can't include engine, I will need to make a separate h file for these things,
struct Bone
{
	unsigned int index;
	float frame;
	std::string name;
	std::string parentName;
	DirectX::XMVECTOR rotationQuaternion;
	DirectX::XMVECTOR translationVector;
};

class SkeletonAni
{
private:
	float lenght, fps, currentFrame, animationTime, t;
	unsigned int firstIndex, secondIndex;
	std::map<std::string, unsigned int> boneIDMap;
	std::vector<std::vector<Bone>> keyBones;
	std::vector<DirectX::XMMATRIX> offsetM;
	DirectX::XMMATRIX transM;
	DirectX::XMMATRIX rotQM;
	DirectX::XMVECTOR transV;
	DirectX::XMVECTOR rotQ;
	DirectX::XMMATRIX RT;
	
	DirectX::XMMATRIX& Lerp(float elapsedTime, std::vector<Bone>& keys);
public:
	SkeletonAni();
	void Makeglobal(float elapsedTime, const DirectX::XMMATRIX& globalParent, std::vector<Bone>& keys);
	std::string GetRootName();
	unsigned int GetNrOfBones() const;
	void SetUpOffsetsFromMatrices(std::vector<SkeletonOffsetsHeader>& offsets);
	std::vector<Bone>& GetRootKeyJoints();
	void SetUpIDMapAndFrames(std::map<std::string, unsigned int> boneIDMap, float fps, float aniLenght);
	void SetUpKeys(std::string boneName, std::vector<SkeletonKeysHeader>& keys);
	cb_Skeleton& GetSkeletonData();
	std::map<std::string, unsigned int>& getBoneIDMap();//This is useful in case you have multiple animations.
	cb_Skeleton skeletonDataB;

	
};

