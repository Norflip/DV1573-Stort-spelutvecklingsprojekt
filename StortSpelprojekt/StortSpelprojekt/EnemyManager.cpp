#include "stdafx.h"
#include "EnemyManager.h"

EnemyManager::EnemyManager(ResourceManager* resources, Renderer* renderer, PlayerComp* playerComp)
	: resources(resources), renderer(renderer), playerComp(playerComp)
{
	this->baseEnemies = new ObjectPooler(resources);
}

void EnemyManager::AddBaseEnemy(dx::XMVECTOR position)
{
	baseEnemies->Register("baseEnemy", 0, [](ResourceManager* resources) 
	{
		Object* object = new Object("baseEnemy", ObjectFlag::ENABLED);
		//Object* object = resources->AssembleObject("Enemy", "EnemyMaterial");
		object->AddComponent<SkeletonMeshComponent>(resources->GetResource<SkeletonMeshComponent>("EnemySkeleton"));
		object->GetTransform().SetScale({ 0.125f, 0.125f, 0.125f });
		object->AddComponent<EnemyStatsComp>(100.f, 2.0f, 10.f, 5.f, 3.f, 3.f);
		//enemyStatsComp = enemy->GetComponent<EnemyStatsComp>();
		dx::XMFLOAT3 zero = { 0.f, 0.f, 0.f };
		object->AddComponent<CapsuleColliderComponent>(1.6f, 1.8f, zero);

		object->AddComponent<RigidBodyComponent>(10.f, FilterGroups::ENEMIES, (FilterGroups::EVERYTHING & ~FilterGroups::PICKUPS) & ~FilterGroups::HOLDABLE, BodyType::KINEMATIC, true);
		return object;
	});

	baseEnemy = baseEnemies->GetItem("baseEnemy");

	EnemySMComp* stateMachine = baseEnemy->AddComponent<EnemySMComp>(EnemyState::IDLE);
	stateMachine->RegisterState(EnemyState::IDLE, baseEnemy->AddComponent<EnemyIdleComp>());
	//stateMachine->RegisterState(EnemyState::PATROL, enemy->AddComponent<EnemyPatrolComp>());
	stateMachine->RegisterState(EnemyState::ATTACK, baseEnemy->AddComponent<EnemyAttackComp>(playerComp));
	stateMachine->Start();
	stateMachine->InitAnimation();

	baseEnemy->GetComponent<RigidBodyComponent>()->SetPosition(position);
	//baseEnemies.push_back(baseEnemy);
}

void EnemyManager::InitBaseEnemy()
{
	baseEnemy = new Object("baseEnemy", ObjectFlag::ENABLED);
	baseEnemy->AddComponent<SkeletonMeshComponent>(resources->GetResource<SkeletonMeshComponent>("EnemySkeleton"));
	baseEnemy->GetTransform().SetScale({ 0.125f, 0.125f, 0.125f });
	baseEnemy->AddComponent<EnemyStatsComp>(100.f, 2.0f, 10.f, 5.f, 3.f, 3.f);
	//enemyStatsComp = enemy->GetComponent<EnemyStatsComp>();
	dx::XMFLOAT3 zero = { 0.f, 0.f, 0.f };
	baseEnemy->AddComponent<CapsuleColliderComponent>(1.6f, 1.8f, zero);

	baseEnemy->AddComponent<RigidBodyComponent>(10.f, FilterGroups::ENEMIES, (FilterGroups::EVERYTHING & ~FilterGroups::PICKUPS) & ~FilterGroups::HOLDABLE, BodyType::KINEMATIC, true);

	EnemySMComp* stateMachine = baseEnemy->AddComponent<EnemySMComp>(EnemyState::IDLE);
	stateMachine->RegisterState(EnemyState::IDLE, baseEnemy->AddComponent<EnemyIdleComp>());
	//stateMachine->RegisterState(EnemyState::PATROL, enemy->AddComponent<EnemyPatrolComp>());
	stateMachine->RegisterState(EnemyState::ATTACK, baseEnemy->AddComponent<EnemyAttackComp>(playerComp));
	stateMachine->Start();
	stateMachine->InitAnimation();
}
