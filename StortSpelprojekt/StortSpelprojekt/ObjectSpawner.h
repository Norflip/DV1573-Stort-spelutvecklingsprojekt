#pragma once
#include "ObjectPooler.h"
#include "QuadTree.h"
#include "PossionDiscSampler.h"
#include "Random.h"
#include "SaveState.h"
#include "ResourceManager.h"

#define SPAWN_ITEMS TRUE

class ObjectSpawner
{
	struct Item
	{
		std::string key;
		float yOffset;
		float radius, padding;
	};

	struct Prop
	{
		Mesh* mesh;
		Material* material;

		float yOffset;
		float radius, padding;
		size_t queueCount;
		dx::XMUINT3 randomRotationAxis;
	};

	const float SPAWN_HEIGHT = 12.0f;

public:
	ObjectSpawner();
	virtual ~ObjectSpawner();

	void Initialize(Object* root, ObjectPooler* pooler, Renderer* renderer);

	void Spawn(const SaveState& state, QuadTree& tree, std::unordered_map<int, Chunk*>& chunkMap, ID3D11Device* device);
	void Despawn();

	void SpawnSpecific(std::vector<dx::XMFLOAT2> positions, dx::XMVECTOR axis, std::vector<float> angles, std::string modelName, std::unordered_map<int, Chunk*>& chunkMap, std::function<void(Object*)> modifier);

	void RegisterItem(std::string key, float radius, float padding, float yOffset, size_t queueCount);
	void RegisterInstancedItem(Mesh* mesh, Material* material, float radius, float padding, float yOffset, size_t queueCount, dx::XMUINT3 rotationAxis);

	void DrawDebug();

private:


private:
	std::vector<dx::XMFLOAT2> CreateSpawnPositions(QuadTree& tree, float radius, std::unordered_map<int, Chunk*>& chunkMap);
	Chunk* GetChunk(float x, float z, std::unordered_map<int, Chunk*>& chunkMap) const;

private:
	std::vector<Object*> activeItems;
	std::vector<Object*> props;

	ObjectPooler* pooler;
	Object* root;

	std::vector<dx::XMFLOAT2> itemSpawnPositions;
	std::vector<dx::XMFLOAT2> propSpawnPositions;

	std::vector<Item> items;
	std::vector<Prop> instancedProps;

	Renderer* renderer;
};