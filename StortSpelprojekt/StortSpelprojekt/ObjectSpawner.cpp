#include "ObjectSpawner.h"
#include "ResourceManager.h"

ObjectSpawner::ObjectSpawner()
{
}

ObjectSpawner::~ObjectSpawner()
{
}

void ObjectSpawner::Initialize(Object* root, ObjectPooler* pooler)
{
	this->pooler = pooler;
	this->root = root;
}

void ObjectSpawner::Spawn(PointQuadTree& tree, std::unordered_map<int, Chunk*>& chunkMap, std::vector<Chunk*>& chunks)
{
	itemSpawnPositions = CreateSpawnPositions(tree, 20.0f, chunkMap);
	propSpawnPositions = CreateSpawnPositions(tree, 20.0f, chunkMap);

	/*for (size_t i = 0; i < staticItems.size(); i++)
	{
		dx::XMFLOAT2 pos = itemSpawnPositions[0];
		Chunk* chunk = GetChunk(pos.x, pos.y, chunkMap);

		if (chunk != nullptr)
		{
			float y = chunk->SampleHeight(pos.x, pos.y);

			Object* object = pooler->GetItem("dynamic_stone");
			dx::XMVECTOR position = dx::XMVectorSet(pos.x, y + SPAWN_HEIGHT, pos.y, 0.0f);

			object->GetComponent<RigidBodyComponent>()->SetPosition(position);
			Transform::SetParentChild(root->GetTransform(), object->GetTransform());
			items.push_back(object);
		}
	}*/


	for (int i = itemSpawnPositions.size() - 1; i >= 0; i--)
	{
		dx::XMFLOAT2 pos = itemSpawnPositions[i];
		Chunk* chunk = GetChunk(pos.x, pos.y, chunkMap);

		if (chunk != nullptr)
		{
			float y = chunk->SampleHeight(pos.x, pos.y);

			Object* object = pooler->GetItem("dynamic_stone");
			dx::XMVECTOR position = dx::XMVectorSet(pos.x, y + SPAWN_HEIGHT, pos.y, 0.0f);

			object->GetComponent<RigidBodyComponent>()->SetPosition(position);
			Transform::SetParentChild(root->GetTransform(), object->GetTransform());
			items.push_back(object);
		}
	}

	for (int i = propSpawnPositions.size() - 1; i >= 0; i--)
	{
		dx::XMFLOAT2 pos = propSpawnPositions[i];
		Chunk* chunk = GetChunk(pos.x, pos.y, chunkMap);

		if (chunk != nullptr)
		{
			float y = chunk->SampleHeight(pos.x, pos.y);

			Object* object = pooler->GetItem("static_sphere");
			dx::XMVECTOR position = dx::XMVectorSet(pos.x, y, pos.y, 0.0f);

			object->GetComponent<RigidBodyComponent>()->SetPosition(position);
			Transform::SetParentChild(root->GetTransform(), object->GetTransform());
			items.push_back(object);
		}
	}
}

void ObjectSpawner::Despawn()
{
	for (auto i : items)
	{
		Object* obj = i;
		Transform::ClearFromHierarchy(obj->GetTransform());
		obj->GetComponent<RigidBodyComponent>()->PhysicRelease();
		delete obj;
	}

	items.clear();
}

void ObjectSpawner::RegisterItem(std::string key, float radius, float padding, float yOffset, int queueCount, ItemSpawnType type)
{
	Item item;
	item.key = key;
	item.yOffset;
	item.radius = radius;
	item.padding = padding;

	for (size_t i = 0; i < queueCount; i++)
	{
		switch (type)
		{
			case ItemSpawnType::DYNAMIC:
				dynamicItems.push_back(item); break;

			default:
			case ItemSpawnType::STATIC: 
				staticItems.push_back(item); break;
		}
	}

}


void ObjectSpawner::DrawDebug()
{
	for (size_t i = 0; i < itemSpawnPositions.size(); i++)
	{
		dx::XMFLOAT3 worldPoint(itemSpawnPositions[i].x, 5, itemSpawnPositions[i].y);
		DShape::DrawSphere(worldPoint, 0.5f, dx::XMFLOAT3(1, 1, 0));
	}

	for (size_t i = 0; i < propSpawnPositions.size(); i++)
	{
		dx::XMFLOAT3 worldPoint(propSpawnPositions[i].x, 5, propSpawnPositions[i].y);
		DShape::DrawSphere(worldPoint, 0.5f, dx::XMFLOAT3(0, 0, 1));
	}

}

std::vector<dx::XMFLOAT2> ObjectSpawner::CreateSpawnPositions(PointQuadTree& tree, float radius, std::unordered_map<int, Chunk*>& chunkMap)
{
	dx::XMFLOAT2 min = tree.GetMin();
	dx::XMFLOAT2 max = tree.GetMax();

	PossionDiscSampler sampler;
	std::vector<dx::XMFLOAT2> points = sampler.GeneratePoints(radius, dx::XMFLOAT2(max.x - min.x, max.y - min.y), 5);
	std::vector<dx::XMFLOAT2> validPoints;
	size_t fails = 0;

	for (size_t i = 0; i < points.size(); i++)
	{
		dx::XMFLOAT2 point = points[i];
		point.x += (min.x - max.x) / 2.0f;
		point.y += (min.y - max.y) / 2.0f;

		if (tree.GetInRadius(point, 1.0f).size() == 0)
		{
			Chunk* chunk = GetChunk(point.x, point.y, chunkMap);

			if (chunk != nullptr && chunk->GetType() == ChunkType::TERRAIN)
			{
				tree.Insert(point);
				validPoints.push_back(point);
			}
			else
			{
				fails++;
			}
		}
	}

	std::shuffle(std::begin(validPoints), std::end(validPoints), Random::m_rngEngine);
	return validPoints;
}

Chunk* ObjectSpawner::GetChunk(float x, float z, std::unordered_map<int, Chunk*>& chunkMap) const
{
	dx::XMINT2 index = Chunk::WorldToIndex(x, z);
	int hash = HASH2D_I(index.x, index.y);

	Chunk* chunk = nullptr;
	auto find = chunkMap.find(hash);

	if (find != chunkMap.end())
		chunk = find->second;
	return chunk;
}
