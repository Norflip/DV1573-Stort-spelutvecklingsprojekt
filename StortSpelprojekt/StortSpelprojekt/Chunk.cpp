#include "stdafx.h"
#include "Chunk.h"
#include <iostream>
#include "Physics.h"
#include "Engine.h"
#include <algorithm>

Chunk::Chunk(dx::XMINT2 index, ChunkType type) : index(index), type(type)
{

}

Chunk::~Chunk()
{
}

void Chunk::Update(const float& deltaTime)
{
#if _DEBUG and DRAW_DEBUG
	dx::XMFLOAT3 color = DSHAPE_CHUNK_COLORS[static_cast<int>(this->type)];

	dx::XMFLOAT3 center;
	dx::XMStoreFloat3(&center, GetOwner()->GetTransform().GetPosition());
	center.x += (float)CHUNK_SIZE / 2.0f;
	center.z += (float)CHUNK_SIZE / 2.0f;

	//DShape::DrawWireBox(center, dx::XMFLOAT3(CHUNK_SIZE * 0.9f, 1.0f /*CHUNK_SIZE * 0.9f*/, CHUNK_SIZE * 0.9f), color);


	rp::AABB aabb = collider->getWorldAABB();
	dx::XMFLOAT3 pos (aabb.getCenter().x, aabb.getCenter().y, aabb.getCenter().z);
	dx::XMFLOAT3 ext (aabb.getExtent().x * 0.95f, aabb.getExtent().y * 0.95f, aabb.getExtent().z * 0.95f);

	DShape::DrawWireBox(pos, ext, dx::XMFLOAT3(1.0f, 1.0f, 0.0f));

	for (size_t y = 0; y < CHUNK_SIZE + 1; y++)
	{
		for (size_t x = 0; x < CHUNK_SIZE + 1; x++)
		{
			float dy = shape->getHeightAt(x, y);
			pos = dx::XMFLOAT3(x + center.x - (CHUNK_SIZE / 2.0f), dy, y + center.z - (CHUNK_SIZE / 2.0f));

			DShape::DrawWireSphere(pos, 0.1f, dx::XMFLOAT3(1.0f, 1.0f, 0.0f));
		}
	}
#endif
}

void Chunk::Create(const WorldDescription& description, const Path& path, ID3D11Device* device)
{
	CreateChunkData(description, path, device);
	SetupCollisionObject();
}

float Chunk::SampleInfluence(const float& x, const float& z) const
{
	int col, row;
	float influence = -1.0f;
	if (TryGetLocalColRow(x, z, col, row))
		influence = data.influenceMap[col + row * (CHUNK_SIZE + 1)];
	
	return influence;
}

dx::XMFLOAT3 Chunk::SampleNormal(const float& x, const float& z) const
{
	float hLeft = SampleHeight(x - 1, z);
	float hRight = SampleHeight(x + 1, z);
	float hDown = SampleHeight(x, z - 1);
	float hUp = SampleHeight(x, z + 1);

	dx::XMFLOAT3 normal;
	dx::XMVECTOR horizontal = dx::XMVector3Normalize({ 1.0f, 0.0f, hLeft - hRight });
	dx::XMVECTOR vertical = dx::XMVector3Normalize({ 0.0f, 1.0f, hUp - hDown });
	dx::XMStoreFloat3(&normal, dx::XMVector3Cross(horizontal, vertical));

	float tmp = normal.y;
	normal.y = normal.z;
	normal.z = tmp;

	return normal;
}

bool Chunk::TryGetLocalColRow(const float& x, const float& z, int& col, int& row) const
{
	bool inside = false;

	dx::XMFLOAT3 position;
	dx::XMStoreFloat3(&position, GetOwner()->GetTransform().GetPosition());
	dx::XMFLOAT2 chunkPosition = IndexToWorldXZ(WorldToIndex(x, z));

	col = static_cast<int>(floorf(x - chunkPosition.x));
	row = static_cast<int>(floorf(z - chunkPosition.y));

	return (col >= 0 && row >= 0 && col <= CHUNK_SIZE && row <= CHUNK_SIZE);
}

float Chunk::SampleHeight(const float& x, const float& z) const
{
	int col, row;
	float height = -1.0f;
	if (TryGetLocalColRow(x,z, col, row))
		height = shape->getHeightAt(col, row);

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

int Chunk::WorldToIndexHash(float x, float z)
{
	dx::XMINT2 index = WorldToIndex(x, z);
	return HASH2D_I(index.x, index.y);
}

void Chunk::PhysicRelease()
{
	Physics* physics = Engine::Instance->GetPhysics();
	rp::PhysicsWorld* world = physics->GetWorld();
	body->removeCollider(collider);
	world->destroyRigidBody(body);
	physics->GetCommon().destroyHeightFieldShape(shape);

}

void Chunk::SetupCollisionObject()
{
	Physics* physics = Engine::Instance->GetPhysics();

	dx::XMFLOAT3 worldPosition;
	dx::XMStoreFloat3(&worldPosition, GetOwner()->GetTransform().GetPosition());

	const int gridSize = static_cast<int>(CHUNK_SIZE) + 1;

	min = 0.1f;// 0.1f;
	max = TERRAIN_SCALE;

	rp::PhysicsCommon& common = physics->GetCommon();
	shape = common.createHeightFieldShape(gridSize, gridSize, min, max, static_cast<void*>(data.heightMap), rp::HeightFieldShape::HeightDataType::HEIGHT_FLOAT_TYPE);

	float origin = (max - min) * 0.5f;
	const float offset = CHUNK_SIZE / 2.0f;

	rp::Transform transform;
	rp::Vector3 btPosition(worldPosition.x + offset, origin, worldPosition.z + offset);
	transform.setPosition(btPosition);

	body = physics->GetWorld()->createRigidBody(transform);
	body->setType(rp::BodyType::STATIC);
	body->enableGravity(false);

	collider = body->addCollider(shape, rp::Transform::identity());

	body->setUserData(static_cast<void*>(GetOwner()));
	body->setIsActive(true);
	collider->setCollisionCategoryBits(static_cast<unsigned short>(FilterGroups::TERRAIN));
	collider->setCollideWithMaskBits(static_cast<unsigned short>(FilterGroups::EVERYTHING));
	collider->getMaterial().setBounciness(0.f);
	collider->getMaterial().setFrictionCoefficient(10.f);
}

void Chunk::CreateChunkData(const WorldDescription& description, const Path& path, ID3D11Device* device)
{
	const size_t size = CHUNK_SIZE + 1;
	dx::XMFLOAT2 chunkPosXZ = Chunk::IndexToWorldXZ(index);

	const float MIN_TERRAIN_HEIGHT = 0.3f;

	unsigned char* buffer = new unsigned char[size * size * 4];
	data.heightMap = new float[size * size];
	data.influenceMap = new float[size * size];

	for (size_t y = 0; y < size; y++)
	{
		for (size_t x = 0; x < size; x++)
		{
			dx::XMFLOAT2 tilePos = dx::XMFLOAT2(chunkPosXZ.x + static_cast<float>(x), chunkPosXZ.y + static_cast<float>(y));

			float influence = path.SampleInfluence(tilePos);
			//distance = std::min(distance, MAX_DISTANCE) / MAX_DISTANCE;



			float sampledHeight = Noise::Sample(chunkPosXZ.x + x, chunkPosXZ.y + y, description.noiseSettings);
			float worldHeight = sampledHeight * influence * TERRAIN_SCALE;
			worldHeight = std::max(worldHeight, MIN_TERRAIN_HEIGHT);
			float height = worldHeight / TERRAIN_SCALE;

			int bufferIndex = x + size * y;

			data.heightMap[bufferIndex] = worldHeight;
			data.influenceMap[bufferIndex] = influence;


			buffer[bufferIndex * 4 + 0] = static_cast<unsigned char>(255 * height);
			buffer[bufferIndex * 4 + 1] = static_cast<unsigned char>(255 * sampledHeight);
			buffer[bufferIndex * 4 + 2] = static_cast<unsigned char>(255 * influence);
			buffer[bufferIndex * 4 + 3] = static_cast<unsigned char>(255);
		}
	}

	if (type == ChunkType::PUZZEL)
	{

		const int SMOOTH_IT = 64;
		const int SAMPLE_SIZE = 2;

		for (size_t i = 0; i < SMOOTH_IT; i++)
		{
			for (size_t y = SAMPLE_SIZE; y < size - SAMPLE_SIZE; y++)
			{
				for (size_t x = SAMPLE_SIZE; x < size - SAMPLE_SIZE; x++)
				{
					float average = 0.0f;
					int samples = 0;

					for (int dy = -SAMPLE_SIZE; dy <= SAMPLE_SIZE; dy++)
					{
						for (int dx = -SAMPLE_SIZE; dx <= SAMPLE_SIZE; dx++)
						{
							average += data.heightMap[(x + dx) + size * (y + dy)];
							samples++;
						}
					}

					average /= samples;
					float height01 = (average / TERRAIN_SCALE);
					int bufferIndex = x + size * y;
					data.heightMap[bufferIndex] = average;
					buffer[bufferIndex * 4 + 0] = static_cast<unsigned char>(255 * height01);
				}
			}
		}

		


		/*float average = 0.0f;


		for (size_t y = 0; y < size; y++)
		{
			for (size_t x = 0; x < size; x++)
			{
				average += data.heightMap[x + size * y];
			}
		}

		int smoothSize = 3;

		average /= ((size - (smoothSize * 2)) * (size - (smoothSize * 2)));

		for (size_t y = smoothSize; y < size - smoothSize; y++)
		{
			for (size_t x = smoothSize; x < size - smoothSize; x++)
			{
				float worldHeight = average;
				float height = worldHeight / TERRAIN_SCALE;

				int bufferIndex = x + size * y;

				data.heightMap[x + size * y] = worldHeight;

				buffer[bufferIndex * 4 + 0] = static_cast<unsigned char>(255 * height);
			}
		}*/

	}

	data.dataTexture = Texture::CreateFromBuffer(buffer, CHUNK_SIZE + 1, CHUNK_SIZE + 1, 4, DXGI_FORMAT_R8G8B8A8_UNORM, device);
}