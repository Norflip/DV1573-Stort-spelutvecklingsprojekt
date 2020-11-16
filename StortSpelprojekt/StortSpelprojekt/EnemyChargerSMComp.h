#pragma once
//#include "SkeletonChargerMeshComponent.h"
#include <map>
#include "EnemyPatrolComp.h"
//#include "EnemyIdleComp.h"
#include "EnemyAttackComp.h"
#include "Input.h"
#include <unordered_map>

enum class EnemyChargerState
{
	NONE,
	IDLE,
	LOAD,
	RUN,
	UNLOAD,
	ATTACK
};

class EnemyChargerSMComp : public Component
{
public:
	std::map<EnemyChargerState, EnemyChargerState> switchState =
	{
		{EnemyChargerState::IDLE, EnemyChargerState::LOAD},
		{EnemyChargerState::LOAD, EnemyChargerState::RUN},
		{EnemyChargerState::RUN, EnemyChargerState::ATTACK},
		{EnemyChargerState::RUN, EnemyChargerState::UNLOAD},
		{EnemyChargerState::UNLOAD, EnemyChargerState::IDLE}
	};

public:
	EnemyChargerSMComp(EnemyChargerState state);
	virtual ~EnemyChargerSMComp();
	void SetState(EnemyChargerState state);
	void Start();
	void Initialize();
	void InitAnimation();

	void Update(const float& deltaTime);
	void RegisterState(EnemyChargerState state, Component* comp);

private:
	EnemyChargerState currentState;
	std::unordered_map<EnemyChargerState, Component*> stateMap;
	EnemyAttackComp* enemyAttackComp;
	SkeletonMeshComponent* skeletonComponent;
	EnemyStatsComp* statsComponent;

	void Animate();
	bool loadRun;
};
