#include "stdafx.h"
#include "World.h"
#include "Input.h"
#include "PickupComponent.h"
#include "Engine.h"

World::World()
{
}

World::~World()
{
}

void World::Initialize(Object* root, ResourceManager* resources, Renderer* renderer)
{
	this->resources = resources;
	this->spawner = new ObjectSpawner();
	this->pooler = new ObjectPooler(resources);
	
	spawner->Initialize(root, pooler, renderer);

	generator.Initialize(root, resources, spawner, renderer->GetDevice(), renderer->GetContext());
	lastRelevantIndex = dx::XMINT2(-5000, -5000);
}

void World::ConstructSegment(const SaveState& state)
{
	SegmentDescription description(0, 10, 2);
	description.directionalSteps = 1;
	description.maxSteps = 10;

	RegisterToPool(pooler, spawner, description.queueModifier);


	Random::SetSeed(state.GetSegmentedSeed());
	generator.Construct(state, description);
}

void World::DeconstructSegment()
{
	relevant.clear();
	generator.Deconstruct();
}

void World::UpdateRelevantChunks(const Transform& transform, CameraComponent* camera)
{
	const float offset = 0.1f;

	// check player index
	// if not the same as the current one, loop relevant and enable other chunks
	if (generator.IsConstructed())
	{
		dx::XMFLOAT3 position;
		dx::XMStoreFloat3(&position, transform.GetWorldPosition());
		dx::XMINT2 newIndex = Chunk::WorldToIndex(position.x, position.z);

		if (lastRelevantIndex.x != newIndex.x || lastRelevantIndex.y != newIndex.y)
		{
			lastRelevantIndex = newIndex;

			for (auto i : relevant)
				i->GetOwner()->RemoveFlag(ObjectFlag::ENABLED);

			relevant.clear();
			GetChunksInRadius(lastRelevantIndex, RELEVANT_RADIUS, relevant);

			std::cout << "CHUNKS: " << relevant.size() << std::endl;

			for (auto i : relevant)
			{
				i->GetOwner()->AddFlag(ObjectFlag::ENABLED);
			}		
		}

		/*for (auto i : relevant)
		{
			Object* obj = i->GetOwner();
			Bounds bounds = obj->GetComponent<MeshComponent>()->GetBounds();
			bool inView = camera->InView(bounds, obj->GetTransform().GetWorldMatrix());

			if (inView)
			{
				obj->RemoveFlag(ObjectFlag::ENABLED);
			}
			else
			{
				obj->AddFlag(ObjectFlag::ENABLED);
			}
		}*/
	}
}

void World::DrawDebug()
{
	generator.DrawDebug();
}

Chunk* World::GetChunk(const float& x, const float& z) const
{
	dx::XMINT2 index = Chunk::WorldToIndex(x, z);
	int hash = HASH2D_I(index.x, index.y);
	Chunk* chunk = nullptr;

	auto find = generator.GetChunkMap().find(hash);

	if (find != generator.GetChunkMap().end())
		chunk = find->second;

	return chunk;
}

float World::SampleHeight(const float& x, const float& z) const
{
	Chunk* chunk = GetChunk(x, z);
	return (chunk) ? chunk->SampleHeight(x, z) : -1;
}

float World::SampleRoadInfluence(const float& x, const float& z) const
{
	Chunk* chunk = GetChunk(x, z);
	return (chunk) ? chunk->SampleInfluence(x, z) : -1;
}

void World::SampleNormal(const float& x, const float& z, dx::XMFLOAT3& normal) const
{
	float hLeft = SampleHeight(x - 1, z);
	float hRight = SampleHeight(x + 1, z);
	float hDown = SampleHeight(x, z - 1);
	float hUp = SampleHeight(x, z + 1);

	dx::XMVECTOR horizontal = dx::XMVector3Normalize({ 1.0f, 0.0f, hLeft - hRight });
	dx::XMVECTOR vertical = dx::XMVector3Normalize({ 0.0f, 1.0f, hUp - hDown });
	dx::XMStoreFloat3(&normal, dx::XMVector3Cross(horizontal, vertical));
}

void World::GetChunksInRadius(const dx::XMINT2& index, int radius, std::vector<Chunk*>& chunks) const
{
	if (generator.IsConstructed())
	{
		for (int y = -radius; y <= radius; y++)
		{
			for (int x = -radius; x <= radius; x++)
			{
				int i = HASH2D_I(index.x + x, index.y + y);
				auto find = generator.GetChunkMap().find(i);
				if (find != generator.GetChunkMap().end())
					chunks.push_back((*find).second);
			}
		}
	}
}

dx::XMINT2 World::GetChunkIndex(Object* object) const
{
	dx::XMINT2 index(0, 0);

	if (object != nullptr)
	{
		dx::XMFLOAT3 position;
		dx::XMStoreFloat3(&position, object->GetTransform().GetPosition());

		index.x = static_cast<int>(floorf(position.x / CHUNK_SIZE));
		index.y = static_cast<int>(floorf(position.z / CHUNK_SIZE));
	}
	return index;
}

int World::TryGetQueueCount(std::string key, const std::map<std::string, int>& queueCountTable, int defaultCount) const
{
	auto found = queueCountTable.find(key);
	return(found != queueCountTable.end()) ? found->second : defaultCount;
}

void World::RegisterFood(ObjectPooler* pooler, ObjectSpawner* spawner, const std::map<std::string, int>& queueCountTable) const
{
	pooler->Register("Baked_beans", 0, [](ResourceManager* resources)
		{
			Object* object = resources->AssembleObject("Soup", "SoupMaterial");

			object->AddComponent<BoxColliderComponent>(dx::XMFLOAT3(0.5f, 0.5f, 0.5f), dx::XMFLOAT3(0, 0, 0));
			object->AddComponent<PickupComponent>(Type::Food, 20.0f);
			object->AddComponent<RigidBodyComponent>(10.0f, FilterGroups::PICKUPS, FilterGroups::EVERYTHING, BodyType::KINEMATIC, true);

			//Physics::Instance().RegisterRigidBody(rd);
			return object;
		});

	pooler->Register("Banana", 0, [](ResourceManager* resources)
		{
			Object* object = resources->AssembleObject("Banana", "BananaMaterial");

			object->AddComponent<BoxColliderComponent>(dx::XMFLOAT3(0.5f, 0.5f, 0.5f), dx::XMFLOAT3(0, 0, 0));
			object->AddComponent<PickupComponent>(Type::Food, 10.0f);
			object->AddComponent<RigidBodyComponent>(10.0f, FilterGroups::PICKUPS, FilterGroups::EVERYTHING, BodyType::KINEMATIC, true);

			//Physics::Instance().RegisterRigidBody(rd);
			return object;
		});

	pooler->Register("Apple", 0, [](ResourceManager* resources)
		{
			Object* object = resources->AssembleObject("Apple", "AppleMaterial");

			object->AddComponent<BoxColliderComponent>(dx::XMFLOAT3(0.5f, 0.5f, 0.5f), dx::XMFLOAT3(0, 0, 0));
			object->AddComponent<PickupComponent>(Type::Food, 10.0f);
			object->AddComponent<RigidBodyComponent>(10.0f, FilterGroups::PICKUPS, FilterGroups::EVERYTHING, BodyType::KINEMATIC, true);

			//Physics::Instance().RegisterRigidBody(rd);
			return object;
		});

	pooler->Register("Fruits", 0, [](ResourceManager* resources)
		{
			Object* object = resources->AssembleObject("Fruits", "FruitsMaterial");

			object->AddComponent<BoxColliderComponent>(dx::XMFLOAT3(0.5f, 0.5f, 0.5f), dx::XMFLOAT3(0, 0, 0));
			object->AddComponent<PickupComponent>(Type::Food, 35.0f);
			object->AddComponent<RigidBodyComponent>(10.0f, FilterGroups::PICKUPS, FilterGroups::EVERYTHING, BodyType::KINEMATIC, true);

			//Physics::Instance().RegisterRigidBody(rd);
			return object;
		});

	pooler->Register("Pear", 0, [](ResourceManager* resources)
		{
			Object* object = resources->AssembleObject("Pear", "PearMaterial");

			object->AddComponent<BoxColliderComponent>(dx::XMFLOAT3(0.5f, 0.5f, 0.5f), dx::XMFLOAT3(0, 0, 0));
			object->AddComponent<PickupComponent>(Type::Food, 35.0f);
			object->AddComponent<RigidBodyComponent>(10.0f, FilterGroups::PICKUPS, FilterGroups::EVERYTHING, BodyType::KINEMATIC, true);

			//Physics::Instance().RegisterRigidBody(rd);
			return object;
		});

	spawner->RegisterItem("Baked_beans", 1.0f, 1.0f, 0.0f, TryGetQueueCount("Baked_beans", queueCountTable));
	spawner->RegisterItem("Banana", 1.0f, 1.0f, 0.0f, TryGetQueueCount("Banana", queueCountTable));
	spawner->RegisterItem("Apple", 1.0f, 1.0f, 0.0f, TryGetQueueCount("Apple", queueCountTable));
	spawner->RegisterItem("Fruits", 1.0f, 1.0f, 0.0f, TryGetQueueCount("Fruits", queueCountTable));
	spawner->RegisterItem("Pear", 1.0f, 1.0f, 0.0f, TryGetQueueCount("Pear", queueCountTable));
}

void World::RegisterFuel(ObjectPooler* pooler, ObjectSpawner* spawner, const std::map<std::string, int>& queueCountTable) const
{
	pooler->Register("FuelGreen", 0, [](ResourceManager* resources)
		{
			Object* object = resources->AssembleObject("FuelCanGreen", "FuelCanGreenMaterial");

			object->AddComponent<BoxColliderComponent>(dx::XMFLOAT3(0.5f, 0.5f, 0.5f), dx::XMFLOAT3(0, 0, 0));
			object->AddComponent<PickupComponent>(Type::Fuel, 20.0f);
			object->AddComponent<RigidBodyComponent>(10.0f, FilterGroups::HOLDABLE, FilterGroups::EVERYTHING & ~FilterGroups::PLAYER, BodyType::KINEMATIC, true);

			//Physics::Instance().RegisterRigidBody(rd);
			return object;
		});

	pooler->Register("FuelBlue", 0, [](ResourceManager* resources)
		{
			Object* object = resources->AssembleObject("FuelCanBlue", "FuelCanBlueMaterial");

			object->AddComponent<BoxColliderComponent>(dx::XMFLOAT3(0.5f, 0.5f, 0.5f), dx::XMFLOAT3(0, 0, 0));
			object->AddComponent<PickupComponent>(Type::Fuel, 20.0f);
			object->AddComponent<RigidBodyComponent>(10.0f, FilterGroups::HOLDABLE, FilterGroups::EVERYTHING & ~FilterGroups::PLAYER, BodyType::KINEMATIC, true);

			//Physics::Instance().RegisterRigidBody(rd);
			return object;
		});

	pooler->Register("FuelRed", 0, [](ResourceManager* resources)
		{
			Object* object = resources->AssembleObject("FuelCanRed", "FuelCanRedMaterial");

			object->AddComponent<BoxColliderComponent>(dx::XMFLOAT3(0.5f, 0.5f, 0.5f), dx::XMFLOAT3(0, 0, 0));
			object->AddComponent<PickupComponent>(Type::Fuel, 20.0f);
			object->AddComponent<RigidBodyComponent>(10.0f, FilterGroups::HOLDABLE, FilterGroups::EVERYTHING & ~FilterGroups::PLAYER, BodyType::KINEMATIC, true);

			//Physics::Instance().RegisterRigidBody(rd);
			return object;
		});

	pooler->Register("Propane", 0, [](ResourceManager* resources)
		{
			Object* object = resources->AssembleObject("Propane", "PropaneMaterial");

			object->AddComponent<BoxColliderComponent>(dx::XMFLOAT3(0.5f, 0.5f, 0.5f), dx::XMFLOAT3(0, 0, 0));
			object->AddComponent<PickupComponent>(Type::Fuel, 35.0f);
			object->AddComponent<RigidBodyComponent>(10.0f, FilterGroups::HOLDABLE, FilterGroups::EVERYTHING & ~FilterGroups::PLAYER, BodyType::KINEMATIC, true);

			//Physics::Instance().RegisterRigidBody(rd);
			return object;
		});

	spawner->RegisterItem("FuelGreen", 1.0f, 1.0f, 0.0f, TryGetQueueCount("FuelGreen", queueCountTable));
	spawner->RegisterItem("FuelBlue", 1.0f, 1.0f, 0.0f, TryGetQueueCount("FuelBlue", queueCountTable));
	spawner->RegisterItem("FuelRed", 1.0f, 1.0f, 0.0f, TryGetQueueCount("FuelRed", queueCountTable));
	spawner->RegisterItem("Propane", 1.0f, 1.0f, 0.0f, TryGetQueueCount("Propane", queueCountTable));
}

void World::RegisterHealth(ObjectPooler* pooler, ObjectSpawner* spawner, const std::map<std::string, int>& queueCountTable) const
{
	pooler->Register("Health_kit", 0, [](ResourceManager* resources)
		{
			Object* object = resources->AssembleObject("HealthKit", "HealthKitMaterial");


			object->AddComponent<BoxColliderComponent>(dx::XMFLOAT3(0.5f, 0.5f, 0.5f), dx::XMFLOAT3(0, 0, 0));
			object->AddComponent<PickupComponent>(Type::Health, 40.0f);
			object->AddComponent<RigidBodyComponent>(10.0f, FilterGroups::PICKUPS, FilterGroups::EVERYTHING, BodyType::KINEMATIC, true);

			//Physics::Instance().RegisterRigidBody(rd);

			return object;
		});

	pooler->Register("Pills", 0, [](ResourceManager* resources)
		{
			Object* object = resources->AssembleObject("HealthJar", "HealthJarMaterial");

			object->AddComponent<BoxColliderComponent>(dx::XMFLOAT3(0.5f, 0.5f, 0.5f), dx::XMFLOAT3(0, 0, 0));
			object->AddComponent<PickupComponent>(Type::Health, 25.0f);
			object->AddComponent<RigidBodyComponent>(10.0f, FilterGroups::PICKUPS, FilterGroups::EVERYTHING, BodyType::KINEMATIC, true);

			//Physics::Instance().RegisterRigidBody(rd);

			return object;
		});

	spawner->RegisterItem("Health_kit", 1.0f, 1.0f, 0.0f, TryGetQueueCount("Health_kit", queueCountTable));
	spawner->RegisterItem("Pills", 1.0f, 1.0f, 0.0f, TryGetQueueCount("Pills", queueCountTable));
}

void World::RegisterWeapon(ObjectPooler* pooler, ObjectSpawner* spawner, const std::map<std::string, int>& queueCountTable) const
{
	// Hammer, Axe and Bat here?
}

void World::RegisterStatic(ObjectPooler* pooler, ObjectSpawner* spawner, const std::map<std::string, int>& queueCountTable) const
{
	spawner->RegisterInstancedItem(resources->GetResource<Mesh>("Rock1"), resources->GetResource<Material>("Rock1Material"), 1.0f, 1.0f, 0.0f, 5, dx::XMUINT3(1, 1, 1));
	spawner->RegisterInstancedItem(resources->GetResource<Mesh>("Rock2"), resources->GetResource<Material>("Rock2Material"), 1.0f, 1.0f, -0.5f, 5, dx::XMUINT3(1, 1, 1));
	spawner->RegisterInstancedItem(resources->GetResource<Mesh>("Rock3"), resources->GetResource<Material>("Rock3Material"), 1.0f, 1.0f, 0.0f, 5, dx::XMUINT3(1, 1, 1));
	spawner->RegisterInstancedItem(resources->GetResource<Mesh>("Log"), resources->GetResource<Material>("LogMaterial"), 1.0f, 1.0f, 0.0f, 1, dx::XMUINT3(0, 1, 0));

	// varf�r?
	//spawner->RegisterInstancedItem(resources->GetResource<Mesh>("Basket"), resources->GetResource<Material>("BasketMaterial"), 1.0f, 1.0f, 0.0f, 1);
}

void World::RegisterToPool(ObjectPooler* pooler, ObjectSpawner* spawner, const std::map<std::string, int>& queueCountTable) const
{
	RegisterFood(pooler, spawner, queueCountTable);
	RegisterFuel(pooler, spawner, queueCountTable);
	RegisterHealth(pooler, spawner, queueCountTable);
	RegisterWeapon(pooler, spawner, queueCountTable);
	RegisterStatic(pooler, spawner, queueCountTable);

	/*pooler->Register("static_sphere", 0, [](ResourceManager* resources)
		{
			Object* object = resources->AssembleObject("Test", "TestMaterial");

			object->AddComponent<BoxColliderComponent>(dx::XMFLOAT3(0.5f, 0.5f, 0.5f), dx::XMFLOAT3(0, 0, 0));

			RigidBodyComponent* rd = object->AddComponent<RigidBodyComponent>(0.0f, FilterGroups::PROPS, FilterGroups::EVERYTHING, BodyType::STATIC);
			Physics::Instance().RegisterRigidBody(rd);
			return object;
		});*/

		//spawner->RegisterInstancedItem(resources->GetResource<Mesh>("Rock1"), resources->GetResource<Material>("Rock1Material"), 1.0f, 1.0f, 0.0f, 1);
		//spawner->RegisterInstancedItem(resources->GetResource<Mesh>("Test"), resources->GetResource<Material>("TestMaterial"), 1.0f, 1.0f, 3.0f, 1);
}
