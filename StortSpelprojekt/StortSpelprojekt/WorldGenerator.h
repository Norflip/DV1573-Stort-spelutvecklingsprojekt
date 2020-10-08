#pragma once

#include <unordered_map>
#include <unordered_set>
#include <random>
#include "GrassComponent.h"
#include "Object.h"
#include "Chunk.h"
#include "MeshComponent.h"
#include "Noise.h"
#include "ShittyOBJLoader.h"
#include "Math.h"
#include "SaveState.h"

constexpr int LOAD_RADIUS = 1;
constexpr size_t MAX_CHUNK_RENDER = 16;

typedef std::vector<dx::XMINT2> Path;

class WorldGenerator
{
public:
	WorldGenerator();
	virtual ~WorldGenerator();
	void InitalizeGrass(ID3D11Device* device,ID3D11DeviceContext* context);
	void Initialize(ID3D11Device* device);
	void Generate(const SaveState& levelState, ID3D11Device* device);
	void Draw(Renderer*, CameraComponent*);
	
	
private:
	Path CalculatePath (size_t steps, int seed);
	dx::XMINT2 GetDirection(dx::XMINT2 direction, float value, const std::default_random_engine& rng);

	float GetDistanceToPath(const dx::XMFLOAT2& position, const Path& path) const;
	dx::XMFLOAT2 PathIndexToWorld(const dx::XMINT2& i) const;
	
	Chunk* CreateChunk(ChunkType type, dx::XMINT2 index, const Path& path, ID3D11Device* device);
	int GetSegmentSeed(const SaveState& levelState) { return levelState.seed ^ std::hash<int>()(levelState.segment); }
	dx::XMFLOAT3 CalculateNormal(float x, float y, const Noise::Settings& settings) const;

	

private:
	bool initialized = false;
	Mesh chunkMesh;
	Shader shader;

	std::unordered_map<int, float*> chunkData;
	std::unordered_map<int, Chunk*> chunkMap;
	std::vector<Chunk*> chunks;

	std::vector<GrassComponent*> grassComponents;
	Shader grassShader;
	std::vector<Mesh::Vertex> grassV;
	std::vector<unsigned int> grassI;
	
	
};