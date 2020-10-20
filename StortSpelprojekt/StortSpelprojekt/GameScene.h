#pragma once
#include "Scene.h"

class GUIFont;
class SpriteRenderPass;

class GameScene : public Scene
{
private:

public:
	GameScene(ResourceManager* manager);
	~GameScene();

	void Initialize(Renderer* renderer) override;
	void InitializeObjects() override;
	void InitializeGUI() override;

	void OnActivate() override;
	void OnDeactivate() override;

	void Update(const float& deltaTime) override;
	void FixedUpdate(const float& fixedDeltaTime) override;
	void Render() override;

};