#pragma once
#include "Scene.h"
#include "World.h"

class GUIFont;
class GUICompass;
class SpriteRenderPass;

ALIGN16
class GameScene : public Scene
{
private:
	void RemoveEnemy();
	
public:
	GameScene();
	virtual ~GameScene();

	void Initialize() override;
	void InitializeObjects() override;
	void InitializeGUI() override;
	void InitializeLights();

	void OnActivate() override;
	void OnDeactivate() override;
	void SwitchScene();

	void Update(const float& deltaTime) override;
	void FixedUpdate(const float& fixedDeltaTime) override;
	void Render() override;

	ALIGN16_ALLOC;

private:
	World world;
	Object* house;
	Object* leftSign;
	Object* rightSign;
	Object* roadSign;

	int fogId;
	float fogCol;
};