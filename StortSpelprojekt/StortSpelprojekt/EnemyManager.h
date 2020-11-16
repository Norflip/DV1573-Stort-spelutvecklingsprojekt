#pragma once
#include "EnemyChargerSMComp.h"
#include "EnemySMComp.h"
#include "ResourceManager.h"
#include "ObjectPooler.h"

class EnemyManager
{
	public:
		EnemyManager(ResourceManager* resources, Object* player, PlayerComp* playerComp, Object* root);
		~EnemyManager() {}
		std::vector<Object*> GetBaseEnemies() const { return baseEnemies; }
		void InitBaseEnemy();
		void InitChargerEnemy();
		void RemoveEnemy(Object* enemy);
		ObjectPooler* GetPool() { return enemyPool; }
	private:
		void SpawnEnemy(dx::XMVECTOR position);

		ObjectPooler* enemyPool;
		std::vector<Object*> baseEnemies;
		//std::vector<Object*> baseEnemies;
		Object* player;
		PlayerComp* playerComp;
		ResourceManager* resources;
		Object* root;
		int nrOfEnemies;
};