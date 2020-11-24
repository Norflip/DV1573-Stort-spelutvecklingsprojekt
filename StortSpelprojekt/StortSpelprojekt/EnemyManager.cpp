#include "stdafx.h"
#include "EnemyManager.h"
#include "Engine.h"

EnemyManager::EnemyManager() {}

EnemyManager::~EnemyManager()
{
}

void EnemyManager::Initialize(Object* player, PlayerComp* playerComp, Object* root)
{
	this->resources = Engine::Instance->GetResources();
	this->player = player;
	this->playerComp = playerComp;
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
		object->AddComponent<EnemyStatsComp>(100.f, 2.0f, 10.f, 5.f, 3.f, 3.f);
		dx::XMFLOAT3 zero = { 0.f, 0.f, 0.f };
		object->AddComponent<CapsuleColliderComponent>(1.6f, 1.8f, zero);
		object->AddComponent<RigidBodyComponent>(10.f, FilterGroups::ENEMIES, (FilterGroups::EVERYTHING & ~FilterGroups::PICKUPS) & ~FilterGroups::HOLDABLE, BodyType::KINEMATIC, true);
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
		object->AddComponent<EnemyStatsComp>(100.f, 2.0f, 10.f, 5.f, 3.f, 3.f);
		dx::XMFLOAT3 zero = { 0.f, 0.f, 0.f };
		object->AddComponent<CapsuleColliderComponent>(0.8f, 0.8f, zero);
		object->AddComponent<RigidBodyComponent>(10.f, FilterGroups::ENEMIES, (FilterGroups::EVERYTHING & ~FilterGroups::PICKUPS) & ~FilterGroups::HOLDABLE, BodyType::KINEMATIC, true);
		
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
	enemyPool->ReturnItem(enemy);
}

void EnemyManager::SpawnEnemies()
{
	// whyyyy
	nrOfEnemies = 1;

	for (size_t i = 0; i < nrOfEnemies; i++)
	{
		dx::XMFLOAT3 playerPos = player->GetComponent<PlayerComp>()->GetStartPosition();
		SpawnEnemy("baseEnemy", { (float)playerPos.x + (i + 2) * 10.0f, playerPos.y + 3.0f, (float)(playerPos.z + i * 5.0f) });
		
	}
	
	for (size_t i = 0; i < nrOfEnemies; i++)
	{
		dx::XMFLOAT3 playerPos = player->GetComponent<PlayerComp>()->GetStartPosition();
		SpawnEnemy("chargerEnemy", { (float)playerPos.x + (i + 2) * 10.0f, playerPos.y + 5.0f, (float)(playerPos.z + i * 5.0f) });
	}
}

void EnemyManager::DespawnEnemies()
{
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
	stateMachine->InitAnimation();


	enemy->GetComponent<RigidBodyComponent>()->SetPosition(position);
	Object::AddToHierarchy(root, enemy);
	
	enemy->GetComponent<EnemyStatsComp>()->SetManager(this);

	enemyVector.push_back(enemy);
}
