#pragma once
#include "Scene.h"
#include "World.h"
#include "ParticleSystem.h"

class GUIFont;
class SpriteRenderPass;

ALIGN16
class GameScene : public Scene
{
private:
	void RemoveEnemy();
	ParticleSystem* particles;
	Object* bananaObject;
	
public:
	GameScene();
	virtual ~GameScene();

	void Initialize() override;
	void InitializeObjects() override;
	void InitializeGUI() override;
	void InitializeLights();

	void OnActivate() override;
	void OnDeactivate() override;

	void Update(const float& deltaTime) override;
	void FixedUpdate(const float& fixedDeltaTime) override;
	void Render() override;

	ALIGN16_ALLOC;

private:
	World world;
	
};