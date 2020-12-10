#pragma once
#include "SkeletonMeshComponent.h"
#include <map>
#include "EnemyPatrolComp.h"
#include "EnemyIdleComp.h"
#include "EnemyAttackComp.h"
#include "Input.h"
#include <unordered_map>
#include "EnemyManager.h"
#include "MetaProgress.h"

#define ENEMY_RADIUS_LIMIT 100.f //Kill Enemy outside of this length from player
//(kill enemies since they never will be encountered)

enum class EnemyState
{
    NONE,
    IDLE,
    PATROL,
    ATTACK,
    RUN,
    LOAD,
    UNLOAD,
    DEATH
};

class EnemySMComp : public Component
{
public:
    std::map<EnemyState, EnemyState> switchState =
    {
        {EnemyState::IDLE, EnemyState::PATROL},
        {EnemyState::IDLE, EnemyState::ATTACK},
        {EnemyState::ATTACK, EnemyState::IDLE},
        {EnemyState::PATROL, EnemyState::ATTACK},
        {EnemyState::PATROL, EnemyState::IDLE}
    };

public:
    EnemySMComp(EnemyState state);
    virtual ~EnemySMComp();
    void SetState(EnemyState state);
    void Start();
    void Initialize();
    void InitAnimation(Object* playerObj);

    void Update(const float& deltaTime);
    void RegisterState(EnemyState state, Component* comp);
private:
    EnemyState currentState;
    std::unordered_map<EnemyState, Component*> stateMap;
    //EnemyAttackComp* enemyAttackComp;
    void Animate();
    SkeletonMeshComponent* skeletonComponent;
    EnemyAttackComp* attackComponent;
    EnemyStatsComp* statsComponent;
    EnemyPatrolComp* enemyPatrolComp;
    Object* player;
    
};