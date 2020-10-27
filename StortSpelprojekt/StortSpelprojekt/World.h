#pragma once
#include <functional>
#include "ObjectPooler.h"
#include "ResourceManager.h"
#include "SegmentDescription.h"
#include "SegmentGenerator.h"
#include "SaveState.h"

class World
{
public:
	World();
	virtual ~World();

	void Initialize(Object* root, ResourceManager* resources, ObjectPooler* pooler, Renderer* renderer);

	void ConstructSegment(SaveState state, SegmentDescription description);
	void DeconstructSegment();
	
	void UpdateRelevantChunks ();

	void SetPlayer(Object* object) { this->player = object; }
	void SetHouse(Object* object) { this->house = object; }
	void DrawDebug();

	void MoveHouseAndPlayerToStart();

	
	float TMP_OFFSET = 0.0f;

private:
	dx::XMINT2 GetChunkIndex(Object* object) const;
	void RegisterToPool(ObjectPooler* pooler);

private:
	SegmentGenerator generator;

	std::vector<Chunk*> relevant;
	std::unordered_map<int, Chunk*> relevantChunkMap;
	std::unordered_map<int, Chunk*> chunkMap;
	int chunkRelevancyRadius = 1;

	dx::XMINT2 playerIndex;
	Object* player;
	Object* house;
};