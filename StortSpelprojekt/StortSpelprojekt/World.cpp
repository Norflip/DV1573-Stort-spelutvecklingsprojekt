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
	generator.Initialize(root, this, renderer);
	lastRelevantIndex = dx::XMINT2(-5000, -5000);
}

void World::ConstructSegment(const SaveState& state)
{
	Random::SetSeed(state.GetSegmentedSeed());
	this->description = DescriptionFromState(state);

	ObjectSpawner* spawner = generator.GetSpawner();
	RegisterFood(spawner, description.queueModifier);
	RegisterFuel(spawner, description.queueModifier);
	RegisterHealth(spawner, description.queueModifier);
	RegisterWeapon(spawner, description.queueModifier);
	RegisterStatic(spawner, description.queueModifier);


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

			for (auto i : relevant)
			{
				i->GetOwner()->AddFlag(ObjectFlag::ENABLED);
			}
		}
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

	float tmp = normal.y;
	normal.y = normal.z;
	normal.z = tmp;
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

WorldDescription World::DescriptionFromState(const SaveState& state) const
{
	WorldDescription description(state.seed);
	description.directionalSteps = 1;
	description.maxSteps = 4;

	description.minEnviromentProps = 4;
	description.maxEnviromentProps = 8;

	return description;
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

void World::RegisterFood(ObjectSpawner* spawner, const std::map<std::string, int>& queueCountTable) const
{
	spawner->RegisterItem("Baked_beans", TryGetQueueCount("Baked_beans", queueCountTable), [](ResourceManager* resources)
		{
			return ObjectSpawner::DefaultCreateItem("Soup", PickupType::Food, 25.0f);
		});

	spawner->RegisterItem("Banana", TryGetQueueCount("Banana", queueCountTable), [](ResourceManager* resources)
		{
			return ObjectSpawner::DefaultCreateItem("Banana", PickupType::Food, 15.0f);
		});

	spawner->RegisterItem("Apple", TryGetQueueCount("Apple", queueCountTable), [](ResourceManager* resources)
		{
			return ObjectSpawner::DefaultCreateItem("Apple", PickupType::Food, 15.0f);
		});

	spawner->RegisterItem("Fruits", TryGetQueueCount("Fruits", queueCountTable), [](ResourceManager* resources)
		{
			return ObjectSpawner::DefaultCreateItem("Fruits", PickupType::Food, 35.0f);
		});

	spawner->RegisterItem("Pear", TryGetQueueCount("Pear", queueCountTable), [](ResourceManager* resources)
		{
			return ObjectSpawner::DefaultCreateItem("Pear", PickupType::Food, 35.0f);
		});
}

void World::RegisterFuel(ObjectSpawner* spawner, const std::map<std::string, int>& queueCountTable) const
{
	spawner->RegisterItem("FuelGreen", TryGetQueueCount("FuelGreen", queueCountTable), [](ResourceManager* resources)
		{
			return ObjectSpawner::DefaultCreateItem("FuelCanGreen", PickupType::Fuel, 20.0f);
		});

	spawner->RegisterItem("FuelBlue", TryGetQueueCount("FuelBlue", queueCountTable), [](ResourceManager* resources)
		{
			return ObjectSpawner::DefaultCreateItem("FuelCanBlue", PickupType::Fuel, 20.0f);
		});

	spawner->RegisterItem("FuelRed", TryGetQueueCount("FuelRed", queueCountTable), [](ResourceManager* resources)
		{
			return ObjectSpawner::DefaultCreateItem("FuelCanRed", PickupType::Fuel, 20.0f);
		});

	spawner->RegisterItem("Propane", TryGetQueueCount("Propane", queueCountTable), [](ResourceManager* resources)
		{
			return ObjectSpawner::DefaultCreateItem("Propane", PickupType::Fuel, 35.0f);
		});
}

void World::RegisterHealth(ObjectSpawner* spawner, const std::map<std::string, int>& queueCountTable) const
{
	spawner->RegisterItem("Health_kit", TryGetQueueCount("Health_kit", queueCountTable), [](ResourceManager* resources)
		{
			return ObjectSpawner::DefaultCreateItem("HealthKit", PickupType::Health, 40.0f);
		});
	spawner->RegisterItem("Pills", TryGetQueueCount("Pills", queueCountTable), [](ResourceManager* resources)
		{
			return ObjectSpawner::DefaultCreateItem("HealthJar", PickupType::Health, 25.0f);
		});
}

void World::RegisterWeapon(ObjectSpawner* spawner, const std::map<std::string, int>& queueCountTable) const
{
	// Hammer, Axe and Bat here?
}

void World::RegisterStatic(ObjectSpawner* spawner, const std::map<std::string, int>& queueCountTable) const
{
	static const dx::XMUINT3 UP = dx::XMUINT3(0, 1, 0);

	spawner->RegisterInstancedItem("Rock1", 0.0f, 1, UP);
	spawner->RegisterInstancedItem("Rock2", 0.0f, 1, UP);
	spawner->RegisterInstancedItem("Rock3", 0.0f, 1, UP);
	spawner->RegisterInstancedItem("Log",	0.0f, 1, UP);

}