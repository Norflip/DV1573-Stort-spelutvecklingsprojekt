#pragma once
#include "ObjectPooler.h"
#include "ResourceManager.h"
#include "SegmentDescription.h"
#include "SegmentGenerator.h"
#include "SaveState.h"

class World
{
	typedef std::unordered_map<int, Chunk*> ChunkMap;
	const int RELEVANT_RADIUS = 1;

public:
	World();
	virtual ~World();

	void Initialize(Object* root, ResourceManager* resources, Renderer* renderer);

	void ConstructSegment(const SaveState& state);
	void DeconstructSegment();
	
	void UpdateRelevantChunks (const Transform& transform, CameraComponent* camera);
	void DrawDebug();

	Chunk* GetChunk(const float& x, const float& z) const;
	float SampleHeight(const float& x, const float& z) const;
	float SampleRoadInfluence(const float& x, const float& z) const;
	void SampleNormal(const float& x, const float& z, dx::XMFLOAT3& normal) const;
	void GetChunksInRadius(const dx::XMINT2& index, int radius, std::vector<Chunk*>& chunks) const;

	Path& GetPath() { return this->generator.GetPath(); }

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

	ObjectSpawner* spawner;
	ObjectPooler* pooler;
	
	dx::XMINT2 lastRelevantIndex;
};