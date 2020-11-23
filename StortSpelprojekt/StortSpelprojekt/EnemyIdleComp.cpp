#include "stdafx.h"
#include "EnemyIdleComp.h"

EnemyIdleComp::EnemyIdleComp()
{
    randomIntervall = (rand() % 4);
    timer = 0.f;
    //this->enemyPatrolComp->SetMoving(true);
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
