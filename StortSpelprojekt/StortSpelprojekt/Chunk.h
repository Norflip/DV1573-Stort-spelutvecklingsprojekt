#pragma once
#include <DirectXMath.h>
namespace dx = DirectX;

#include "Component.h"
#include "Math.h"
#include "Physics.h"
#include "DShape.h"

constexpr int CHUNK_SIZE = 32;
constexpr float TERRAIN_SCALE = 10.0f;

enum class ChunkType
{
	START,
	END,
	PATH,
	PUZZEL,
	NON_PATH,
	TERRAIN
};

#define USE_RIGIDBODY 1
#define DRAW_DEBUG 0

struct ChunkIndex
{
	int x;
	int z;
	ChunkType type;
};

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
	Chunk(dx::XMINT2 index, ChunkType type);
	virtual ~Chunk();

#if _DEBUG and DRAW_DEBUG
	void Update(const float& deltaTime) override;
#endif // DEBUG

	void SetupCollisionObject(float* heightMap);
	float SampleHeight(float x, float z);

	void SetHeightMap(float* heightMap) { this->heightMap = heightMap; }
	float* GetHeightMap() const { return this->heightMap; }

	dx::XMINT2 GetIndex() const { return this->index; }
	ChunkType GetType() const { return this->type; }

	static dx::XMVECTOR IndexToWorld(const dx::XMINT2& index, float y);
	static dx::XMFLOAT2 IndexToWorldXZ(const dx::XMINT2& index);
	static dx::XMINT2 WorldToIndex(float x, float z);
	static int WorldToIndex1D(float x, float z);

	void PhysicRelease();

private:
	void GetHeightFieldMinMax(float* heightMap, size_t size, float& min, float& max);

private:
	float min, max;
	rp::Collider* collider;

#if USE_RIGIDBODY
	rp::RigidBody* body;
#else
	rp::CollisionBody* body;
#endif 

	rp::HeightFieldShape* shape;

	dx::XMINT2 index;
	ChunkType type;
	float* heightMap;
};
