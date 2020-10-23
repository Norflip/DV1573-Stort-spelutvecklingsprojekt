#pragma once
#include "Chunk.h"
#include "GridData.h"
#include "SaveState.h"
#include "SegmentDescription.h"
#include "Material.h"
#include "ResourceManager.h"
#include "PossionDiscSampler.h"

#include "GrassComponent.h"
#include "InstancedMeshComponent.h"
#include "PointQuadTree.h"
#include "ObjectSpawner.h"

#define CW_CHUNK_MESH 1

class SegmentGenerator
{
	struct ChunkPointInformation
	{
		float height;
		float distance;
	};

	const int CHUNK_PADDING = 2;
	const float FRACKIG_TREE_ADJUSTMENT = -3.0f;
	const float TREE_SPAWN_FACTOR = 0.4f;

public:
	SegmentGenerator();
	virtual ~SegmentGenerator();
	void Initialize(Object* root, ResourceManager* resourceManager, ObjectPooler* pooler, ID3D11Device* device, ID3D11DeviceContext* context);

	void Construct(const SegmentDescription& description);
	void Deconstruct();

	void GetChunksInRadius(const dx::XMINT2& index, int radius, std::vector<Chunk*>& chunks) const;
	bool IsConstructed() const { return this->constructed; }

	Chunk* GetChunk(float x, float z) const;
	void DrawDebug();

private:
	std::vector<ChunkPointInformation> CreateChunkMap(const dx::XMINT2& index, const SegmentDescription& description, float*& heightMap, unsigned char*& buffer);

	Chunk* CreateChunk(const dx::XMINT2& index, Object* root, const SegmentDescription& description, ChunkType type);
	void CreateChunkMesh(Mesh& mesh, ID3D11Device* device);

	void InitializeTrees(ResourceManager* resources);

	void AddTreesToChunk(Chunk* chunk, std::vector<ChunkPointInformation>& chunkInformation);
	void AddGrassToChunk(Chunk* chunk, Texture& texture);
	bool ValidateTreePoint(const dx::XMFLOAT2& point, std::vector<ChunkPointInformation>& chunkInformation);

private:
	bool constructed, initialized;
	GridData grid;
	ObjectSpawner spawner;
	std::vector<Chunk*> chunks;
	std::unordered_map<int, Chunk*> chunkMap;

	Shader* grassShader;
	Shader* chunkShader;

	std::vector<Mesh> stylizedTreeModel;
	std::vector<Material> stylizedTreeMaterial;

	Mesh chunkMesh;
	bool hasChunkMesh;
	std::vector<dx::XMFLOAT2> itemSpawns;
	std::vector<dx::XMFLOAT2> propSpawns;

	PointQuadTree treePoints;
	Object* root;

	ID3D11DeviceContext* context;
	ID3D11Device* device;
};