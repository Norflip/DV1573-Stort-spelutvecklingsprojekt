#include "stdafx.h"
#include "World.h"
#include "Input.h"
#include "PickupComponent.h"
#include "Engine.h"

World::World() : player(nullptr), house(nullptr)
{
}

World::~World()
{
}

void World::Initialize(Object* root, ResourceManager* resources, ObjectPooler* pooler, Renderer* renderer, CameraComponent* camera)
{
	this->resources = resources;
	ObjectSpawner* spawner = new ObjectSpawner();
	spawner->Initialize(root, pooler, renderer, resources, camera);
	RegisterToPool(pooler, spawner, std::map<std::string, int>());
	
	generator.Initialize(root, resources, spawner, renderer->GetDevice(), renderer->GetContext());
	playerIndex = dx::XMINT2(-5000, -5000);
}

void World::ConstructSegment(SaveState state, SegmentDescription description)
{
	//@TODO MOVE PLAYER AND HOUSE

	int seed = state.seed ^ std::hash<int>()(state.segment);
	Random::SetSeed(seed);
	state.GenerateSeeds(4);
	generator.Construct(state, description);
}

void World::DeconstructSegment()
{
	relevant.clear();
	generator.Deconstruct();
}

void World::UpdateRelevantChunks()
{
	const float offset = 0.1f;

	// check player index
	// if not the same as the current one, loop relevant and enable other chunks
	if (player != nullptr && generator.IsConstructed())
	{
		dx::XMINT2 newIndex = GetChunkIndex(player);
		//std::cout << playerIndex.x << ", " << playerIndex.y << "\t" << newIndex.x << ", " << newIndex.y << std::endl;

		if (playerIndex.x != newIndex.x || playerIndex.y != newIndex.y)
		{
			playerIndex = newIndex;

			for (auto i : relevant)
				i->GetOwner()->RemoveFlag(ObjectFlag::ENABLED);

			relevant.clear();
			generator.GetChunksInRadius(playerIndex, RELEVANT_RADIUS, relevant);

			for (auto i : relevant)
				i->GetOwner()->AddFlag(ObjectFlag::ENABLED);
		}
	}
}

void World::DrawDebug()
{
	generator.DrawDebug();
}

void World::MoveHouseAndPlayerToStart()
{
	if (house != nullptr && player != nullptr && generator.IsConstructed())
	{
		std::vector<dx::XMINT2> indexes = generator.GetPath().GetIndexes();
		dx::XMINT2 spawnIndex = indexes[0];

		dx::XMVECTOR position = dx::XMVectorAdd(Chunk::IndexToWorld(spawnIndex, 0.0f), dx::XMVectorSet(CHUNK_SIZE / 2.0f, 0, CHUNK_SIZE / 2.0f, 0));
		house->GetTransform().SetPosition(position);

		if (house->HasComponent<RigidBodyComponent>())
			house->GetComponent<RigidBodyComponent>()->SetPosition(position);

		position = dx::XMVectorAdd(position, dx::XMVectorSet(5, 12, 0, 0));

		player->GetTransform().SetPosition(position);
		player->GetComponent<RigidBodyComponent>()->SetPosition(position);
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
		object->AddComponent<RigidBodyComponent>(10.0f, FilterGroups::HOLDABLE, FilterGroups::EVERYTHING &~FilterGroups::PLAYER , BodyType::KINEMATIC, true);

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
	spawner->RegisterInstancedItem(resources->GetResource<Mesh>("Rock1"), resources->GetResource<Material>("Rock1Material"), 1.0f, 1.0f, 0.0f, 5, dx::XMUINT3(1,1,1));
	spawner->RegisterInstancedItem(resources->GetResource<Mesh>("Rock2"), resources->GetResource<Material>("Rock2Material"), 1.0f, 1.0f, -0.5f, 5, dx::XMUINT3(1, 1, 1));
	spawner->RegisterInstancedItem(resources->GetResource<Mesh>("Rock3"), resources->GetResource<Material>("Rock3Material"), 1.0f, 1.0f, 0.0f, 5, dx::XMUINT3(1, 1, 1));
	spawner->RegisterInstancedItem(resources->GetResource<Mesh>("Log"), resources->GetResource<Material>("LogMaterial"), 1.0f, 1.0f, 0.0f, 1, dx::XMUINT3(0, 1, 0));

	// varför?
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
