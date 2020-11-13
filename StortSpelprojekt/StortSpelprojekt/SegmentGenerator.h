#pragma once
#include "Chunk.h"
#include "ChunkGrid.h"
#include "SaveState.h"
#include "SegmentDescription.h"
#include "Material.h"
#include "ResourceManager.h"
#include "PossionDiscSampler.h"

#include "GrassComponent.h"
#include "QuadTree.h"
#include "ObjectSpawner.h"

class SegmentGenerator
{
	struct Data
	{
		
	};

	const int CHUNK_PADDING = 2;
	const float TREE_SPAWN_FACTOR = 0.4f;
	const float MIN_TERRAIN_HEIGHT = 0.1f;
	const float TERRAIN_BASE_HEIGHT = 1.0f;

	const float TREE_HEIGHT_ADJUSTMENT_FACTOR = 0.9f;

public:
	SegmentGenerator();
	virtual ~SegmentGenerator();
	void Initialize(Object* root, ResourceManager* resourceManager, ObjectSpawner* spawner, ID3D11Device* device, ID3D11DeviceContext* context);

	void Construct(const SaveState& state, const SegmentDescription& description);
	void Deconstruct();

	bool IsConstructed() const { return this->constructed; }

	void DrawDebug();

	Path& GetPath() { return this->path; }
	const Path& GetPath() const { return this->path; }

	std::unordered_map<int, Chunk*>& GetChunkMap() { return this->chunkMap; }
	const std::unordered_map<int, Chunk*>& GetChunkMap() const { return this->chunkMap; }

private:
	void CreateChunkMap(const dx::XMINT2& index, const SegmentDescription& description, Chunk::Data& data);

	Chunk* CreateChunk(const dx::XMINT2& index, Object* root, const SegmentDescription& description, ChunkType type);
	Mesh* CreateChunkMesh(ID3D11Device* device);

	void AddTreesToChunk(Chunk* chunk, const Chunk::Data& data);
	void AddGrassToChunk(Chunk* chunk, Texture* texture);
	bool ValidateTreePoint(const dx::XMFLOAT2& point, const Chunk::Data& data);


private:
	bool constructed, initialized;
	ChunkGrid grid;
	ObjectSpawner* spawner;
	
	cb_Material materialData;
	Shader* grassShader;
	Shader* chunkShader;

	Texture* grassTexture;
	Texture* roadTexture;

	Path path;
	std::unordered_map<int, Chunk*> chunkMap;

	std::vector<Mesh*> stylizedTreeModel;
	std::vector<Material*> stylizedTreeMaterial;

	Mesh* chunkMesh;
	bool hasChunkMesh;

	QuadTree treePoints;
	Object* root;

	ID3D11Device* device;
	ID3D11DeviceContext* context;

};