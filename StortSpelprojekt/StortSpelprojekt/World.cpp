#include "World.h"
#include "Input.h"

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
	RegisterToPool(pooler, spawner);
	
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
	generator.Construct(description);

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

void World::RegisterToPool(ObjectPooler* pooler, ObjectSpawner* spawner) const
{
	pooler->Register("dynamic_stone", 0, [](ResourceManager* resources)
	{
		Object* object = new Object("dynamic_stone");
		Mesh* mesh1 = resources->GetResource<Mesh>("Test");
		Material* material1 = resources->GetResource<Material>("TestMaterial");

		object->AddComponent<MeshComponent>(*mesh1, *material1);
		object->AddComponent<BoxColliderComponent>(dx::XMFLOAT3(0.5f, 0.5f, 0.5f), dx::XMFLOAT3(0, 0, 0));

		RigidBodyComponent* rd = object->AddComponent<RigidBodyComponent>(5.0f, FilterGroups::PICKUPS, FilterGroups::EVERYTHING, true);
		Physics::Instance().RegisterRigidBody(rd);

		return object;
	});

	pooler->Register("static_sphere", 0, [](ResourceManager* resources)
	{
		Object* object = new Object("static_sphere");
		Mesh* mesh1 = resources->GetResource<Mesh>("Test2");
		Material* material1 = resources->GetResource<Material>("Test2Material");

		object->AddComponent<MeshComponent>(*mesh1, *material1);
		object->AddComponent<BoxColliderComponent>(dx::XMFLOAT3(0.5f, 0.5f, 0.5f), dx::XMFLOAT3(0, 0, 0));

		RigidBodyComponent* rd = object->AddComponent<RigidBodyComponent>(0.0f, FilterGroups::PROPS, FilterGroups::EVERYTHING, false);

		Physics::Instance().RegisterRigidBody(rd);
		return object;
	});

	spawner->AddItem("dynamic_stone", 1.0f, 1.0f, 10, 25, 0.0f, ItemSpawnType::DYNAMIC);
	spawner->AddItem("static_sphere", 1.0f, 1.0f, 10, 25, 0.0f, ItemSpawnType::STATIC);
}
