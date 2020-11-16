#pragma once
#include <DirectXMath.h>

#include "Component.h"
#include "Math.h"
#include "DShape.h"
#include "Texture.h"

enum class ChunkType
{
	START,
	END,
	PATH,
	PUZZEL,
	NON_PATH,
	TERRAIN
};

namespace dx = DirectX;

constexpr int CHUNK_SIZE = 32;
constexpr float TERRAIN_SCALE = 7.5f;
#define DRAW_DEBUG 0

class Physics;

class Chunk : public Component
{
	const dx::XMFLOAT3 DSHAPE_CHUNK_COLORS[6] 
	{
		dx::XMFLOAT3(1,0,0),				// red
		dx::XMFLOAT3(0.8f,0.1f,0),			// kinda red
		dx::XMFLOAT3(0,1,0),				// green
		dx::XMFLOAT3(0,0,1),				// blue
		dx::XMFLOAT3(0.5f, 0.5f, 0.5f),		// grey
		dx::XMFLOAT3(1,1,1)					// white
	};

public:
	struct Data
	{
		float* heightMap;
		float* influenceMap;
		Texture* dataTexture;
		Data() : heightMap(nullptr), influenceMap(nullptr), dataTexture(nullptr) {}
	};

public:
	Chunk(dx::XMINT2 index, ChunkType type, const Data& data);
	virtual ~Chunk();

	void Update(const float& deltaTime) override;

	void Create();

	bool TryGetLocalColRow(const float& x, const float& z, int& col, int& row) const;
	float SampleHeight(const float& x, const float& z) const;
	float SampleInfluence(const float& x, const float& z) const;

	dx::XMINT2 GetIndex() const { return this->index; }
	ChunkType GetType() const { return this->type; }

	static dx::XMVECTOR IndexToWorld(const dx::XMINT2& index, float y);
	static dx::XMFLOAT2 IndexToWorldXZ(const dx::XMINT2& index);
	static dx::XMINT2 WorldToIndex(float x, float z);
	static int WorldToIndex1D(float x, float z);

	void PhysicRelease();

private:
	void SetupCollisionObject();

private:
	float min, max;
	rp::Collider* collider;
	rp::RigidBody* body;
	rp::HeightFieldShape* shape;

	Data data;
	dx::XMINT2 index;
	ChunkType type;
};
