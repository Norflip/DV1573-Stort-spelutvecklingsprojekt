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
#include "DShape.h"
#include "PossionDiscSampler.h"
#include "Path.h"

constexpr int LOAD_RADIUS = 1;
constexpr size_t MAX_CHUNK_RENDER = 16;

class WorldGenerator
{
public:
	WorldGenerator();
	virtual ~WorldGenerator();

	void InitalizeGrass(ID3D11Device* device,ID3D11DeviceContext* context);
	void InitializeTrees(std::vector<Mesh> models, std::vector<Material> materials, ID3D11Device* device);
	void Initialize(ID3D11Device* device);

	void Generate(const SaveState& levelState, ID3D11Device* device, Object* root);
	void DrawShapes ();
	
private:
	Chunk* CreateChunk(ChunkType type, dx::XMINT2 index, const Noise::Settings& settings, ID3D11Device* device, Object* root);
	int GetSegmentSeed(const SaveState& levelState) { return levelState.seed ^ std::hash<int>()(levelState.segment); }
	dx::XMFLOAT3 CalculateNormal(float x, float y, const Noise::Settings& settings) const;

private:
	Mesh chunkMesh;
	Shader shader;
	Path path;

	std::unordered_map<int, float*> chunkData;
	std::unordered_map<int, Chunk*> chunkMap;
	std::vector<Chunk*> chunks;

	std::vector<GrassComponent*> grassComponents;
	Shader grassShader;
	std::vector<Mesh::Vertex> grassV;
	std::vector<unsigned int> grassI;
	
	
};