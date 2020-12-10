#pragma once
#include "ObjectPooler.h"
#include "PickupComponent.h"
#include "QuadTree.h"

constexpr size_t MAX_PICKUP_TYPES = 3;

class ItemManager
{
	const float SPAWN_DISTANCE = 80.0f;

public:
	ItemManager();
	virtual ~ItemManager();

	void Register(const std::string& key, PickupType type, size_t queueCount, std::function<Object* (ResourceManager*)> factory);
	void Unregister(const std::string& key);
	void ChangeQueueCount(const std::string& key, size_t count);

	Object* SpawnRandom(const dx::XMVECTOR& position, Object* container);
	Object* SpawnRandomOfType(PickupType type, const dx::XMVECTOR& position, Object* container);
	Object* SpawnSpecific(const std::string& key, const dx::XMVECTOR& position, Object* container);
	void Despawn(Object* object);
	void DespawnAll();

	static Object* DefaultCreateItem(std::string key, PickupType type, float value);
	static ItemManager* instance;

private:
	ObjectPooler* pooler;
	std::vector<std::string> allItemRegistry;
	std::vector<std::string> pickupKeyRegistry[MAX_PICKUP_TYPES];
	
	std::vector<Object*> activeItems;
};