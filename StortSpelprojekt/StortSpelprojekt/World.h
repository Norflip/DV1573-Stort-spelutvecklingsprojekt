#pragma once
#include "ObjectPooler.h"
#include "ResourceManager.h"
#include "SegmentDescription.h"
#include "SegmentGenerator.h"
#include "SaveState.h"

class World
{
	typedef std::unordered_map<int, Chunk*> ChunkMap;
	const int RELEVANT_RADIUS = 2;

public:
	World();
	virtual ~World();

	void Initialize(Object* root, ResourceManager* resources, ObjectPooler* pooler, Renderer* renderer, CameraComponent* camera);

	void ConstructSegment(SaveState state, SegmentDescription description);
	void DeconstructSegment();
	
	void UpdateRelevantChunks ();

	void SetPlayer(Object* object) { this->player = object; }
	void SetHouse(Object* object) { this->house = object; }
	void DrawDebug();

	Path& GetPath() { return this->generator.GetPath(); }
	void MoveHouseAndPlayerToStart();

private:
	dx::XMINT2 GetChunkIndex(Object* object) const;
	void RegisterToPool(ObjectPooler* pooler, ObjectSpawner* spawner, const std::map<std::string, int>& queueCountTable) const;
	int TryGetQueueCount(std::string key,  const std::map<std::string, int>& queueCountTable, int defaultCount = 1) const;
	
	void RegisterFood(ObjectPooler* pooler, ObjectSpawner* spawner, const std::map<std::string, int>& queueCountTable) const;
	void RegisterFuel(ObjectPooler* pooler, ObjectSpawner* spawner, const std::map<std::string, int>& queueCountTable) const;
	void RegisterHealth(ObjectPooler* pooler, ObjectSpawner* spawner, const std::map<std::string, int>& queueCountTable) const;
	void RegisterWeapon(ObjectPooler* pooler, ObjectSpawner* spawner, const std::map<std::string, int>& queueCountTable) const;
	void RegisterStatic(ObjectPooler* pooler, ObjectSpawner* spawner, const std::map<std::string, int>& queueCountTable) const;

private:
	SegmentGenerator generator;
	ResourceManager* resources;
	std::vector<Chunk*> relevant;

	ChunkMap relevantChunkMap;
	ChunkMap chunkMap;
	
	dx::XMINT2 playerIndex;
	Object* player;
	Object* house;
};