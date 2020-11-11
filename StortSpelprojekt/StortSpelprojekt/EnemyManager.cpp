#include "stdafx.h"
#include "EnemyManager.h"

EnemyManager::EnemyManager(ResourceManager* resources, Object* player, PlayerComp* playerComp, Object* root)
	: resources(resources), player(player), playerComp(playerComp), root(root), nrOfEnemies(2)
{
	this->enemyPool = new ObjectPooler(resources);
}

void EnemyManager::InitBaseEnemy()
{
	enemyPool->Register("baseEnemy", 2, [](ResourceManager* resources)
	{
		Object* object = new Object("baseEnemy", ObjectFlag::ENABLED);
		object->AddComponent<SkeletonMeshComponent>(resources->GetResource<SkeletonMeshComponent>("EnemySkeleton"));
		object->GetTransform().SetScale({ 0.125f, 0.125f, 0.125f });
		object->AddComponent<EnemyStatsComp>(100.f, 2.0f, 10.f, 5.f, 3.f, 3.f);
		dx::XMFLOAT3 zero = { 0.f, 0.f, 0.f };
		object->AddComponent<CapsuleColliderComponent>(1.6f, 1.8f, zero);

		object->AddComponent<RigidBodyComponent>(10.f, FilterGroups::ENEMIES, (FilterGroups::EVERYTHING & ~FilterGroups::PICKUPS) & ~FilterGroups::HOLDABLE, BodyType::KINEMATIC, true);
		EnemySMComp* stateMachine = object->AddComponent<EnemySMComp>(EnemyState::IDLE);
		stateMachine->RegisterState(EnemyState::IDLE, object->AddComponent<EnemyIdleComp>());
		//stateMachine->RegisterState(EnemyState::PATROL, enemy->AddComponent<EnemyPatrolComp>());
		stateMachine->RegisterState(EnemyState::ATTACK, object->AddComponent<EnemyAttackComp>(nullptr));
		return object;
	});

	for (int i = 0; i < nrOfEnemies; i++)
	{
		dx::XMFLOAT3 playerPos;
		dx::XMStoreFloat3(&playerPos, player->GetTransform().GetPosition());
		SpawnEnemy({ playerPos.x, playerPos.y + 6, (float)(playerPos.z + i * 5 )});
	}
}

void EnemyManager::RemoveEnemy(Object* enemy)
{
	enemyPool->ReturnItem("baseEnemy", enemy);
}

void EnemyManager::SpawnEnemy(dx::XMVECTOR position)
{
	Object* enemy = enemyPool->GetItem("baseEnemy");
	EnemySMComp* stateMachine = enemy->GetComponent<EnemySMComp>();
	enemy->GetComponent<EnemyAttackComp>()->SetPlayer(playerComp);
	stateMachine->Start();
	stateMachine->InitAnimation();
	enemy->GetComponent<RigidBodyComponent>()->SetPosition(position);
	Transform::SetParentChild(root->GetTransform(), enemy->GetTransform());
}