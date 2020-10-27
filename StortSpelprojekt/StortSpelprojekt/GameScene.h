#pragma once
#include "Scene.h"
#include "World.h"

class GUIFont;
class SpriteRenderPass;

class GameScene : public Scene
{
private:
	void RemoveEnemy();

	Object* bananaObject;
public:
	GameScene(ResourceManager* manager);
	~GameScene();

	void Initialize(Renderer* renderer) override;
	void InitializeObjects() override;
	void InitializeGUI() override;
	void InitializeLights();

	void OnActivate() override;
	void OnDeactivate() override;

	void Update(const float& deltaTime) override;
	void FixedUpdate(const float& fixedDeltaTime) override;
	void Render() override;
private:
	World world;
};