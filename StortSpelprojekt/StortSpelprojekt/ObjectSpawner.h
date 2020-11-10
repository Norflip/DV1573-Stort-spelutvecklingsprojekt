#pragma once
#include "ObjectPooler.h"
#include "PointQuadTree.h"
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

	struct InstancedProp
	{
		Mesh* mesh;
		Material* material;

		float yOffset;
		float radius, padding;
		size_t queueCount;
	};

	const float SPAWN_HEIGHT = 12.0f;

public:
	ObjectSpawner();
	virtual ~ObjectSpawner();

	void Initialize(Object* root, ObjectPooler* pooler, Renderer* renderer, ResourceManager* resource, CameraComponent* camera);

	void Spawn(const SaveState& state, PointQuadTree& tree, std::unordered_map<int, Chunk*>& chunkMap, std::vector<Chunk*>& chunks, ID3D11Device* device);
	void Despawn();

	void RegisterItem(std::string key, float radius, float padding, float yOffset, size_t queueCount);
	void RegisterInstancedItem(Mesh* mesh, Material* material, float radius, float padding, float yOffset, size_t queueCount);

	void DrawDebug();

private:
	std::vector<dx::XMFLOAT2> CreateSpawnPositions(PointQuadTree& tree, float radius, std::unordered_map<int, Chunk*>& chunkMap);
	Chunk* GetChunk(float x, float z, std::unordered_map<int, Chunk*>& chunkMap) const;

private:
	std::vector<Object*> activeItems;
	std::vector<Object*> props;

	ObjectPooler* pooler;
	Object* root;

	std::vector<dx::XMFLOAT2> itemSpawnPositions;
	std::vector<dx::XMFLOAT2> propSpawnPositions;

	std::vector<Item> items;
	std::vector<InstancedProp> instancedProps;

	Renderer* renderer;
	ResourceManager* resources;
	CameraComponent* camera;
};