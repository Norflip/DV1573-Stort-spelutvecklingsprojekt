#pragma once
#include <string>
#include <DirectXMath.h>
#include <map>
#include <ImportZWEB.h>
#include "Buffers.h"
#include <SimpleMath.h>

#define CLAMP(x, low, high)  (((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))


//I have it here becaúse I can't include engine, I will need to make a separate h file for these things,
struct Bone
{
	unsigned int index;
	float frame;
	std::string name;
	std::string parentName;
	/*dx::XMFLOAT4 rotationQuaternion;
	dx::XMFLOAT4 translationVector;*/
	dx::SimpleMath::Quaternion rotationQuaternion;
	dx::SimpleMath::Vector3 translationVector;
};

class SkeletonAni
{
private:
	float lenght, fps, currentFrame, animationTime, t;
	unsigned int firstIndex, secondIndex;
	std::map<std::string, unsigned int> boneIDMap;
	std::vector<std::vector<Bone>> keyBones;
	std::vector</*dx::XMFLOAT4X4*/dx::SimpleMath::Matrix> offsetM;
	/*dx::XMFLOAT4X4 transM;
	dx::XMFLOAT4X4 rotQM;
	dx::XMFLOAT4 transV;
	dx::XMFLOAT4 rotQ;
	dx::XMFLOAT4X4 RT;*/
	dx::SimpleMath::Matrix transM;
	dx::SimpleMath::Matrix rotQM;
	dx::SimpleMath::Vector3 transV;
	dx::SimpleMath::Quaternion rotQ;
	dx::SimpleMath::Matrix RT;
	
	/*DirectX::XMFLOAT4X4&*/dx::SimpleMath::Matrix& Lerp(float elapsedTime, std::vector<Bone>& keys);
public:
	SkeletonAni();
	std::vector<dx::XMFLOAT4X4>& Makeglobal(float elapsedTime, const DirectX::XMMATRIX& globalParent, std::vector<Bone>& keys);
	std::string GetRootName();
	unsigned int GetNrOfBones() const;
	void SetUpOffsetsFromMatrices(std::vector<SkeletonOffsetsHeader>& offsets);
	std::vector<Bone>& GetRootKeyJoints();
	void SetUpIDMapAndFrames(std::map<std::string, unsigned int> boneIDMap, float fps, float aniLenght);
	void SetUpKeys(std::string boneName, std::vector<SkeletonKeysHeader>& keys);
	//cb_Skeleton& GetSkeletonData();
	std::map<std::string, unsigned int>& getBoneIDMap();//This is useful in case you have multiple animations.
	//cb_Skeleton skeletonDataB;
	std::vector<dx::XMFLOAT4X4>& GetSkeletonData();
	std::vector<dx::XMFLOAT4X4> bones;
};

//imgui