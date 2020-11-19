#include "stdafx.h"
#include "EnemyIdleComp.h"

EnemyIdleComp::EnemyIdleComp()
{
	randomIntervall = (rand() % 4);
}

EnemyIdleComp::~EnemyIdleComp()
{
}

void EnemyIdleComp::Initialize()
{
	enemyPatrolComp = GetOwner()->GetComponent<EnemyPatrolComp>();
}

void EnemyIdleComp::Update(const float& deltaTime)
{
	//std::cout << "In idle state" << std::endl;

	if (timer >= randomIntervall)
	{
		timer = 0;
		randomIntervall = (rand() % 4);
		this->enemyPatrolComp->SetMoving(true);
		
	}
	else
	{
		timer += deltaTime;
	}
}
