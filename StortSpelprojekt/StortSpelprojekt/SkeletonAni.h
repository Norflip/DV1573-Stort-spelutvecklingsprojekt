#pragma once
#include <string>
#include <DirectXMath.h>
#include <map>
#include <ImportZWEB.h>

#define CLAMP(x, low, high)  (((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))

struct SkeletonData
{
	DirectX::XMFLOAT4X4 bones[60]; //Is it possible to put this into a structured buffer?
};
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
	SkeletonData skeletonDataB;
	DirectX::XMMATRIX& lerp(float elapsedTime, std::vector<Bone>& keys);
public:
	SkeletonAni();
	void makeglobal(float elapsedTime, const DirectX::XMMATRIX& globalParent, std::vector<Bone>& keys);
	std::string getRootName();
	unsigned int getNrOfBones() const;
	void setUpOffsetsFromMatrices(std::vector<SkeletonOffsetsHeader>& offsets);
	std::vector<Bone>& getRootKeyJoints();
	void setUpIDMapAndFrames(std::map<std::string, unsigned int> boneIDMap, float fps, float aniLenght);
	void setUpKeys(std::string boneName, std::vector<SkeletonKeysHeader>& keys);
	const SkeletonData& getSkeletonData();
};

