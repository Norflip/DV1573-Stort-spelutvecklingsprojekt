#pragma once
#include "Scene.h"

class GUIFont;
class SpriteRenderPass;

ALIGN16
class WinScene : public Scene
{
private:

public:
	WinScene();
	virtual ~WinScene();

	void Initialize() override;
	void InitializeObjects() override;
	void InitializeGUI() override;

	void OnActivate() override;
	void OnDeactivate() override;

	void Update(const float& deltaTime) override;
	void FixedUpdate(const float& fixedDeltaTime) override;
	void Render() override;

	ALIGN16_ALLOC;

};