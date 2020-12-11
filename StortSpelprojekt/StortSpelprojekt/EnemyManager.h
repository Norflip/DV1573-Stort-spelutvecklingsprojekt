#pragma once
#include "EnemySMComp.h"
#include "ResourceManager.h"
#include "ObjectPooler.h"
#include "Pathfinding.h"
#include "Math.h"
#include "Random.h"
constexpr float ENEMY_SPAWN_RATE_MAX = 5;//9.f; //how fast enemies spawn
constexpr float ENEMY_SPAWN_RATE_MIN = 2;//2.f; 
constexpr float ENEMY_SPAWN_RADIUS = 10.f; //distance from player
constexpr int ENEMY_BASE_TOTAL = 40;//12; //base value of max nr of enemies

//multiplier per level/segment
constexpr float ENEMY_HEALTH_MULTIPLIER = 0.2f;  
constexpr float ENEMY_ATTACK_MULTIPLIER = 0.4f; 

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
		//void SpawnEnemies();
		void DespawnEnemies();

		void SetWorld(World* world) { this->world = world; }
		void SetSegment(int segment) { this->currentLevel = segment; }

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
		World* world;
		int currentLevel;

		//size_t nrOfEnemies; //total
		int nrOfEnemiesTotal;
		float enemySpawnRate;
		size_t nrOfBaseEnemies;
		size_t nrOfChargeEnemies;
		//size_t aliveEnemies; //current number of enemies //can use vector size
		float enemySpawnTimer;


		int numLights;
		//bool PointInFrustum(dx::XMFLOAT4 frustum[4], dx::XMFLOAT3 point);
		//bool PointInFrustum(const std::vector<dx::XMFLOAT4>& frustum, const dx::XMFLOAT3& point);
		//bool SphereInFrustum(const std::vector<dx::XMFLOAT4>& frustum, const dx::XMFLOAT3& point);
		//bool SphereInPlane(const Sphere& sphere, const s_Plane& plane);
};