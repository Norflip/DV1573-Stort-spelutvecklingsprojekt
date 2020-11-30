#pragma once
#include "Object.h"
#include "EnemyPatrolComp.h"
class EnemyIdleComp : public Component
{
public:
    EnemyIdleComp();
    virtual ~EnemyIdleComp();
    void Initialize();
    void Update(const float& deltaTime) override;
private:
    float randomIntervall;
    float timer;
    EnemyPatrolComp* enemyPatrolComp;
};