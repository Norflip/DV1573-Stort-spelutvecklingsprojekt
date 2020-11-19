#pragma once
#include "Component.h"
#include "GUIManager.h"
#include "GameClock.h"
#include "GUISprite.h"
#include "Scene.h"
#include "AudioMaster.h"
#include "WeaponComponent.h"

class EnemyStatsComp;

class PlayerComp :
    public Component
{
public:
	//enum NEXT_SCENE { INTRO, LOSE, GAME, WIN };

	//PlayerComp();
	PlayerComp(Renderer* renderer, CameraComponent* camComp, Object* house, Physics* physics, GUIManager* guimanager, float health, float movementSpeed, float radius, float attack, float attackSpeed);
	virtual ~PlayerComp();
	void Update(const float& deltaTime) override;
	void FixedUpdate(const float& fixedDeltaTime) override;
	float GetHealth() const { return health; }
	float GetSpeed() const { return movementSpeed; }
	float GetAttack() const { return attack; }
	float GetAttackSpeed() const { return attackSpeed; }
	float GetFuel()const { return fuel; }
	float GetFood()const { return food; }
	float GetSprintSpeed()const { return SprintSpeed; }
	float GetCrouchSpeed()const { return crouchSpeed; }
	float GetMovementspeed()const { return movementSpeed; }

	float GetRadius() const { return radius; }

	void Reset();

	void LoseHealth(float damage) { health -= damage; }	
	void AddHealth(float health) { this->health += health; }	
	void AddAttack(float attack) { this->attack += attack; }
	void SetAttackSpeed(float attackSpeed) { this->attackSpeed = attackSpeed; }
	void SetAttack(float attack) { this->attack = attack; }
	void SetHealth(float health) { this->health = health; }
	void SetMovementSpeed(float movementSpeed) { this->movementSpeed = movementSpeed; }
	void SetCrouchSpeed(float crouchSpeed) { this->crouchSpeed = crouchSpeed; }
	void SetSprintSpeed(float SprintSpeed) { this->crouchSpeed = SprintSpeed; }
	void SetFuel(float fuel) { this->fuel = fuel; }
	void SetFood(float food) { this->fuel = food; }
	CameraComponent* GetCamera(){ return cam; };
	void InsertArms(Object* arms);
	void SetguiMan(GUIManager* guiMan) { this->guiMan = guiMan; }
	void InsertWeapon(WeaponComponent* weapon, std::string name);
	void SetInteriorPosition(float x, float y, float z);
	dx::XMFLOAT3 GetInteriorPosition() { return this->interiorPosition; }

private:
	//det h�r skriver en kommentar till hold objekt
	void HoldObject();
	void DropObject();
	float health, attack, attackSpeed, fuel, food;
	//int currentWeapon;
	float movementSpeed, crouchSpeed, SprintSpeed;
	float foodLossPerSecond, fuelBurnPerMeter, healthLossPerSecond;
	float radius;
	GUIManager* guiMan;
	GUISprite* fuelDippingBar, * foodDippingBar, * healthDippingBar, * fuelBar, * foodBar, * healthBar;
	Renderer* renderer;
	POINT p;
	std::unordered_map<std::string, WeaponComponent*> weaponsList;
	EnemyStatsComp* enemyStatsComp;
	float rayDistance;	
	Object* currentWeapon;
	Object* holding;
	Object* house;
	Object* arms;
	RayHit hit;
	Physics* physics;
	CameraComponent* cam;
	//GameClock attackTimer;

	bool foodEmpty;
	bool pickedUpLastFrame;
	float throwStrength;
	float hpLossDist, maxDist, hpLossPerDistance;
	float ReverseAndClamp(float inputValue);
	float test = 0;
	void RayCast(const float& deltaTime);
	sm::Vector3 holdAngle;
	sm::Vector3 toYAW;
	sm::Matrix inverseViewMatrix, wepOffTrans, wepOffRot, wepWorld;
	sm::Vector3	weaponScale, weaponPos, up;
	sm::Quaternion weaponRot;

	dx::XMFLOAT3 interiorPosition;

	 
};

