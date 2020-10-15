#pragma once
#include "Scene.h"

class GUIFont;
class SpriteRenderPass;

class IntroScene : public Scene
{
private:

public:
	IntroScene(ResourceManager* manager);
	~IntroScene();

	void Initialize(Renderer* renderer) override;
	void InitializeObjects() override;
	void InitializeGUI() override;

	void OnActivate() override;
	void OnDeactivate() override;

	void Update(const float& deltaTime) override;
	void FixedUpdate(const float& fixedDeltaTime) override;
	void Render() override;
};