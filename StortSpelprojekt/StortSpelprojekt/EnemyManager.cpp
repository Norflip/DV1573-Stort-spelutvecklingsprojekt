#include "stdafx.h"
#include "EnemyManager.h"

EnemyManager::EnemyManager(ResourceManager* resources, Renderer* renderer, PlayerComp* playerComp)
	: resources(resources), renderer(renderer), playerComp(playerComp)
{
}

void EnemyManager::AddBaseEnemy(dx::XMVECTOR position)
{


	/*baseEnemiesTest->Register("baseEnemy", 1, [](ResourceManager* resources) 
	{
		bool defaultAnimation = false;
		Shader* skeletonShader = resources->GetShaderResource("skeletonShader");
		std::vector<Mesh*> skeletonMesh = ZWEBLoader::LoadMeshes(ZWEBLoadType::SkeletonAnimation, "Models/baseMonster.ZWEB", renderer->GetDevice());
		std::vector<Material*> skeletonMat = ZWEBLoader::LoadMaterials("Models/baseMonster.ZWEB", skeletonShader, renderer->GetDevice());
		SkeletonAni skeletonbaseMonsterIdle = ZWEBLoader::LoadSkeletonOnly("Models/baseMonsterIdle.ZWEB", skeletonMesh[0]->GetBoneIDS(), defaultAnimation);
		SkeletonAni skeletonbaseMonsterWalk = ZWEBLoader::LoadSkeletonOnly("Models/baseMonsterWalk.ZWEB", skeletonMesh[0]->GetBoneIDS(), defaultAnimation);
		SkeletonAni skeletonbaseMonsterRun = ZWEBLoader::LoadSkeletonOnly("Models/baseMonsterRun.ZWEB", skeletonMesh[0]->GetBoneIDS(), defaultAnimation);
		SkeletonAni skeletonbaseMonsterAttack = ZWEBLoader::LoadSkeletonOnly("Models/baseMonsterAttack.ZWEB", skeletonMesh[0]->GetBoneIDS(), defaultAnimation);
		SkeletonAni skeletonbaseMonsterDeath = ZWEBLoader::LoadSkeletonOnly("Models/baseMonsterDeath.ZWEB", skeletonMesh[0]->GetBoneIDS(), defaultAnimation);
		Object* object = new Object("baseEnemy", ObjectFlag::ENABLED);
		SkeletonMeshComponent* baseMonsterComp = object->AddComponent<SkeletonMeshComponent>(skeletonMesh[0], skeletonMat[0]);
		Mesh* mesh1 = resources->GetResource<Mesh>("Test");
		Material* material1 = resources->GetResource<Material>("TestMaterial");

		object->AddComponent<MeshComponent>(mesh1, material1);
		object->AddComponent<BoxColliderComponent>(dx::XMFLOAT3(0.5f, 0.5f, 0.5f), dx::XMFLOAT3(0, 0, 0));
		object->AddComponent<RigidBodyComponent>(10.0f, FilterGroups::DEFAULT, FilterGroups::EVERYTHING, BodyType::DYNAMIC, true);
		return object;
	});*/

	baseEnemy->GetComponent<RigidBodyComponent>()->SetPosition(position);
	baseEnemies.push_back(baseEnemy);
}

void EnemyManager::InitBaseEnemy()
{
	baseEnemy = new Object("baseEnemy", ObjectFlag::ENABLED);
	bool defaultAnimation = false;
	Shader* skeletonShader = resources->GetShaderResource("skeletonShader");
	std::vector<Mesh*> skeletonMesh = ZWEBLoader::LoadMeshes(ZWEBLoadType::SkeletonAnimation, "Models/baseMonster.ZWEB", renderer->GetDevice());
	std::vector<Material*> skeletonMat = ZWEBLoader::LoadMaterials("Models/baseMonster.ZWEB", skeletonShader, renderer->GetDevice());
	SkeletonAni skeletonbaseMonsterIdle = ZWEBLoader::LoadSkeletonOnly("Models/baseMonsterIdle.ZWEB", skeletonMesh[0]->GetBoneIDS(), defaultAnimation);
	SkeletonAni skeletonbaseMonsterWalk = ZWEBLoader::LoadSkeletonOnly("Models/baseMonsterWalk.ZWEB", skeletonMesh[0]->GetBoneIDS(), defaultAnimation);
	SkeletonAni skeletonbaseMonsterRun = ZWEBLoader::LoadSkeletonOnly("Models/baseMonsterRun.ZWEB", skeletonMesh[0]->GetBoneIDS(), defaultAnimation);
	SkeletonAni skeletonbaseMonsterAttack = ZWEBLoader::LoadSkeletonOnly("Models/baseMonsterAttack.ZWEB", skeletonMesh[0]->GetBoneIDS(), defaultAnimation);
	SkeletonAni skeletonbaseMonsterDeath = ZWEBLoader::LoadSkeletonOnly("Models/baseMonsterDeath.ZWEB", skeletonMesh[0]->GetBoneIDS(), defaultAnimation);
	SkeletonMeshComponent* baseMonsterComp = baseEnemy->AddComponent<SkeletonMeshComponent>(skeletonMesh[0], skeletonMat[0]);
	baseMonsterComp->SetAnimationTrack(skeletonbaseMonsterIdle, SkeletonStateMachine::IDLE);
	baseMonsterComp->SetAnimationTrack(skeletonbaseMonsterWalk, SkeletonStateMachine::WALK);
	baseMonsterComp->SetAnimationTrack(skeletonbaseMonsterRun, SkeletonStateMachine::RUN);
	baseMonsterComp->SetAnimationTrack(skeletonbaseMonsterAttack, SkeletonStateMachine::ATTACK);
	baseMonsterComp->SetAnimationTrack(skeletonbaseMonsterDeath, SkeletonStateMachine::DEATH);
	baseMonsterComp->BlendAnimations();
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
