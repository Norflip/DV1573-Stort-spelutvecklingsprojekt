#pragma once
#include "component.h"
#include "CameraComponent.h"
#include "Object.h"
#include "DXHelper.h"
#include "Input.h"
#include "AudioMaster.h"
class SkeletonAni;

namespace dx = DirectX;

ALIGN16 
class WeaponComponent : public Component
{
public:
	WeaponComponent(SkeletonMeshComponent* meshComp);

	virtual void Initialize() override;
	virtual void Update(const float& deltaTime) override;

	ALIGN16_ALLOC;

private:
	SkeletonMeshComponent* skeleton;
	void SetPosition(float time);
};