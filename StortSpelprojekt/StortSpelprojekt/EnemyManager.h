#pragma once
#include "EnemySMComp.h"
#include "ResourceManager.h"
#include "ObjectPooler.h"
#include "Pathfinding.h"
#include "Math.h"
#include "Random.h"
constexpr float ENEMY_SPAWN_RATE_MAX = 3.f; //how fast enemies spawn
constexpr float ENEMY_SPAWN_RATE_MIN = 1.f; 
constexpr float ENEMY_SPAWN_RADIUS = 3.f; //distance from player
constexpr int ENEMY_TOTAL = 14; //max nr of enemies

class EnemyManager
{
	public:
		EnemyManager();
		virtual ~EnemyManager();

		void Initialize(Object* playerObj,Object* houseObj, PlayerComp* playerComp,CameraComponent* camComp, Object* rootObj);

		std::vector<Object*> GetEnemies() const { return enemyVector; }
		void InitBaseEnemy();
		void InitChargerEnemy();
		void RemoveEnemy(Object* enemy);
		
		void SpawnRandomEnemy(const float& deltaTime);
		//void TerminateEnemies();
		//void SpawnEnemies();
		void DespawnEnemies();

	private:
		void SpawnEnemy(std::string key, dx::XMVECTOR position);
		ObjectPooler* enemyPool;
		std::vector<Object*> enemyVector;
		Object* player;
		Object* house;
		PlayerComp* playerComp;
		CameraComponent* camComp;
		ResourceManager* resources;
		Object* root;

		//size_t nrOfEnemies; //total
		float enemySpawnRate;
		size_t nrOfBaseEnemies;
		size_t nrOfChargeEnemies;
		size_t aliveEnemies; //current number of enemies
		float enemySpawnTimer;

		//bool PointInFrustum(dx::XMFLOAT4 frustum[4], dx::XMFLOAT3 point);
		//bool PointInFrustum(const std::vector<dx::XMFLOAT4>& frustum, const dx::XMFLOAT3& point);
		//bool SphereInFrustum(const std::vector<dx::XMFLOAT4>& frustum, const dx::XMFLOAT3& point);
		//bool SphereInPlane(const Sphere& sphere, const s_Plane& plane);
};