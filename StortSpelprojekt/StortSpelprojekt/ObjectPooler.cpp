#include "stdafx.h"
#include "ObjectPooler.h"

ObjectPooler::ObjectPooler(ResourceManager* resources) : resourceManager(resources) {}
ObjectPooler::~ObjectPooler()
{
	Clear();
}

void ObjectPooler::Register(std::string key, size_t count, std::function<Object* (ResourceManager*)> factory)
{
	assert(count < POOL_MAX_LIMIT);

	Pool* pool = new Pool();
	pool->key = key;
	pool->factory = factory;

	pools.insert({ key, pool });
	Warm(pools[key], count, true);
}

void ObjectPooler::Unregister(std::string key)
{
	auto found = pools.find(key);
	if (found != pools.end())
		pools.erase(found);
}

Object* ObjectPooler::GetItem(std::string key)
{
	Pool* pool = GetPool(key);
	Object* obj = nullptr;

	if (pool != nullptr)
	{
		if (pool->inside.size() == 0)
		{
			Warm(pool, 1, true);
		}
		else
		{
			std::cout << "RETURNING POOLED ITEM. " << pool->inside.size() << " REMANING.\n";
		}

		obj = pool->inside.front();
		pool->inside.pop();

		pool->outside.insert(obj);
		obj->AddFlag(ObjectFlag::ENABLED);
		RigidBodyComponent* body = obj->GetComponent<RigidBodyComponent>();
		if (body != nullptr)
			body->GetRigidBody()->setIsActive(true);
	}

	return obj;
}

void ObjectPooler::ReturnItem(Object* object)
{
	PooledItem* pooledItem = object->GetComponent<PooledItem>();
	if (pooledItem != nullptr)
	{
		std::string key = pooledItem->GetKey();
		Pool* pool = GetPool(key);
		if (pool != nullptr)
		{
			object->RemoveFlag(ObjectFlag::ENABLED);
			assert(pool->inside.size() + pool->outside.size() > POOL_MAX_LIMIT);

			RigidBodyComponent* body = object->GetComponent<RigidBodyComponent>();
			if (body != nullptr)
				body->GetRigidBody()->setIsActive(false);


			auto found = pool->outside.find(object);
			if (found != pool->outside.end())
			{
				auto callbacks = object->GetComponentsOfSubType<PoolReturnCallback>();
				for (auto i : callbacks)
					i->OnReturn();

				pool->outside.erase(found);
				pool->inside.push(object);
			}
		}
	}
}

ObjectPooler::Pool* ObjectPooler::GetPool(std::string key)
{
	auto found = pools.find(key);
	return found != pools.end() ? (*found).second : nullptr;
}

void ObjectPooler::Clear()
{
	if (pools.size() > 0)
	{
		for (auto i : pools)
		{
			Pool* pool = i.second;
			while (!pool->inside.empty())
			{
				Object* obj = pool->inside.front();
				pool->inside.pop();
				delete obj;
			}

			//for (auto j : pool->outside) delete j;
			delete pool;
		}

		pools.clear();
	}
}

void ObjectPooler::Warm(Pool* pool, size_t amount, bool additive)
{
	if (amount > 0)
	{
		size_t toAdd = amount;
		if (additive)
			toAdd -= pool->inside.size();

		assert((pool->inside.size() + pool->outside.size() + toAdd) < POOL_MAX_LIMIT);

		for (size_t i = 0; i < toAdd; i++)
		{
			Object* obj = (pool->factory)(resourceManager);
			obj->AddComponent< PooledItem>()->SetKey(pool->key);

			obj->RemoveFlag(ObjectFlag::ENABLED);
			pool->inside.push(obj);
		}
	}
}
