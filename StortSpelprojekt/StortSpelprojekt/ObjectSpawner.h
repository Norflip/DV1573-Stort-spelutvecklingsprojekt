#pragma once
#include "ObjectPooler.h"
#include "PointQuadTree.h"
#include "PossionDiscSampler.h"
#include "Random.h"
#include "SaveState.h"

#define SPAWN_PROPS 0
#define SPAWN_ITEMS 1

enum class ItemSpawnType
{
	ITEM,
	PROP
};

class ObjectSpawner
{
	struct Item
	{
		std::string key;
		float yOffset;
		float radius, padding;
	};

	const float SPAWN_HEIGHT = 12.0f;

public:
	ObjectSpawner();
	virtual ~ObjectSpawner();

	void Initialize(Object* root, ObjectPooler* pooler);

	void Spawn(const SaveState& state, PointQuadTree& tree, std::unordered_map<int, Chunk*>& chunkMap, std::vector<Chunk*>& chunks);
	void Despawn();

	void RegisterItem(std::string key, float radius, float padding, float yOffset, int queueCount, ItemSpawnType type);

	void DrawDebug();

private:
	std::vector<dx::XMFLOAT2> CreateSpawnPositions(PointQuadTree& tree, float radius, std::unordered_map<int, Chunk*>& chunkMap);
	Chunk* GetChunk(float x, float z, std::unordered_map<int, Chunk*>& chunkMap) const;

private:
	std::vector<Object*> items;
	ObjectPooler* pooler;
	Object* root;

	std::vector<dx::XMFLOAT2> itemSpawnPositions;
	std::vector<dx::XMFLOAT2> propSpawnPositions;

	std::vector<Item> staticItems;
	std::vector<Item> dynamicItems;
};