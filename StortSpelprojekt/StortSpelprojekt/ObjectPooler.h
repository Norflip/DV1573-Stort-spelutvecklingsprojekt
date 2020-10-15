#pragma once
#include "Object.h"
#include "PoolReturnCallback.h"

#include <assert.h>
#include <functional>
#include <queue>
#include <unordered_set>

constexpr size_t POOL_MAX_LIMIT = 8000;

class ObjectPooler
{
	struct Pool
	{ 
		std::string key;
		std::function<Object* ()> factory;
		std::unordered_set<Object*> outside;
		std::queue<Object*> inside;
	};

public:
	ObjectPooler();
	virtual ~ObjectPooler();

	void Register(std::string key, size_t count, std::function<Object* ()> factory);
	void Unregister(std::string key);

	Object* GetItem(std::string key);
	void ReturnItem(std::string key, Object* object);
	Pool* GetPool(std::string key);

	void Clear();

private:
	void Warm(Pool* pool, size_t amount, bool additive);

	std::unordered_map<std::string, Pool*> pools;
};

/*
ObjectPool pooler;
pooler.Register("tree_0", 10, 10, []()->{});
*/