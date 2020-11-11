#pragma once
#include "EnemySMComp.h"
#include "ResourceManager.h"
#include "ObjectPooler.h"

class EnemyManager
{
	public:
		EnemyManager(ResourceManager* resources, Renderer* renderer, PlayerComp* playerComp, Object* root);
		~EnemyManager() {}
		std::vector<Object*> GetBaseEnemies() const { return baseEnemies; }
		void InitBaseEnemy();
		void RemoveEnemy(Object* enemy);
		ObjectPooler* GetPool() { return enemyPool; }
	private:
		void SpawnEnemy(dx::XMVECTOR position);

		ObjectPooler* enemyPool;
		std::vector<Object*> baseEnemies;
		//std::vector<Object*> baseEnemies;
		PlayerComp* playerComp;
		ResourceManager* resources;
		Renderer* renderer;
		Object* root;

};