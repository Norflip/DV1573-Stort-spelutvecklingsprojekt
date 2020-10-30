#pragma once
#include "Object.h"
#include "PoolReturnCallback.h"
#include "ResourceManager.h"

constexpr size_t POOL_MAX_LIMIT = 8000;

class ObjectPooler
{
	struct Pool
	{ 
		std::string key;
		std::function<Object* (ResourceManager*)> factory;
		std::unordered_set<Object*> outside;
		std::queue<Object*> inside;
	};

public:
	ObjectPooler(ResourceManager* resourceManager);
	virtual ~ObjectPooler();

	void Register(std::string key, size_t count, std::function<Object* (ResourceManager*)> factory);
	void Unregister(std::string key);

	Object* GetItem(std::string key);
	void ReturnItem(std::string key, Object* object);
	Pool* GetPool(std::string key);

	void Clear();

private:
	void Warm(Pool* pool, size_t amount, bool additive);

	std::unordered_map<std::string, Pool*> pools;
	ResourceManager* resourceManager;
};

/*
ObjectPool pooler;
pooler.Register("tree_0", 10, 10, []()->{});
*/