#include "stdafx.h"
#include "World.h"
#include "Input.h"
#include "PickupComponent.h"
#include "Engine.h"

World::World() : currentRelevantCount(0)
{
}

World::~World()
{
}

void World::Initialize(Object* root, ItemManager* items, Renderer* renderer)
{
	this->items = items;
	this->resources = Engine::Instance->GetResources();
	generator.Initialize(root, this, items, renderer);
	ResetRelevanceIndex();
}

void World::ConstructSegment(const SaveState& state)
{
	Random::SetSeed(state.GetSegmentedSeed());
	ResetRelevanceIndex();
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
	generator.Deconstruct();
	currentRelevantCount = 0;
}

void World::ResetRelevanceIndex()
{
	lastRelevantIndex = dx::XMINT2(DEFAULT_RELEVANT_INDEX, DEFAULT_RELEVANT_INDEX);
}

void World::UpdateRelevantChunks(const Transform& transform, CameraComponent* camera)
{
	// check player index
	// if not the same as the current one, loop relevant and enable other chunks
	if (generator.IsConstructed())
	{
		dx::XMFLOAT3 position;
		dx::XMStoreFloat3(&position, transform.GetWorldPosition());
		dx::XMINT2 newIndex = Chunk::WorldToIndex(position.x, position.z);

		if (lastRelevantIndex.x != newIndex.x || lastRelevantIndex.y != newIndex.y)
		{
			//std::cout << "\nLoading new chunks. count: " << relevant.size() << std::endl;

			lastRelevantIndex = newIndex;
			for (size_t i = 0; i < currentRelevantCount; i++)
				relevant[i]->GetOwner()->RemoveFlag(ObjectFlag::ENABLED, false);

			GetChunksInRadius(lastRelevantIndex, TERRAIN_RELEVANT_RADIUS, relevant, currentRelevantCount);

			//std::cout << "new relevant count: " << relevant.size() <<  std::endl;
			for (size_t i = 0; i < currentRelevantCount; i++)
				relevant[i]->GetOwner()->AddFlag(ObjectFlag::ENABLED, false);
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
	Chunk* chunk = GetChunk(x, z);
	if (chunk)
		normal = chunk->SampleNormal(x, z);
}

void World::GetChunksInRadius(const dx::XMINT2& index, int radius, Chunk* chunks[], size_t& count) const
{
	if (generator.IsConstructed())
	{
		const World::ChunkMap& map = generator.GetChunkMap();
		count = 0;
		//std::cout << "getting chunks in radius | items: " << map.size() << ", radius: " << radius << std::endl;

		for (int y = -radius; y <= radius; y++)
		{
			for (int x = -radius; x <= radius; x++)
			{
				int i = HASH2D_I(index.x + x, index.y + y);
				auto find = map.find(i);
				if (find != map.end())
				{
					chunks[count++] = (*find).second;
				}
			}
		}
	}
}

dx::XMFLOAT3 World::GetPlayerPositionFromHouse(Object* house) const
{
	dx::XMFLOAT3 worldPos;
	dx::XMVECTOR current = house->GetTransform().GetWorldPosition();
	dx::XMStoreFloat3(&worldPos, current);
	const float xOffset = 5.0f;
	const float zOffset = 5.0f;
	const float playerHeight = 1.75f / 2.0f;

	worldPos.x += xOffset;
	worldPos.z += zOffset;
	worldPos.y = SampleHeight(worldPos.x, worldPos.z) + playerHeight + 0.001f;

	return worldPos;
}

void World::UpdateGrass(ID3D11DeviceContext* context)
{
	generator.GetSpawner()->UpdateGrass(context);
}

WorldDescription World::DescriptionFromState(const SaveState& state) const
{
	WorldDescription description(state.GetSegmentedSeed());
	description.directionalSteps = 1;
	description.maxSteps = 10;

	description.leftIndex = Random::Range(0, 2);
	description.rightIndex = Random::Range(0, 2);

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
	items->Register("Baked_beans", PickupType::Food, TryGetQueueCount("Baked_beans", queueCountTable), [](ResourceManager* resources)
		{
			return ItemManager::DefaultCreateItem("Soup", PickupType::Food, 25.0f);
		});

	items->Register("Banana", PickupType::Food, TryGetQueueCount("Banana", queueCountTable), [](ResourceManager* resources)
		{
			return ItemManager::DefaultCreateItem("Banana", PickupType::Food, 15.0f);
		});

	items->Register("Apple", PickupType::Food, TryGetQueueCount("Apple", queueCountTable), [](ResourceManager* resources)
		{
			return ItemManager::DefaultCreateItem("Apple", PickupType::Food, 15.0f);
		});

	items->Register("Fruits", PickupType::Food, TryGetQueueCount("Fruits", queueCountTable), [](ResourceManager* resources)
		{
			return ItemManager::DefaultCreateItem("Fruits", PickupType::Food, 35.0f);
		});

	items->Register("Pear", PickupType::Food, TryGetQueueCount("Pear", queueCountTable), [](ResourceManager* resources)
		{
			return ItemManager::DefaultCreateItem("Pear", PickupType::Food, 35.0f);
		});
}

void World::RegisterFuel(ObjectSpawner* spawner, const std::map<std::string, int>& queueCountTable) const
{
	/*spawner->RegisterItem("FuelGreen", TryGetQueueCount("FuelGreen", queueCountTable), [](ResourceManager* resources)
		{
			return ObjectSpawner::DefaultCreateItem("FuelCanGreen", PickupType::Fuel, 20.0f);
		});*/

		/*spawner->RegisterItem("FuelBlue", TryGetQueueCount("FuelBlue", queueCountTable), [](ResourceManager* resources)
			{
				return ObjectSpawner::DefaultCreateItem("FuelCanBlue", PickupType::Fuel, 20.0f);
			});*/

	items->Register("FuelRed", PickupType::Fuel, TryGetQueueCount("FuelRed", queueCountTable), [](ResourceManager* resources)
		{
			return ItemManager::DefaultCreateItem("FuelCanRed", PickupType::Fuel, 20.0f);
		});

	items->Register("Propane", PickupType::Fuel, TryGetQueueCount("Propane", queueCountTable), [](ResourceManager* resources)
		{
			return ItemManager::DefaultCreateItem("Propane", PickupType::Fuel, 35.0f);
		});
}

void World::RegisterHealth(ObjectSpawner* spawner, const std::map<std::string, int>& queueCountTable) const
{
	items->Register("Health_kit", PickupType::Health, TryGetQueueCount("Health_kit", queueCountTable), [](ResourceManager* resources)
		{
			return ItemManager::DefaultCreateItem("HealthKit", PickupType::Health, 40.0f);
		});
	items->Register("Pills", PickupType::Health, TryGetQueueCount("Pills", queueCountTable), [](ResourceManager* resources)
		{
			return ItemManager::DefaultCreateItem("HealthJar", PickupType::Health, 25.0f);
		});
}

void World::RegisterWeapon(ObjectSpawner* spawner, const std::map<std::string, int>& queueCountTable) const
{
	// Hammer, Axe and Bat here?
}

void World::RegisterStatic(ObjectSpawner* spawner, const std::map<std::string, int>& queueCountTable) const
{
	static const dx::XMUINT3 UP = dx::XMUINT3(0, 1, 0);

	spawner->RegisterInstancedItem("Rock1", 0.5f, 1, UP);
	spawner->RegisterInstancedItem("Rock2", 0.5f, 1, UP);
	//spawner->RegisterInstancedItem("Rock3", 0.5f, 1, UP);
	spawner->RegisterInstancedItem("Log", 0.5f, 1, UP);
}