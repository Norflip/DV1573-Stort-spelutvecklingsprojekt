#include "Chunk.h"
#include <iostream>


Chunk::Chunk(dx::XMINT2 index, ChunkType type) : index(index), type(type), heightMap(nullptr)
{

}

Chunk::~Chunk()
{
	if (heightMap != nullptr)
	{
		delete heightMap;
		heightMap = nullptr;
	}
}

#if _DEBUG and DRAW_DEBUG
void Chunk::Update(const float& deltaTime)
{
	dx::XMFLOAT3 color = DSHAPE_CHUNK_COLORS[static_cast<int>(this->type)];

	dx::XMFLOAT3 center;
	dx::XMStoreFloat3(&center, GetOwner()->GetTransform().GetPosition());
	center.x += (float)CHUNK_SIZE / 2.0f;
	center.z += (float)CHUNK_SIZE / 2.0f;

	DShape::DrawWireBox(center, dx::XMFLOAT3(CHUNK_SIZE * 0.9f, 1.0f /*CHUNK_SIZE * 0.9f*/, CHUNK_SIZE * 0.9f), color);
}
#endif

void Chunk::SetupCollisionObject(float* heightMap)
{
	this->heightMap = heightMap;
	dx::XMFLOAT3 worldPosition;
	dx::XMStoreFloat3(&worldPosition, GetOwner()->GetTransform().GetPosition());


	rp::Transform transform;

	const float offset = CHUNK_SIZE / 2.0f;
	rp::Vector3 btPosition(worldPosition.x + offset, worldPosition.y, worldPosition.z + offset);
	transform.setPosition(btPosition);

	const int gridSize = static_cast<int>(CHUNK_SIZE) + 1;
	float min, max;
	GetHeightFieldMinMax(heightMap, gridSize, min, max);
	
	//min = -TERRAIN_SCALE;
	//max = TERRAIN_SCALE;

	rp::PhysicsCommon& common = Physics::Instance().GetCommon();
	shape = common.createHeightFieldShape(gridSize, gridSize, min, max, static_cast<void*>(heightMap), rp::HeightFieldShape::HeightDataType::HEIGHT_FLOAT_TYPE);

	rp::Transform colliderTransform;
	float originHeight = -(max - min) * 0.5f - min;
	std::cout << "ORIGIN HEIGHT: " << originHeight << std::endl;
	colliderTransform.setPosition(rp::Vector3(0, originHeight, 0));

	rp::PhysicsWorld* world = Physics::Instance().GetWorld();



#if  USE_RIGIDBODY

	body = world->createRigidBody(transform);
	body->setType(rp::BodyType::STATIC);
	body->enableGravity(false);
	collider = body->addCollider(shape, colliderTransform);

#else
	body = world->createCollisionBody(transform);
	collider = body->addCollider(shape, colliderTransform);

#endif //  USE_RIGIDBODY

	body->setUserData(static_cast<void*>(GetOwner()));
	body->setIsActive(true);
	collider->setCollisionCategoryBits(static_cast<unsigned short>(FilterGroups::TERRAIN));
	collider->setCollideWithMaskBits(static_cast<unsigned short>(FilterGroups::EVERYTHING));
	collider->getMaterial().setBounciness(0.f);
}

float Chunk::SampleHeight(float x, float z)
{
	dx::XMFLOAT3 position;
	dx::XMStoreFloat3(&position, GetOwner()->GetTransform().GetPosition());
	dx::XMFLOAT2 chunkPosition = IndexToWorldXZ(WorldToIndex(x, z));

	int col = static_cast<int>(floorf(x - chunkPosition.x));
	int row = static_cast<int>(floorf(z - chunkPosition.y));

	float height = 0.0f;

	if (col >= 0 && row >= 0 && col <= CHUNK_SIZE && row <= CHUNK_SIZE)
	{
		height = shape->getHeightAt(col, row);
		//std::cout << "col: " << row << ", row: " << col << ", height: " << height << std::endl;
	}
	else
	{
		std::cout << "FAILED to get chunk height: col: " << col << ", row: " << row << std::endl;
	}

	return height;
}

dx::XMVECTOR Chunk::IndexToWorld(const dx::XMINT2& index, float y)
{
	float x = static_cast<float>(index.x * (int)CHUNK_SIZE);// +((float)CHUNK_SIZE / 2.0f);
	float z = static_cast<float>(index.y * (int)CHUNK_SIZE);// - CHUNK_SIZE;// + ((float)CHUNK_SIZE / 2.0f);
	dx::XMVECTOR pos = { x,y,z };
	return pos;
}

dx::XMFLOAT2 Chunk::IndexToWorldXZ(const dx::XMINT2& index)
{
	float x = (float)index.x * (float)CHUNK_SIZE;
	float y = (float)index.y * (float)CHUNK_SIZE;// -CHUNK_SIZE;

	return dx::XMFLOAT2(x, y);
}

dx::XMINT2 Chunk::WorldToIndex(float x, float z)
{
	const float size = static_cast<float>(CHUNK_SIZE);
	int dx = static_cast<int>(floorf(x / size));
	int dz = static_cast<int>(floorf(z / size));
	return dx::XMINT2(dx, dz);
}

int Chunk::WorldToIndex1D(float x, float z)
{
	dx::XMINT2 index = WorldToIndex(x, z);
	return HASH2D_I(x, z);
}

void Chunk::PhysicRelease()
{
	Physics::Instance().GetWorld()->destroyCollisionBody(body);
}

void Chunk::GetHeightFieldMinMax(float* heightMap, size_t size, float& minv, float& maxv)
{
	minv = FLT_MAX;
	maxv = FLT_MIN;

	for (size_t i = 0; i < size * size; i++)
	{
		float height = heightMap[i];
		minv = std::min(height, minv);
		maxv = std::max(height, maxv);
	}

	std::cout << "min: " << minv << ", max: " << maxv << std::endl;
}
