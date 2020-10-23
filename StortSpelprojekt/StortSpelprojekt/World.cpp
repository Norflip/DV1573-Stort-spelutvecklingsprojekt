#include "World.h"


World::World() : player(nullptr), house(nullptr)
{
}

World::~World()
{
}

void World::Initialize(Object* root, ResourceManager* resources, ObjectPooler* pooler, Renderer* renderer)
{
	generator.Initialize(root, resources, pooler, renderer->GetDevice(), renderer->GetContext());
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

	generator.Deconstruct();
	relevant.clear();

	physics.MutexUnlock();
}

void World::UpdateRelevantChunks()
{
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