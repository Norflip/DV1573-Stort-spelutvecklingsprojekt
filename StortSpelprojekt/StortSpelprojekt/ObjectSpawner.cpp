#include "stdafx.h"
#include "ObjectSpawner.h"
#include "ResourceManager.h"
#include "Engine.h"
#include <random>
#include "MeshCollider.h"

ObjectSpawner::ObjectSpawner()
{
}

ObjectSpawner::~ObjectSpawner()
{
}

void ObjectSpawner::Initialize(Object* root, ObjectPooler* pooler, Renderer* renderer, ResourceManager* resource, CameraComponent* camera)
{
	this->pooler = pooler;
	this->root = root;
	this->renderer = renderer;
	this->resources = resource;
	this->camera = camera;
}

void ObjectSpawner::Spawn(const SaveState& state, PointQuadTree& tree, std::unordered_map<int, Chunk*>& chunkMap, std::vector<Chunk*>& chunks, ID3D11Device* device)
{
	Random::SetSeed(state.GetSeed(0));
	propSpawnPositions = CreateSpawnPositions(tree, 10.0f, chunkMap);

	Random::SetSeed(state.GetSeed(1));
	itemSpawnPositions = CreateSpawnPositions(tree, 20.0f, chunkMap);

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

	//int propIndex = 0;
	//for (int i = propSpawnPositions.size() - 1; i >= 0; i--)
	//{
	//	propIndex %= staticItems.size();
	//	Item& item = staticItems[propIndex];

	//	dx::XMFLOAT2 pos = propSpawnPositions[i];
	//	Chunk* chunk = GetChunk(pos.x, pos.y, chunkMap);

	//	if (chunk != nullptr)
	//	{
	//		float y = chunk->SampleHeight(pos.x, pos.y) + item.yOffset;

	//		//	dx::XMMATRIX invWorld = dx::XMMatrixInverse(nullptr, chunk->GetOwner()->GetTransform().GetWorldMatrix());

	//		Object* object = pooler->GetItem(item.key);
	//		dx::XMVECTOR position = dx::XMVectorSet(pos.x, y, pos.y, 0.0f);
	//		//	dx::XMVECTOR pos = dx::XMVector3Transform(position, invWorld);

	//		object->GetTransform().SetWorldPosition(position);


	//		object->GetComponent<RigidBodyComponent>()->SetPosition(position);

	//		// k�r med chunk ist�llet f�r root d� dom �nd� inte kan flyttas.
	//		//Transform::SetParentChild(root->GetTransform(), object->GetTransform());

	//		// kr�ver inverse av parent world matrix
	//		Transform::SetParentChild(chunk->GetOwner()->GetTransform(), object->GetTransform());

	//		items.push_back(object);
	//		propIndex++;
	//	}
	//}

	struct TempData
	{
		InstancedProp* prop;
	//	Chunk* chunk;
		std::vector<Mesh::InstanceData> instancedData;
		std::vector<dx::XMFLOAT3> positions;
		// ROTATIONS LATER
	};

	std::unordered_map<Chunk*, TempData> data;

	size_t propIndex = 0;
	for (size_t i = 0; i < propSpawnPositions.size(); i++)
	{
		InstancedProp& prop = instancedProps[propIndex];

		for (size_t j = 0; j < prop.queueCount && i < propSpawnPositions.size(); j++)
		{
			dx::XMFLOAT2 pos = propSpawnPositions[i];

			Chunk* chunk = GetChunk(pos.x, pos.y, chunkMap);
			auto find = data.find(chunk);
			if (find == data.end())
				data.insert({ chunk, TempData() });

			float y = chunk->SampleHeight(pos.x, pos.y) + prop.yOffset;
			dx::XMFLOAT3 position = dx::XMFLOAT3(pos.x, y, pos.y);

			Mesh::InstanceData singleInstancedData;
			dx::XMMATRIX translation = dx::XMMatrixTranslation(position.x, position.y, position.z);
			dx::XMStoreFloat4x4(&singleInstancedData.instanceWorld, dx::XMMatrixTranspose(translation));
			singleInstancedData.instancePosition = position;

			data[chunk].positions.push_back(position);
			data[chunk].instancedData.push_back(singleInstancedData);
			data[chunk].prop = &prop;
		}

		propIndex++;
		propIndex %= instancedProps.size();

		i += (prop.queueCount - 1);
	}




	size_t index = 0;
	for (auto i : data)
	{
		Object* props = new Object("props_" + std::to_string(index), ObjectFlag::DEFAULT /* | ObjectFlag::NO_CULL */);
		MeshComponent* mc = props->AddComponent<MeshComponent>(i.second.prop->mesh, i.second.prop->material);
		mc->SetInstanceable(0, i.second.instancedData, i.second.instancedData.size(), device);

		props->AddComponent<MeshCollider>(i.second.prop->mesh, i.second.positions);
		props->AddComponent<RigidBodyComponent>(0.f, FilterGroups::PROPS, FilterGroups::EVERYTHING, BodyType::STATIC, true);
		
		Transform::SetParentChild(i.first->GetOwner()->GetTransform(), props->GetTransform());

		

	}





#if SPAWN_ITEMS
	int itemIndex = 0;
	if (items.size() > 0)
	{
		for (size_t i = 0; i < itemSpawnPositions.size(); i++)
		{
			itemIndex %= items.size();
			Item& item = items[itemIndex];

			dx::XMFLOAT2 pos = itemSpawnPositions[i];
			Chunk* chunk = GetChunk(pos.x, pos.y, chunkMap);

			if (chunk != nullptr)
			{
				float y = chunk->SampleHeight(pos.x, pos.y);

				Object* object = pooler->GetItem(item.key);
				dx::XMVECTOR position = dx::XMVectorSet(pos.x, y + SPAWN_HEIGHT, pos.y, 0.0f);

				object->GetComponent<RigidBodyComponent>()->SetPosition(position);
				Transform::SetParentChild(root->GetTransform(), object->GetTransform());
				activeItems.push_back(object);
								
				itemIndex++;

				/* Particles */
				object->AddComponent<ParticleSystemComponent>(renderer, camera, resources->GetShaderResource("particleShader"));
				object->GetComponent<ParticleSystemComponent>()->InitializeParticles(renderer->GetDevice(), L"Textures/starstar.png");									
			}
		}
	}
		

	std::cout << "ITEMS: " << itemSpawnPositions.size() << std::endl;
#endif

	std::cout << "PROPS: " << propSpawnPositions.size() << std::endl;
}

void ObjectSpawner::Despawn()
{
	if (activeItems.size() > 0)
	{
		for (auto i : activeItems)
		{
			Object* obj = i;
			obj->GetComponent<RigidBodyComponent>()->Release();			
			Transform::ClearFromHierarchy(obj->GetTransform());
			delete obj;
		}
	}
		
	activeItems.clear();
}

void ObjectSpawner::RegisterItem(std::string key, float radius, float padding, float yOffset, size_t queueCount)
{
	Item item;
	item.key = key;
	item.yOffset = yOffset;
	item.radius = radius;
	item.padding = padding;

	for (size_t i = 0; i < queueCount; i++)
		items.push_back(item);
}

void ObjectSpawner::RegisterInstancedItem(Mesh* mesh, Material* material, float radius, float padding, float yOffset, size_t queueCount)
{
	assert(mesh != nullptr);
	assert(material != nullptr);
	assert(queueCount > 0);

	InstancedProp prop;
	prop.mesh = mesh;
	prop.material = material;
	prop.yOffset = yOffset;
	prop.radius = radius;
	prop.padding = padding;
	prop.queueCount = queueCount;

	instancedProps.push_back(prop);
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
	std::vector<dx::XMFLOAT2> points = sampler.GeneratePoints(radius, dx::XMFLOAT2(max.x - min.x, max.y - min.y), false, 5);
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
				//tree.Insert(point);
				validPoints.push_back(point);
			}
			else
			{
				fails++;
			}
		}
	}

	std::random_shuffle(std::begin(validPoints), std::end(validPoints));
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
