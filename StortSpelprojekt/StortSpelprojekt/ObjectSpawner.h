#pragma once
#include "ObjectPooler.h"
#include "QuadTree.h"
#include "PossionDiscSampler.h"
#include "Random.h"
#include "SaveState.h"
#include "ResourceManager.h"
#include "PickupComponent.h"
#include "ItemManager.h"

#include <DirectXMath.h>
namespace dx = DirectX;

#define SPAWN_ITEMS TRUE

class World;

class ObjectSpawner
{
	struct Prop
	{
		Mesh* mesh;
		Material* material;
		Bounds bounds;
		float yOffset;
		size_t queueCount;
		dx::XMUINT3 randomRotationAxis;
	};

	struct TreeModel
	{
		float minScale;
		float maxScale;
		float minRotation;
		float maxRotation;
		float discRadius;
		float segmentScaleFactor;
		std::vector<Mesh*> meshes;
		std::vector<Material*> materials;

	};

	const float SPAWN_HEIGHT = 12.0f;
	const float TREE_SPAWN_FACTOR = 0.4f;
	const float TREE_RADIUS = 1.0f;
	const float TREE_HEIGHT_ADJUSTMENT_FACTOR = 0.9f;
	const float NO_TREE_CHANCE = 0.5f;

public:
	ObjectSpawner();
	virtual ~ObjectSpawner();

	void Initialize(Object* root, World* world, ItemManager* items, Renderer* renderer);

	void Spawn(const SaveState& state, const Bounds& worldBounds, std::unordered_map<int, Chunk*>& chunkMap);
	void Despawn();

	void SpawnSpecific(std::vector<dx::XMFLOAT2> positions, dx::XMVECTOR axis, std::vector<float> angles, std::string modelName, std::unordered_map<int, Chunk*>& chunkMap, std::function<void(Object*)> modifier);

	//void RegisterItem(std::string key, size_t queueCount, std::function<Object* (ResourceManager*)> factory);
	void RegisterInstancedItem(std::string key, float yOffsetFactor, size_t queueCount, dx::XMUINT3 rotationAxis);
	void RegisterInstancedItem(Mesh* mesh, Material* material, float yOffset, size_t queueCount, dx::XMUINT3 rotationAxis);
	void DrawDebug();

private:
	void SpawnStatic(Chunk* chunk);
	void SpawnItem(Chunk* chunk);

	bool ValidSpawnPoint(const dx::XMFLOAT2& point, Chunk* chunk, float minInfluence) const;
	void AddTreesToChunk(const TreeModel& treeModel, Chunk* chunk, size_t segment) const;
	void AddGrassToChunk(Chunk* chunk) const;

private:
	std::vector<dx::XMFLOAT2> CreateSpawnPositions(QuadTree* tree, float spawnRadius, float itemRadius, std::unordered_map<int, Chunk*>& chunkMap) const;
	template <typename T>
	void ShuffleVector(std::vector<T>& v) const;

private:
	std::vector<Object*> props;

	//std::vector<dx::XMFLOAT3> TMP_POS;

	ObjectPooler* pooler;
	Object* root;
	World* world;
	QuadTree* environmentQT;
	ItemManager* items;

	std::vector<dx::XMFLOAT2> itemSpawnPositions;
	std::vector<dx::XMFLOAT2> propSpawnPositions;

	//std::vector<Item> itemRegistry;
	std::vector<Prop> instancedProps;
	TreeModel baseTreeModel;

	Renderer* renderer;
};

template<typename T>
inline void ObjectSpawner::ShuffleVector(std::vector<T>& v) const
{
	size_t elements = v.size();
	for (size_t i = 1; i < elements; i++)
	{
		size_t swapIndex = Random::Range(0, i - 1);
		T tmp = v[i];
		v[i] = v[swapIndex];
		v[swapIndex] = tmp;
	}
}
