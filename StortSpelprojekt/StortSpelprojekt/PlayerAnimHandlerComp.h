#pragma once
//#include "SkeletonMeshComponent.h"
//#include "Mesh.h"
//#include "Material.h"
//#include "ControllerComp.h"
#include "DXHelper.h"
class ControllerComp;
class SkeletonMeshComponent;

class PlayerAnimHandlerComp : public Component
{
public:
	PlayerAnimHandlerComp(SkeletonMeshComponent* skeletonMeshComp, Object* camObject, Object* playerObject);
	virtual~PlayerAnimHandlerComp();

	void Initialize();
	void Update(const float& deltaTime);

	void Animate(float time);
	void SetPosition();
private:
	CameraComponent* camComp;
	ControllerComp* controlComp;

	dx::XMVECTOR armsPos;
	dx::XMVECTOR armsRot;
	dx::XMVECTOR armsScale;
	dx::XMVECTOR up;
	dx::XMMATRIX armsWorld, armsOffTrans, armsOffRot;
	dx::XMMATRIX inverseViewMatrix;

	Object* camObject;
	Object* playerObject;
	SkeletonMeshComponent* skeletonMeshComp;

	bool attacking;
	float attackTimer = 0;
	float attackCooldown = 0;
	
};