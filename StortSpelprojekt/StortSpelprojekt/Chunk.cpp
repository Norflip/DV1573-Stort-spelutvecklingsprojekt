#include "Chunk.h"
#include <iostream>
#define USE_RIGIDBODY 1

Chunk::Chunk(dx::XMINT2 index, ChunkType type) : index(index), type(type), heightMap(nullptr)
{

}

Chunk::~Chunk()
{
	if (heightMap != nullptr)
	{
	//	delete heightMap;
	//	heightMap = nullptr;
	}
}

void Chunk::SetupCollisionObject(float* heightMap)
{
	this->heightMap = heightMap;
	dx::XMFLOAT3 worldPosition;
	dx::XMStoreFloat3(&worldPosition, GetOwner()->GetTransform().GetPosition());


	rp::Transform transform;

	const float offset = CHUNK_SIZE / 2.0f;
	rp::Vector3 btPosition (worldPosition.x + offset, worldPosition.y, worldPosition.z + offset);
	transform.setPosition(btPosition);

	Chunk* chunk = GetOwner()->GetComponent<Chunk>();
	const int gridSize = static_cast<int>(CHUNK_SIZE) + 1;
	const int m_upAxis = 1;

	float min, max;
	GetHeightFieldMinMax(heightMap, gridSize, min, max);

	min = 0.0f;
	max = TERRAIN_SCALE;

	rp::PhysicsCommon& common = Physics::Instance().GetCommon();
	rp::HeightFieldShape* shape = common.createHeightFieldShape(gridSize, gridSize, min, max, static_cast<void*>(heightMap), rp::HeightFieldShape::HeightDataType::HEIGHT_FLOAT_TYPE);
	
	
	rp::PhysicsWorld* world = Physics::Instance().GetWorld();
	rp::RigidBody* body = world->createRigidBody(transform);
	body->setType(rp::BodyType::STATIC);
	body->enableGravity(false);
	body->setUserData(static_cast<void*>(GetOwner()));

	rp::Transform colliderTransform;
	colliderTransform.setPosition(rp::Vector3(0, TERRAIN_SCALE / 2.0f, 0));

	rp::Collider* collider = body->addCollider(shape, colliderTransform);
	collider->setCollisionCategoryBits(static_cast<unsigned short>(FilterGroups::TERRAIN));
	collider->setCollideWithMaskBits(static_cast<unsigned short>(FilterGroups::EVERYTHING));

//
//	btHeightfieldTerrainShape* heightShape = new btHeightfieldTerrainShape(gridSize, gridSize, static_cast<void*>(heightMap), 1.0f, min, max, m_upAxis, PHY_FLOAT, true);
//	
//	heightShape->setUseDiamondSubdivision();
//	//heightShape->setFlipTriangleWinding(false);
//	heightShape->buildAccelerator();
//
//	const int group = static_cast<int>(FilterGroups::TERRAIN);
//	const int mask = static_cast<int>(FilterGroups::EVERYTHING & ~FilterGroups::TERRAIN);
//	Physics& physics = Physics::Instance();
//
//#if USE_RIGIDBODY
//
//	float mass = 0.0f;
//	btVector3 inertia(0, 0, 0);
//	
//	btDefaultMotionState* myMotionState = new btDefaultMotionState(transform);
//	btRigidBody::btRigidBodyConstructionInfo cInfo(mass, myMotionState, heightShape, inertia);
//	btRigidBody* body = new btRigidBody(cInfo);
//	body->setFriction(0.8f);
//	body->setHitFraction(0.8f);
//	body->setRestitution(0.6f);
//	body->setUserPointer(this);
//	body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_STATIC_OBJECT);
//	physics.GetWorld()->addRigidBody(body, group, mask);
//
//#endif
//#if !USE_RIGIDBODY
//
//	btCollisionObject* body = new btCollisionObject();
//	body->setCollisionShape(heightShape);
//	body->setWorldTransform(transform);
//
//	physics.GetWorld()->addCollisionObject(body, group, mask);
//#endif

}

float Chunk::SampleHeight(float x, float z)
{
	int col = (int)floorf(x);
	int row = (int)floorf(z);
	float height = 0.0f;

	if (row >= 0 && col >= 0 && row < CHUNK_SIZE && col < CHUNK_SIZE)
	{
		float bl = heightMap[col + CHUNK_SIZE * row];
		float br = heightMap[(col + 1) + CHUNK_SIZE * row];
		float tr = heightMap[(col + 1) + CHUNK_SIZE * (row + 1)];
		float tl = heightMap[col + CHUNK_SIZE * (row + 1)];

		float u = x - (float)col;
		float v = z - (float)row;
		height = Math::Lerp(Math::Lerp(bl, br, u), Math::Lerp(tl, tr, u), 1.0f - v);
	}

	std::cout << "col: " << col << ", row: " << row << ", height: " << height << std::endl;
	return height * TERRAIN_SCALE;
}

dx::XMVECTOR Chunk::IndexToWorld(const dx::XMINT2& index, float y)
{
	float x = static_cast<float>(index.x * (int)CHUNK_SIZE);// +((float)CHUNK_SIZE / 2.0f);
	float z = static_cast<float>(index.y * (int)CHUNK_SIZE);// - CHUNK_SIZE;// + ((float)CHUNK_SIZE / 2.0f);
	dx::XMVECTOR pos = { x,y,z };
	return pos;
}

dx::XMFLOAT2 Chunk::IndexToXZ(const dx::XMINT2& index)
{
	float x = (float)index.x * (float)CHUNK_SIZE;
	float y = (float)index.y * (float)CHUNK_SIZE;// -CHUNK_SIZE;

	return dx::XMFLOAT2(x, y);
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


//
//dx::XMINT2 Chunk::WorldToIndex(const dx::XMVECTOR& position)
//{
//	dx::XMFLOAT3 pos;
//	dx::XMStoreFloat3(&pos, position);
//
//	int x = static_cast<int>(floor(pos.x / CHUNK_SIZE));
//	int z = static_cast<int>(floor(pos.z / CHUNK_SIZE));
//	return dx::XMINT2(x, z);
//}
//
//dx::XMINT2 Chunk::WorldToIndex(const dx::XMFLOAT2& position)
//{
//	int x = static_cast<int>(floor(position.x / CHUNK_SIZE));
//	int z = static_cast<int>(floor(position.y / CHUNK_SIZE));
//	return dx::XMINT2(x, z);
//}
