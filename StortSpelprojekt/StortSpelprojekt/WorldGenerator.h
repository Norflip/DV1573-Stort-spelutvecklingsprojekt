#pragma once

#include <unordered_map>
#include <unordered_set>
#include <random>

#include "Object.h"
#include "Chunk.h"
#include "MeshComponent.h"
#include "Noise.h"
#include "ShittyOBJLoader.h"
#include "Math.h"
#include "SaveState.h"
#include "ChunkCollider.h"
#include "RigidBodyComponent.h"
#include "Physics.h"

constexpr int LOAD_RADIUS = 1;
constexpr size_t MAX_CHUNK_RENDER = 16;

typedef std::vector<dx::XMINT2> Path;

class WorldGenerator
{
public:
	WorldGenerator();
	virtual ~WorldGenerator();

	void Initialize(ID3D11Device* device);
	void Generate(const SaveState& levelState, Physics& physics, ID3D11Device* device);
	void Draw(Renderer*, CameraComponent*);

private:
	Path CalculatePath (size_t steps, int seed);
	dx::XMINT2 GetDirection(dx::XMINT2 direction, float value, const std::default_random_engine& rng);

	float GetDistanceToPath(const dx::XMFLOAT2& position, const Path& path) const;
	dx::XMFLOAT2 PathIndexToWorld(const dx::XMINT2& i) const;
	
	Chunk* CreateChunk(ChunkType type, dx::XMINT2 index, const Path& path, Physics& physics, ID3D11Device* device);
	dx::XMFLOAT3 CalculateNormal(float x, float y, const Noise::Settings& settings) const;

private:
	bool initialized = false;
	Mesh chunkMesh;
	Shader shader;

	std::unordered_map<int, float*> chunkData;
	std::unordered_map<int, Chunk*> chunkMap;
	std::vector<Chunk*> chunks;
};