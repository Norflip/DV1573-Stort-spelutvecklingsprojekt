#pragma once
#include "Chunk.h"
#include "SaveState.h"
#include "WorldDescription.h"
#include "Material.h"
#include "ResourceManager.h"
#include "PossionDiscSampler.h"

#include "GrassComponent.h"
#include "QuadTree.h"
#include "ObjectSpawner.h"

class World;

class WorldGenerator
{

	const int CHUNK_PADDING = 2;
	const float MIN_TERRAIN_HEIGHT = 0.1f;
	const float TERRAIN_BASE_HEIGHT = 1.0f;

public:

	typedef std::function<Object* (Chunk*, dx::XMVECTOR)> PropFactory;

	struct EnviromentProp
	{
		PropFactory* factory;
		bool onRoad;
		size_t width;
		size_t height;
		size_t minSegment;
		size_t maxSegment;
		size_t usedCount;
	};

	struct ChunkIndexInfo
	{
		dx::XMINT2 index;
		ChunkType type;
		EnviromentProp* prop;
	};


public:
	WorldGenerator();
	virtual ~WorldGenerator();
	void Initialize(Object* root, World* world, Renderer* renderer);

	void Construct(const SaveState& state, const WorldDescription& description);
	void Deconstruct();
	bool IsConstructed() const { return this->constructed; }
	void DrawDebug();

	ObjectSpawner* GetSpawner() const { return this->spawner; }
	Path& GetPath() { return this->path; }
	const Path& GetPath() const { return this->path; }
	std::unordered_map<int, Chunk*>& GetChunkMap() { return this->chunkMap; }
	const std::unordered_map<int, Chunk*>& GetChunkMap() const { return this->chunkMap; }

	size_t RegisterEnviromentProp (size_t minSegment, size_t maxSegment, size_t queueCount, PropFactory factory);
	//size_t RegisterRoadObstacle(size_t minSegment, size_t maxSegment);

private:
	void SetPathPointsToChunkType(const Path& path, std::unordered_map<int, ChunkIndexInfo>& chunkInformation);

	void AddEnvironmentProps(const size_t& segmentIndex, const WorldDescription& description, dx::XMINT2 minIndex, dx::XMINT2 maxIndex, std::unordered_map<int, ChunkIndexInfo>& chunkInformation);
	static bool SortProps(const EnviromentProp& propA, const EnviromentProp& propB);

	Chunk* CreateChunk(const ChunkIndexInfo& indexInfo, Object* root, const WorldDescription& description);
	Mesh* GetChunkMesh(ID3D11Device* device);

	void AddChunksFromPath(std::vector<dx::XMINT2>& path, std::unordered_map<int, ChunkIndexInfo>& chunks);
	void AddPadding(int radius, std::vector<dx::XMINT2>& points, std::unordered_map<int, ChunkIndexInfo>& chunks, dx::XMINT2& minIndex, dx::XMINT2& maxIndex);
	void UpdateDirection(dx::XMINT2& direction);
	
private:
	bool constructed, initialized;
	ObjectSpawner* spawner;
	
	cb_Material materialData;
	Shader* grassShader;
	Shader* chunkShader;

	Texture* grassTexture;
	Texture* roadTexture;

	std::vector<EnviromentProp> enviromentProps;

	std::vector<Mesh*> stylizedTreeModel;
	std::vector<Material*> stylizedTreeMaterial;

	Path path;
	std::unordered_map<int, Chunk*> chunkMap;

	Mesh* chunkMesh;
	bool hasChunkMesh;

	QuadTree treePoints;
	Object* root;
	Renderer* renderer;
};