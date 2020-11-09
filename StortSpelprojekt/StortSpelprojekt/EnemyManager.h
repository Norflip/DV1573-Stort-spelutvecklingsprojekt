#pragma once
#include "EnemySMComp.h"
#include "ResourceManager.h"
#include "ObjectPooler.h"

class EnemyManager
{
	public:
		EnemyManager(ResourceManager* resources, Renderer* renderer, PlayerComp* playerComp);
		~EnemyManager() {}
		void Update();
		void AddBaseEnemy(dx::XMVECTOR position);
		void DeleteEnemy();
		Object* GetBaseEnemy() const { return baseEnemy; }
		void InitBaseEnemy();
	private:
		//ObjectPooler* baseEnemies;
		Object* baseEnemy;
		std::vector<Object*> baseEnemies;
		PlayerComp* playerComp;
		ResourceManager* resources;
		Renderer* renderer;

};