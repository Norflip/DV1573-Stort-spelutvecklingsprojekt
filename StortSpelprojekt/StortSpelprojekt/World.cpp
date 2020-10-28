#include "World.h"
#include "Input.h"
#include "PickupComponent.h"

World::World() : player(nullptr), house(nullptr)
{
}

World::~World()
{
}

void World::Initialize(Object* root, ResourceManager* resources, ObjectPooler* pooler, Renderer* renderer)
{
	ObjectSpawner* spawner = new ObjectSpawner();
	spawner->Initialize(root, pooler);
	RegisterToPool(pooler, spawner, std::map<std::string, int>());
	
	generator.Initialize(root, resources, spawner, renderer->GetDevice(), renderer->GetContext());
	playerIndex = dx::XMINT2(-5000, -5000);
}

void World::ConstructSegment(SaveState state, SegmentDescription description)
{
	//@TODO MOVE PLAYER AND HOUSE
	

	Physics& physics = Physics::Instance();
	physics.MutexLock();

	int seed = state.seed ^ std::hash<int>()(state.segment);
	Random::SetSeed(seed);

	state.GenerateSeeds(4);

	generator.Construct(state, description);

	physics.MutexUnlock();
}

void World::DeconstructSegment()
{
	Physics& physics = Physics::Instance();
	physics.MutexLock();

	relevant.clear();
	generator.Deconstruct();

	physics.MutexUnlock();
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
			Physics& physics = Physics::Instance();
			physics.MutexLock();

			playerIndex = newIndex;

			for (auto i : relevant)
				i->GetOwner()->RemoveFlag(ObjectFlag::ENABLED);

			relevant.clear();
			generator.GetChunksInRadius(playerIndex, chunkRelevancyRadius, relevant);

			for (auto i : relevant)
				i->GetOwner()->AddFlag(ObjectFlag::ENABLED);

			physics.MutexUnlock();
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
		dx::XMINT2 spawnIndex = indexes[1];
		
		dx::XMVECTOR position = dx::XMVectorAdd(Chunk::IndexToWorld(spawnIndex, 0.0f), dx::XMVectorSet(CHUNK_SIZE / 2.0f, 0, CHUNK_SIZE / 2.0f, 0));
		house->GetTransform().SetPosition(position);

		position = dx::XMVectorAdd(position, dx::XMVectorSet(5, 5, 0, 0));
	
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

void World::RegisterToPool(ObjectPooler* pooler, ObjectSpawner* spawner, const std::map<std::string, int>& queueCountTable) const
{

	pooler->Register("Health_kit", 0, [](ResourceManager* resources)
	{
		Object* object = resources->AssembleObject("HealthKit", "HealthKitMaterial");


		object->AddComponent<BoxColliderComponent>(dx::XMFLOAT3(0.5f, 0.5f, 0.5f), dx::XMFLOAT3(0, 0, 0));
		object->AddComponent<PickupComponent>(Type::Health, 20.0f);

		RigidBodyComponent* rd = object->AddComponent<RigidBodyComponent>(10.0f, FilterGroups::PICKUPS, FilterGroups::EVERYTHING, BodyType::KINEMATIC);
		Physics::Instance().RegisterRigidBody(rd);

		return object;
	});

	pooler->Register("Fuel_can", 0, [](ResourceManager* resources)
	{
		Object* object = resources->AssembleObject("FuelCan", "FuelCanMaterial");

		object->AddComponent<BoxColliderComponent>(dx::XMFLOAT3(0.5f, 0.5f, 0.5f), dx::XMFLOAT3(0, 0, 0));
		object->AddComponent<PickupComponent>(Type::Fuel, 20.0f);

		RigidBodyComponent* rd = object->AddComponent<RigidBodyComponent>(10.0f, FilterGroups::PICKUPS, FilterGroups::EVERYTHING, BodyType::KINEMATIC);

		Physics::Instance().RegisterRigidBody(rd);
		return object;
	});

	pooler->Register("Baked_beans", 0, [](ResourceManager* resources)
	{
		Object* object = resources->AssembleObject("BakedBeans", "BakedBeansMaterial");

		object->AddComponent<BoxColliderComponent>(dx::XMFLOAT3(0.5f, 0.5f, 0.5f), dx::XMFLOAT3(0, 0, 0));
		object->AddComponent<PickupComponent>(Type::Food, 20.0f);

		RigidBodyComponent* rd = object->AddComponent<RigidBodyComponent>(10.0f, FilterGroups::PICKUPS, FilterGroups::EVERYTHING, BodyType::KINEMATIC);

		Physics::Instance().RegisterRigidBody(rd);
		return object;
	});

	pooler->Register("static_sphere", 0, [](ResourceManager* resources)
	{
		Object* object = resources->AssembleObject("Test", "TestMaterial");

		object->AddComponent<BoxColliderComponent>(dx::XMFLOAT3(0.5f, 0.5f, 0.5f), dx::XMFLOAT3(0, 0, 0));

		RigidBodyComponent* rd = object->AddComponent<RigidBodyComponent>(0.0f, FilterGroups::PROPS, FilterGroups::EVERYTHING, BodyType::STATIC);
		Physics::Instance().RegisterRigidBody(rd);
		return object;
	});

	spawner->RegisterItem("Health_kit", 1.0f, 1.0f, 0.0f, TryGetQueueCount("Health_kit", queueCountTable), ItemSpawnType::ITEM);
	spawner->RegisterItem("Fuel_can", 1.0f, 1.0f, 0.0f, TryGetQueueCount("Fuel_can", queueCountTable), ItemSpawnType::ITEM);
	spawner->RegisterItem("Baked_beans", 1.0f, 1.0f, 0.0f, TryGetQueueCount("Baked_beans", queueCountTable), ItemSpawnType::ITEM);

	spawner->RegisterItem("static_sphere", 1.0f, 1.0f, 0.0f, TryGetQueueCount("static_sphere", queueCountTable), ItemSpawnType::PROP);
}
