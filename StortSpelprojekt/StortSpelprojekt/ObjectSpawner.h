#pragma once
#include "ObjectPooler.h"
#include "PointQuadTree.h"
#include "PossionDiscSampler.h"
#include "Random.h"

enum class ItemSpawnType
{
	DYNAMIC,
	STATIC
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



	void Spawn(PointQuadTree& tree, std::unordered_map<int, Chunk*>& chunkMap, std::vector<Chunk*>& chunks);
	void Despawn();

	void AddProp(std::string key, float radius, float padding, float yOffset, int queueCount);
	void AddItem(std::string key, float radius, float padding, float yOffset, int queueCount);

	void AddItem(std::string key, float radius, float padding, size_t min, size_t max, float yOffset, ItemSpawnType type = ItemSpawnType::STATIC);
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