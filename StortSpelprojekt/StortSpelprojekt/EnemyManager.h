#pragma once
#include "EnemySMComp.h"
#include "ResourceManager.h"
#include "ObjectPooler.h"
#include "Math.h"
#include "Random.h"
#define ENEMY_SPAWN_RATE 2.f //how fast enemies spawn
#define ENEMY_SPAWN_RADIUS 7.f //distance from player
#define ENEMY_TOTAL 25 //max nr of enemies

class EnemyManager
{
	public:
		EnemyManager();
		virtual ~EnemyManager();

		void Initialize(Object* player, PlayerComp* playerComp,CameraComponent* camComp, Object* root);

		std::vector<Object*> GetEnemies() const { return enemyVector; }
		void InitBaseEnemy();
		void InitChargerEnemy();
		void RemoveEnemy(Object* enemy);
		
		void SpawnEnemy(const float& deltaTime);
		//void TerminateEnemies();
		void SpawnEnemies();
		void DespawnEnemies();

	private:
		void SpawnEnemy(std::string key, dx::XMVECTOR position);
		ObjectPooler* enemyPool;
		std::vector<Object*> enemyVector;
		Object* player;
		PlayerComp* playerComp;
		CameraComponent* camComp;
		ResourceManager* resources;
		Object* root;

		size_t nrOfEnemies; //total
		size_t nrOfBaseEnemies;
		size_t nrOfChargeEnemies;
		size_t aliveEnemies; //current number of enemies
		float enemySpawnTimer;

		//bool PointInFrustum(dx::XMFLOAT4 frustum[4], dx::XMFLOAT3 point);
		bool PointInFrustum(const std::vector<dx::XMFLOAT4>& frustum, const dx::XMFLOAT3& point);
	
};