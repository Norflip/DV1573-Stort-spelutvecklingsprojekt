#include "stdafx.h"
#include "ItemManager.h"
#include "Engine.h"

ItemManager* ItemManager::instance = nullptr;
ItemManager::ItemManager()
{
	instance = this;
	pooler = new ObjectPooler(Engine::Instance->GetResources());
}

ItemManager::~ItemManager()
{
	for (size_t i = 0; i < activeItems.size(); i++)
	{
		Object::RemoveFromHierarchy(activeItems[i]);
		delete activeItems[i];
	}

	delete pooler;
	activeItems.clear();
}

void ItemManager::Register(const std::string& key, PickupType type, size_t queueCount, std::function<Object* (ResourceManager*)> factory)
{
	pooler->Register(key, 0, factory);
	for (size_t i = 0; i < queueCount; i++)
		allItemRegistry.push_back(key);
	pickupKeyRegistry[UICAST(type)].push_back(key);
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

Object* ItemManager::SpawnRandom(const dx::XMVECTOR& position, Object* container)
{
	return SpawnSpecific(allItemRegistry[UICAST(Random::Range(0, allItemRegistry.size()))], position, container);
}

Object* ItemManager::SpawnRandomOfType(PickupType type, const dx::XMVECTOR& position, Object* container)
{
	assert(UICAST(type) >= 0 && UICAST(type) < MAX_PICKUP_TYPES);
	auto& allkeys = pickupKeyRegistry[UICAST(type)];
	std::string key = allkeys[Random::Range(0, allkeys.size())];

	return SpawnSpecific(key, position, container);
}

Object* ItemManager::SpawnSpecific(const std::string& key, const dx::XMVECTOR& position, Object* container)
{
	Object* object = pooler->GetItem(key);

	if (container != nullptr)
		Object::AddToHierarchy(container, object);

	RigidBodyComponent* body = object->GetComponent<RigidBodyComponent>();
	if (body != nullptr)
	{
		body->SetPosition(position);
		body->GetRigidBody()->setIsActive(true);
	}
	object->GetTransform().SetWorldPosition(position);

	activeItems.push_back(object);
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


Object* ItemManager::DefaultCreateItem(std::string key, PickupType type, float value)
{
	Object* object = Engine::Instance->GetResources()->AssembleObject(key, key + "Material", true);

	object->AddComponent<PickupComponent>(type, value);
	if (type == PickupType::Fuel)
	{
		object->AddComponent<BoxColliderComponent>(dx::XMFLOAT3(0.3f, 0.35f, 0.15f), dx::XMFLOAT3(0, 0, 0));
		object->AddComponent<RigidBodyComponent>(10.0f, FilterGroups::HOLDABLE, FilterGroups::EVERYTHING & ~FilterGroups::PLAYER, BodyType::DYNAMIC, true);
	}
	else
	{
		object->AddComponent<BoxColliderComponent>(dx::XMFLOAT3(0.25f, 0.25f, 0.25f), dx::XMFLOAT3(0, 0, 0));
		object->AddComponent<RigidBodyComponent>(10.0f, FilterGroups::PICKUPS, FilterGroups::EVERYTHING & ~FilterGroups::PLAYER, BodyType::DYNAMIC, true);
	}

	Renderer* renderer = Engine::Instance->GetRenderer();
	ParticleSystemComponent* particles = object->AddComponent<ParticleSystemComponent>(renderer, Engine::Instance->GetResources()->GetShaderResource("particleShader"));
	particles->InitializeParticles(renderer->GetDevice(), "Particle");

	return object;
}
