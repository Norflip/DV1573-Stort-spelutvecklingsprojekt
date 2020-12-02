#include "stdafx.h"
#include "EnemyManager.h"
#include "Engine.h"

EnemyManager::EnemyManager() 
{
	enemyPool = nullptr;
	//enemyVector
	player = nullptr;
	playerComp = nullptr;
	camComp = nullptr;
	resources = nullptr;
	root = nullptr;

	nrOfBaseEnemies = 0;
	nrOfChargeEnemies = 0;
	aliveEnemies = 0;
	enemySpawnTimer = 0;
}

EnemyManager::~EnemyManager()
{
}

void EnemyManager::Initialize(Object* player, PlayerComp* playerComp,CameraComponent* camComp, Object* root)
{
	//this->enemySpawnTimer = 0.f;
	//this->aliveEnemies = 0.f;
	this->resources = Engine::Instance->GetResources();
	this->player = player;
	this->playerComp = playerComp;
	this->camComp = camComp;
	this->root = root;
	this->enemyPool = new ObjectPooler(resources);
}

void EnemyManager::InitBaseEnemy()
{
	enemyPool->Register("baseEnemy", 2, [](ResourceManager* resources)
	{
		Object* object = new Object("baseEnemy", ObjectFlag::DEFAULT);
		object->AddComponent<SkeletonMeshComponent>(resources->GetResource<SkeletonMeshComponent>("EnemySkeleton"));
		object->GetTransform().SetScale({ 0.125f, 0.125f, 0.125f });
		object->AddComponent<EnemyStatsComp>(100.f, 2.0f, 10.f, 2.0f, 5.f, 3.f, 3.f);
		dx::XMFLOAT3 zero = { 0.f, 0.f, 0.f };
		object->AddComponent<CapsuleColliderComponent>(1.6f, 1.8f, zero);
		object->AddComponent<RigidBodyComponent>(100.f, FilterGroups::ENEMIES, (FilterGroups::EVERYTHING & ~FilterGroups::PICKUPS) & ~FilterGroups::HOLDABLE, BodyType::KINEMATIC, true);
		EnemySMComp* stateMachine = object->AddComponent<EnemySMComp>(EnemyState::IDLE);
		stateMachine->RegisterState(EnemyState::PATROL, object->AddComponent<EnemyPatrolComp>());
		stateMachine->RegisterState(EnemyState::IDLE, object->AddComponent<EnemyIdleComp>());
		stateMachine->RegisterState(EnemyState::ATTACK, object->AddComponent<EnemyAttackComp>(nullptr));
		return object;
	});

}

void EnemyManager::InitChargerEnemy()
{
	enemyPool->Register("chargerEnemy", 2, [](ResourceManager* resources)
	{
		Object* object = new Object("chargerEnemy", ObjectFlag::DEFAULT);
		object->RemoveFlag(ObjectFlag::ENABLED);

		object->AddComponent<SkeletonMeshComponent>(resources->GetResource<SkeletonMeshComponent>("ChargerSkeleton"));
		object->AddComponent<EnemyStatsComp>(100.f, 2.0f, 10.f, 6.f, 10.f, 2.f, 3.f);
		dx::XMFLOAT3 zero = { 0.f, 0.f, 0.f };
		object->AddComponent<CapsuleColliderComponent>(0.8f, 0.8f, zero);
		object->AddComponent<RigidBodyComponent>(100.f, FilterGroups::ENEMIES, (FilterGroups::EVERYTHING & ~FilterGroups::PICKUPS) & ~FilterGroups::HOLDABLE, BodyType::KINEMATIC, true);
		
		EnemySMComp* stateMachine = object->AddComponent<EnemySMComp>(EnemyState::IDLE);
		stateMachine->RegisterState(EnemyState::PATROL, object->AddComponent<EnemyPatrolComp>());
		stateMachine->RegisterState(EnemyState::IDLE, object->AddComponent<EnemyIdleComp>());
		stateMachine->RegisterState(EnemyState::ATTACK, object->AddComponent<EnemyAttackComp>(nullptr));
		stateMachine->RegisterState(EnemyState::RUN, object->AddComponent<EnemyAttackComp>(nullptr));
		//stateMachine->RegisterState(EnemyChargerState::DEATH, object->GetComponent<EnemyStatsComp>());	// Death component or something something???
		return object;
	});
}

void EnemyManager::RemoveEnemy(Object* enemy)
{
	//std::cout << "enemy died: " <<enemy->GetName()<< std::endl;
	aliveEnemies--;
	if ("baseEnemy")
	{
		nrOfBaseEnemies--;
	}
	else if ("chargerEnemy")
	{
		nrOfChargeEnemies--;
	}
	enemyPool->ReturnItem(enemy);
}

//bool EnemyManager::PointInFrustum(const std::vector<dx::XMFLOAT4>& frustum, const dx::XMFLOAT3& point)
//{
//	for (int i = 0; i < 6;i++)//(Plane plane in frustum) 
//	{
//		Frustum camFrustum; //making copy to easier use dot function
//		camFrustum.planes[i].N.x = frustum[i].x;
//		camFrustum.planes[i].N.y = frustum[i].y;
//		camFrustum.planes[i].N.z = frustum[i].z;
//		camFrustum.planes[i].d = frustum[i].w;
//
//		if (Math::Dot(camFrustum.planes[i].N, point) - camFrustum.planes[i].d < 0)
//		{
//			return false;
//		}
//	}
//	return true;
//}


void EnemyManager::SpawnRandomEnemy(const float& deltaTime)
{
	enemySpawnTimer += deltaTime;
	//spawn on timer intervall if nrof enemies is less than cap
	if (enemySpawnTimer >= ENEMY_SPAWN_RATE && aliveEnemies < ENEMY_TOTAL)
	{
		dx::XMFLOAT3 playerPos = { 0,0,0 };
		dx::XMVECTOR playerVec = player->GetTransform().GetPosition();
		dx::XMStoreFloat3(&playerPos, playerVec);

		//make sure it's outside frustum of player camera and a bit away from player
		float length = 0;

		if (camComp->GetFrustumPlanes().size() == 6)
		{
			//random points
			dx::XMFLOAT3 randPos;// = playerPos;
			dx::XMVECTOR randVec;// = dx::XMLoadFloat3(&randPos);

			//while inside radius of player or inside cam frustum do random again
			randPos.x = playerPos.x + Random::Range(-20, 20+1);
			randPos.y = playerPos.y + 0.5f; //height over ground
			randPos.z = playerPos.z + Random::Range(-20, 20+1);
			randVec = dx::XMLoadFloat3(&randPos);
			//std::cout << length << std::endl;
			dx::XMStoreFloat(&length, dx::XMVector3Length(dx::XMVectorSubtract(playerVec, randVec)));
	

			Bounds enemyBounds;
			enemyBounds.SetMinMax({ -0.5f,-1.f,-0.5f }, { 0.5f,1.f,0.5f });
			
			dx::XMMATRIX world = dx::XMMatrixTranslation(randPos.x, randPos.y, randPos.z);

			if (length > ENEMY_SPAWN_RADIUS && !camComp->InView(enemyBounds,world))  //!SphereInFrustum(theFrustum, randPos))
			{
				//spawn enemies & increment nrof
				aliveEnemies++;
				enemySpawnTimer = 0;
				int enemyType = Random::Range(0, 2);
				if (enemyType == 0) // base_enemy
				{
					SpawnEnemy("baseEnemy", randVec);
					//std::cout << "base enemy ";
					nrOfBaseEnemies++;
				}
				else if (enemyType == 1)// charger_enemy
				{
					SpawnEnemy("chargerEnemy", randVec);
					//std::cout << "charger enemy ";
					nrOfChargeEnemies++;

				}
				//std::cout << "spawned..."<<" nr of is " << aliveEnemies << std::endl;
				//std::cout << "playerpos: x: " << playerPos.x << ", y: " << playerPos.y << ", z: " << playerPos.z << std::endl;
				//std::cout << "enemypos: x: " << randPos.x << ", y:" << randPos.y << ", z: " << randPos.z << std::endl;
				//std::cout << "L: " << length << " - " << !camComp->InView(enemyBounds, world) << std::endl;
			}
		}
	}
}

//void EnemyManager::SpawnEnemies() 
//{
//	// whyyyy
//	nrOfEnemies = 2;
//
//	for (size_t i = 0; i < nrOfBaseEnemies; i++)
//	{
//		dx::XMFLOAT3 playerPos = player->GetComponent<PlayerComp>()->GetStartPosition();
//		SpawnEnemy("baseEnemy", { (float)playerPos.x + (i + 2) * 10.0f, playerPos.y + 3.0f, (float)(playerPos.z + i * 5.0f) });
//		
//	}
//	
//	for (size_t i = 0; i < nrOfChargeEnemies; i++)
//	{
//		dx::XMFLOAT3 playerPos = player->GetComponent<PlayerComp>()->GetStartPosition();
//		SpawnEnemy("chargerEnemy", { (float)playerPos.x + (i + 2) * 10.0f, playerPos.y + 5.0f, (float)(playerPos.z + i * 5.0f) });
//	}
//}

void EnemyManager::DespawnEnemies()
{
	nrOfBaseEnemies = 0;
	nrOfChargeEnemies = 0;
	aliveEnemies = 0;
	for (size_t i = 0; i < enemyVector.size(); i++)
	{
		RemoveEnemy(enemyVector[i]);
	}
}

void EnemyManager::SpawnEnemy(std::string key, dx::XMVECTOR position)
{
	Object* enemy = enemyPool->GetItem(key);
	enemy->AddFlag(ObjectFlag::ENABLED);

	EnemySMComp* stateMachine = enemy->GetComponent<EnemySMComp>();
	enemy->GetComponent<EnemyAttackComp>()->SetPlayer(playerComp);
	stateMachine->InitAnimation(player);

	enemy->GetComponent<RigidBodyComponent>()->SetPosition(position);
	Object::AddToHierarchy(root, enemy);
	
	enemy->GetComponent<EnemyStatsComp>()->SetManager(this);
	enemy->GetComponent<EnemyStatsComp>()->Reset();

	enemyVector.push_back(enemy);
}


