#pragma once
#include "ObjectPooler.h"
#include "ResourceManager.h"
#include "WorldDescription.h"
#include "WorldGenerator.h"
#include "SaveState.h"
#include "ItemManager.h"

constexpr int TERRAIN_RELEVANT_RADIUS = 2;
constexpr int MAX_RELEVANT = (TERRAIN_RELEVANT_RADIUS * 2 + 1) * (TERRAIN_RELEVANT_RADIUS * 2 + 1);

class World
{
	typedef std::unordered_map<int, Chunk*> ChunkMap;
	const int DEFAULT_RELEVANT_INDEX = -100000;

public:
	World();
	virtual ~World();

	void Initialize(Object* root, ItemManager* items, Renderer* renderer);

	void ConstructSegment(const SaveState& state);
	void DeconstructSegment();
	
	void ResetRelevanceIndex();
	void UpdateRelevantChunks (const Transform& transform, CameraComponent* camera);
	void DrawDebug();

	Chunk* GetChunk(const float& x, const float& z) const;
	float SampleHeight(const float& x, const float& z) const;
	float SampleRoadInfluence(const float& x, const float& z) const;
	void SampleNormal(const float& x, const float& z, dx::XMFLOAT3& normal) const;
	void GetChunksInRadius(const dx::XMINT2& index, int radius, Chunk* chunks[], size_t& count) const;

	Path& GetPath() { return this->generator.GetPath(); }
	WorldDescription& GetDescription() { return this->description; }
	const WorldDescription& GetDescription() const { return this->description; }

	dx::XMFLOAT3 GetPlayerPositionFromHouse(Object* house)const;
	void UpdateGrass(ID3D11DeviceContext* context);

private:
	WorldDescription DescriptionFromState(const SaveState& state) const;

	dx::XMINT2 GetChunkIndex(Object* object) const;
	int TryGetQueueCount(std::string key,  const std::map<std::string, int>& queueCountTable, int defaultCount = 1) const;
	
	void RegisterFood(ObjectSpawner* spawner, const std::map<std::string, int>& queueCountTable) const;
	void RegisterFuel(ObjectSpawner* spawner, const std::map<std::string, int>& queueCountTable) const;
	void RegisterHealth(ObjectSpawner* spawner, const std::map<std::string, int>& queueCountTable) const;
	void RegisterWeapon(ObjectSpawner* spawner, const std::map<std::string, int>& queueCountTable) const;
	void RegisterStatic(ObjectSpawner* spawner, const std::map<std::string, int>& queueCountTable) const;

private:
	WorldGenerator generator;
	WorldDescription description;
	Renderer* renderer;
	ResourceManager* resources;
	ItemManager* items;

	Chunk* relevant[MAX_RELEVANT];
	size_t currentRelevantCount;

	//std::vector<Chunk*> relevant;	
	dx::XMINT2 lastRelevantIndex;
};