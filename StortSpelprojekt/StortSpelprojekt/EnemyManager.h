#pragma once
#include "EnemySMComp.h"
#include "ResourceManager.h"
#include "ObjectPooler.h"
class EnemyManager
{
	public:
		EnemyManager();
		virtual ~EnemyManager();

		void Initialize(Object* player, PlayerComp* playerComp, Object* root);

		std::vector<Object*> GetBaseEnemies() const { return baseEnemies; }
		void InitBaseEnemy();
		void InitChargerEnemy();
		void RemoveEnemy(Object* enemy);
		
		void SpawnEnemies();
		void DespawnEnemies();

	private:
		void SpawnEnemy(std::string key, dx::XMVECTOR position);

		ObjectPooler* enemyPool;
		std::vector<Object*> baseEnemies;
		Object* player;
		PlayerComp* playerComp;
		ResourceManager* resources;
		Object* root;
		int nrOfEnemies;
};