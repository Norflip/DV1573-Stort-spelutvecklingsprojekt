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

void ObjectSpawner::Initialize(Object* root, ObjectPooler* pooler, Renderer* renderer)
{
	this->pooler = pooler;
	this->root = root;
	this->renderer = renderer;
}

void ObjectSpawner::Spawn(const SaveState& state, QuadTree& tree, std::unordered_map<int, Chunk*>& chunkMap, ID3D11Device* device)
{
	propSpawnPositions = CreateSpawnPositions(tree, 10.0f, chunkMap);
	itemSpawnPositions = CreateSpawnPositions(tree, 20.0f, chunkMap);

	struct TempData
	{
		Prop* prop;
		std::vector<dx::XMFLOAT4X4> instancedData;
		std::vector<dx::XMFLOAT3> positions;
	};

	std::unordered_map<Chunk*, TempData> data;

	size_t propIndex = 0;
	for (size_t i = 0; i < propSpawnPositions.size(); i++)
	{
		Prop& prop = instancedProps[propIndex];

		for (size_t j = 0; j < prop.queueCount && i < propSpawnPositions.size(); j++)
		{
			dx::XMFLOAT2 pos = propSpawnPositions[i];

			Chunk* chunk = GetChunk(pos.x, pos.y, chunkMap);
			auto find = data.find(chunk);
			if (find == data.end())
				data.insert({ chunk, TempData() });

			float y = chunk->SampleHeight(pos.x, pos.y) + prop.yOffset;
			dx::XMFLOAT3 position = dx::XMFLOAT3(pos.x, y, pos.y);

		//	Mesh::InstanceData singleInstancedData;
		//	dx::XMMATRIX translation = dx::XMMatrixTranslation(position.x, position.y, position.z);
	//		dx::XMStoreFloat4x4(&singleInstancedData.instanceWorld, dx::XMMatrixTranspose(translation));
//
			dx::XMFLOAT4X4 instancedData;
			dx::XMMATRIX translation = dx::XMMatrixTranslation(position.x, position.y, position.z);
			dx::XMStoreFloat4x4(&instancedData, dx::XMMatrixTranspose(translation));
		
			data[chunk].positions.push_back(position);
			data[chunk].instancedData.push_back(instancedData);
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
				//object->GetComponent<MeshComponent>()->SetBatchable(true);

				object->GetComponent<RigidBodyComponent>()->SetPosition(position);
				Transform::SetParentChild(root->GetTransform(), object->GetTransform());
				activeItems.push_back(object);

				itemIndex++;

				/* Particles */
				ParticleSystemComponent* particles = object->AddComponent<ParticleSystemComponent>(renderer, Engine::Instance->GetResources()->GetShaderResource("particleShader"));
				particles->InitializeParticles(renderer->GetDevice(), L"Textures/starstar.png");
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

void ObjectSpawner::SpawnSpecific(std::vector<dx::XMFLOAT2> positions, dx::XMVECTOR axis, std::vector<float> angles, std::string modelName, std::unordered_map<int, Chunk*>& chunkMap, std::function<void(Object*)> modifier)
{
	for (size_t i = 0; i < positions.size(); i++)
	{
		Chunk* chunk = GetChunk(positions[i].x, positions[i].y, chunkMap);

		if (chunk != nullptr)
		{
			Object* obj = Engine::Instance->GetResources()->AssembleObject(modelName, modelName + "Material");
			Transform::SetParentChild(chunk->GetOwner()->GetTransform(), obj->GetTransform());
			
			float height = chunk->SampleHeight(positions[i].x, positions[i].y);
			obj->GetTransform().SetWorldPosition({ positions[i].x, height, positions[i].y });
			obj->GetTransform().SetWorldRotation(dx::XMQuaternionRotationNormal(axis, angles[i]));
			
			modifier(obj);
		}
	}
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

void ObjectSpawner::RegisterInstancedItem(Mesh* mesh, Material* material, float radius, float padding, float yOffset, size_t queueCount, dx::XMUINT3 rotationAxis)
{
	assert(rotationAxis.x == 0 || rotationAxis.x == 1);
	assert(rotationAxis.y == 0 || rotationAxis.y == 1);
	assert(rotationAxis.z == 0 || rotationAxis.z == 1);
	assert(mesh != nullptr);
	assert(material != nullptr);
	assert(queueCount > 0);

	Prop prop;
	prop.mesh = mesh;
	prop.material = material;
	prop.yOffset = yOffset;
	prop.radius = radius;
	prop.padding = padding;
	prop.queueCount = queueCount;
	prop.randomRotationAxis = rotationAxis;
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

std::vector<dx::XMFLOAT2> ObjectSpawner::CreateSpawnPositions(QuadTree& tree, float radius, std::unordered_map<int, Chunk*>& chunkMap)
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

		if (tree.CountInRange(point, radius) == 0)
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
