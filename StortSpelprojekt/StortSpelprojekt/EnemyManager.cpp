#include "stdafx.h"
#include "EnemyManager.h"
#include "Engine.h"

EnemyManager::EnemyManager() 
{
	enemyPool = nullptr;
	//enemyVector
	player = nullptr;
	house = nullptr;
	playerComp = nullptr;
	camComp = nullptr;
	resources = nullptr;
	root = nullptr;
	world = nullptr;

	currentLevel = 0;
	nrOfEnemiesTotal = ENEMY_BASE_TOTAL;
	enemySpawnRate = 1.f;
	nrOfBaseEnemies = 0;
	nrOfChargeEnemies = 0;
	//aliveEnemies = 0;
	enemySpawnTimer = 0;
}

EnemyManager::~EnemyManager()
{
}

void EnemyManager::Initialize(Object* playerObj, Object* houseObj, PlayerComp* playerComp,CameraComponent* camComp, Object* rootObj)
{
	this->enemySpawnTimer = 0.f;
	//this->aliveEnemies = 0.f;
	this->nrOfEnemiesTotal = ENEMY_BASE_TOTAL;
	this->resources = Engine::Instance->GetResources();
	this->player = playerObj;
	this->house = houseObj;
	this->playerComp = playerComp;
	this->camComp = camComp;
	this->root = rootObj;
	this->enemyPool = new ObjectPooler(resources);
}

void EnemyManager::InitBaseEnemy()
{
	enemyPool->Register("baseEnemy", 2, [](ResourceManager* resources)
	{
		Object* object = new Object("baseEnemy", ObjectFlag::DEFAULT);
		SkeletonMeshComponent* skeleton = object->AddComponent<SkeletonMeshComponent>(resources->GetResource<SkeletonMeshComponent>("EnemySkeleton"));
		skeleton->SetEnemyType(EnemyType::BASE);
		object->GetTransform().SetScale({ 0.125f, 0.125f, 0.125f });
		object->AddComponent<EnemyStatsComp>(100.f, 2.0f, 25.f, 2.0f, 5.f, 3.f, 3.f);
		dx::XMFLOAT3 zero = { 0.f, 0.f, 0.f };
		object->AddComponent<CapsuleColliderComponent>(1.6f, 1.8f, zero);
		object->AddComponent<RigidBodyComponent>(100.f, FilterGroups::ENEMIES, (FilterGroups::EVERYTHING & ~FilterGroups::PICKUPS) & ~FilterGroups::HOLDABLE, BodyType::KINEMATIC, true);
		
		
		EnemySMComp* stateMachine = object->AddComponent<EnemySMComp>(EnemyState::IDLE);
		stateMachine->RegisterState(EnemyState::PATROL, object->AddComponent<EnemyPatrolComp>());
		stateMachine->RegisterState(EnemyState::IDLE, object->AddComponent<EnemyIdleComp>());
		stateMachine->RegisterState(EnemyState::ATTACK, object->AddComponent<EnemyAttackComp>(nullptr));
		object->AddComponent<Pathfinding>(nullptr);
		return object;
	});

}

void EnemyManager::InitChargerEnemy()
{
	enemyPool->Register("chargerEnemy", 2, [](ResourceManager* resources)
	{
		Object* object = new Object("chargerEnemy", ObjectFlag::DEFAULT);
		object->RemoveFlag(ObjectFlag::ENABLED);

		SkeletonMeshComponent* skeleton = object->AddComponent<SkeletonMeshComponent>(resources->GetResource<SkeletonMeshComponent>("ChargerSkeleton"));
		skeleton->SetEnemyType(EnemyType::CHARGER);
		object->AddComponent<EnemyStatsComp>(100.f, 2.0f, 25.f, 5.f, 10.f, 2.f, 3.f);
		dx::XMFLOAT3 zero = { 0.f, 0.f, 0.f };
		object->AddComponent<CapsuleColliderComponent>(0.8f, 0.8f, zero);
		object->AddComponent<RigidBodyComponent>(100.f, FilterGroups::ENEMIES, (FilterGroups::EVERYTHING & ~FilterGroups::PICKUPS) & ~FilterGroups::HOLDABLE, BodyType::KINEMATIC, true);
		
		EnemySMComp* stateMachine = object->AddComponent<EnemySMComp>(EnemyState::IDLE);
		stateMachine->RegisterState(EnemyState::PATROL, object->AddComponent<EnemyPatrolComp>());
		stateMachine->RegisterState(EnemyState::IDLE, object->AddComponent<EnemyIdleComp>());
		stateMachine->RegisterState(EnemyState::ATTACK, object->AddComponent<EnemyAttackComp>(nullptr));
		stateMachine->RegisterState(EnemyState::RUN, object->AddComponent<EnemyAttackComp>(nullptr));
		object->AddComponent<Pathfinding>(nullptr);
		//stateMachine->RegisterState(EnemyChargerState::DEATH, object->GetComponent<EnemyStatsComp>());	// Death component or something something???
		return object;
	});
}

void EnemyManager::RemoveEnemy(Object* enemy)
{
	//std::cout << "enemy died: " <<enemy->GetName()<< std::endl;
	//aliveEnemies--;
	if (enemy->GetName() == "baseEnemy")
	{
		nrOfBaseEnemies--;
	}
	else if (enemy->GetName() == "chargerEnemy")
	{
		nrOfChargeEnemies--;
	}
	enemyPool->ReturnItem(enemy);
	for (int i = 0; i < enemyVector.size(); i++)
	{
		if (enemyVector.at(i) == enemy)
			enemyVector.erase(enemyVector.begin()+i);
	}
}

void EnemyManager::SpawnRandomEnemy(const float& deltaTime)
{
	enemySpawnTimer += deltaTime;
	//spawn on timer intervall if nrof enemies is less than cap
	//std::cout << aliveEnemies <<" - " <<enemyVector.size()<< " - new enemy spawns in: " << enemySpawnRate << "sec, time: " << enemySpawnTimer << " sec." << std::endl;
	//std::cout <<enemyVector.size()<< " - new enemy spawns in: " << enemySpawnRate << "sec, time: " << enemySpawnTimer << " sec." << std::endl;
	if (enemySpawnTimer >= enemySpawnRate && /*aliveEnemies*/ enemyVector.size()< this->nrOfEnemiesTotal )
	{
		dx::XMVECTOR houseVec = house->GetTransform().GetPosition();

		dx::XMFLOAT3 playerPos = { 0,0,0 };
		dx::XMVECTOR playerVec = player->GetTransform().GetPosition();
		dx::XMStoreFloat3(&playerPos, playerVec);

		//make sure it's outside frustum of player camera and a bit away from player
		float lengthP = 0; //length enemy to player
		float lengthH = 0; //length enemy to house
		float lengthPH = 0; //length player to house
		if (camComp->GetFrustumPlanes().size() == 6)
		{
			//random points
			dx::XMFLOAT3 randPos;
			dx::XMVECTOR randVec;

			//while inside radius of player or inside cam frustum do random again
			randPos.x = playerPos.x + Random::Range(-40, 40+1);
			randPos.z = playerPos.z + Random::Range(-40, 40+1);
			randPos.y = this->world->SampleHeight(randPos.x, randPos.z) + 1.5f; //height over ground
			randVec = dx::XMLoadFloat3(&randPos);
			dx::XMStoreFloat(&lengthP, dx::XMVector3Length(dx::XMVectorSubtract(playerVec, randVec)));
			dx::XMStoreFloat(&lengthH, dx::XMVector3Length(dx::XMVectorSubtract(houseVec, randVec)));
			dx::XMStoreFloat(&lengthPH, dx::XMVector3Length(dx::XMVectorSubtract(playerVec, houseVec)));
			
			Bounds enemyBounds;
			enemyBounds.SetMinMax({ -0.5f,-1.f,-0.5f }, { 0.5f,1.f,0.5f });
			
			dx::XMMATRIX world = dx::XMMatrixTranslation(randPos.x, randPos.y, randPos.z);

			//total number of enemies is increased depending on how far and length from house
			float houseProgress = house->GetComponent<NodeWalkerComp>()->GetHouseProgress();
			this->nrOfEnemiesTotal = ENEMY_BASE_TOTAL + (this->currentLevel * 1.5) + (double(lengthPH) * 0.02f) + (double(houseProgress) * 4.f); // lengthPH/50
			//std::cout << "Level: " << this->currentLevel <<", potential enemies: "<<nrOfEnemiesTotal<<", current nr: "<< enemyVector.size()<< std::endl;

			//check if player is above -10, a length from player is above value, a length from house is above value, coord is inside cam-view
			if (playerPos.y > -10.f &&lengthP > ENEMY_SPAWN_RADIUS && lengthH > ENEMY_SPAWN_RADIUS && !camComp->InView(enemyBounds, world))  //!SphereInFrustum(theFrustum, randPos))
			{
				//spawn enemies & increment nrof
				enemySpawnTimer = 0;

				//enemies spawns faster depending on how far progress the player made
				this->enemySpawnRate = Random::Range(ENEMY_SPAWN_RATE_MIN - (houseProgress * 0.5f), ENEMY_SPAWN_RATE_MAX - (houseProgress ));
				//std::cout <<"spawnrate: "<< this->enemySpawnRate <<", total possible: "<<nrOfEnemiesTotal<< std::endl;

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
				//std::cout << "spawned..."<<" nr of is " << /*aliveEnemies*/enemyVector.size() << std::endl;
				//std::cout << "playerpos: x: " << playerPos.x << ", y: " << playerPos.y << ", z: " << playerPos.z << std::endl;
				//std::cout << "enemypos: x: " << randPos.x << ", y:" << randPos.y << ", z: " << randPos.z << std::endl;
				//std::cout << "L: " << lengthP << " - " << !camComp->InView(enemyBounds, world) << std::endl;
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
	//aliveEnemies = 0;
	for (size_t i = 0; i < enemyVector.size(); i++)
	{
		RemoveEnemy(enemyVector[i]);
		//enemy
	}
}

void EnemyManager::SpawnEnemy(std::string key, dx::XMVECTOR position)
{
	Object* enemy = enemyPool->GetItem(key);
	enemy->AddFlag(ObjectFlag::ENABLED);

	EnemySMComp* stateMachine = enemy->GetComponent<EnemySMComp>();
	enemy->GetComponent<EnemyAttackComp>()->SetPlayer(playerComp);
	enemy->GetComponent<Pathfinding>()->SetPlayer(playerComp);
	enemy->GetComponent<Pathfinding>()->SetEnabled(false);
	stateMachine->InitAnimation(player);

	enemy->GetComponent<RigidBodyComponent>()->SetPosition(position);
	Object::AddToHierarchy(root, enemy);
	
	EnemyStatsComp* enemyStats = enemy->GetComponent<EnemyStatsComp>();
	enemyStats->SetManager(this);
	enemyStats->Reset();

	int level = MetaProgress::Instance().GetLevelsCleared();
	enemyStats->RaiseStats(1+ ENEMY_HEALTH_MULTIPLIER * level,1+ ENEMY_ATTACK_MULTIPLIER * level);
		

	//std::cout << "enemy ["<<enemyVector.size()<<"]"<<std::endl
	//	<<" hp: " <<enemyStats->GetHealth()<<", attack: "<< enemyStats->GetAttack()<<std::endl;

	enemyVector.push_back(enemy);
}


