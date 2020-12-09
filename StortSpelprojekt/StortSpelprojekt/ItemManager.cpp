#include "stdafx.h"
#include "ItemManager.h"
#include "Engine.h"

ItemManager::ItemManager() : ItemManager(Bounds(dx::XMFLOAT3(-1000, -1000, -1000), dx::XMFLOAT3(1000, 1000, 1000)))
{
}

ItemManager::ItemManager(const Bounds& worldBounds)
{
	itemsSpawnQT = new QuadTree(worldBounds);
	pooler = new ObjectPooler(Engine::Instance->GetResources());
}

ItemManager::~ItemManager()
{
	if (itemsSpawnQT != nullptr)
		delete itemsSpawnQT;
}

void ItemManager::Register(const std::string& key, PickupType type, size_t queueCount, std::function<Object* (ResourceManager*)> factory)
{
	pooler->Register(key, 0, factory);
	for (size_t i = 0; i < queueCount; i++)
		allItemRegistry.push_back(key);
}

void ItemManager::Unregister(const std::string& key)
{
	for (int i = ICAST(allItemRegistry.size() - 1); i >= 0; i--)
	{
		if (allItemRegistry[i] == key)
		{
			allItemRegistry.erase(allItemRegistry.begin() + i);
		}
	}
}

void ItemManager::ChangeQueueCount(const std::string& key, size_t count)
{
	Unregister(key);
	for (size_t i = 0; i < count; i++)
		allItemRegistry.push_back(key);
}

void ItemManager::UpdateNearbySpawns(const dx::XMFLOAT3& eye)
{
	dx::XMFLOAT2 min(eye.x - SPAWN_DISTANCE, eye.z - SPAWN_DISTANCE);
	dx::XMFLOAT2 max(eye.x + SPAWN_DISTANCE, eye.z + SPAWN_DISTANCE);

	size_t spawnCount = 0;
	auto nodes = itemsSpawnQT->GetNodesInArea(min, max);
	for (auto i : nodes)
	{
		Object* object = static_cast<Object*>(i.data);
		if (object != nullptr)
			object->AddFlag(ObjectFlag::ENABLED);
		spawnCount++;
		itemsSpawnQT->Remove(i);
	}

	if (spawnCount > 0)
		std::cout << "SPAWNED: " << spawnCount << std::endl;
}

Object* ItemManager::SpawnRandomOfType(PickupType type, const dx::XMFLOAT3& position, bool spawnInstant)
{
	assert(type >= 0 && type < MAX_PICKUP_TYPES);
	auto& allkeys = pickupKeyRegistry[UICAST(type)];
	std::string key = allkeys[Random::Range(0, allkeys.size())];
	return SpawnSpecific(key, position, spawnInstant);
}

Object* ItemManager::SpawnSpecific(const std::string& key, const dx::XMFLOAT3& position, bool spawnInstant)
{
	Object* object = pooler->GetItem(key);
	object->GetTransform().SetWorldPosition(dx::XMLoadFloat3(&position));
	activeItems.push_back(object);

	if (!spawnInstant)
	{
		dx::XMFLOAT2 min(position.x - 1, position.z - 1);
		dx::XMFLOAT2 max(position.x + 1, position.z + 1);
		object->RemoveFlag(ObjectFlag::ENABLED);
		itemsSpawnQT->Insert(min, max, object);
	}

	return object;
}

void ItemManager::Despawn(Object* object)
{
	pooler->ReturnItem(object);
	auto found = std::find(activeItems.begin(), activeItems.end(), object);
	if (found != activeItems.end())
		activeItems.erase(found);
}

void ItemManager::DespawnAll()
{
	for (int i = activeItems.size() - 1; i >= 0; i--)
	{
		pooler->ReturnItem(activeItems[i]);
		activeItems.erase(activeItems.begin() + i);
	}
}
