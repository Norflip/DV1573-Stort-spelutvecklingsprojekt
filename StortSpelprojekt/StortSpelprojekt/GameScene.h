#pragma once
#include "Scene.h"
#include "World.h"

class GUIFont;
class GUICompass;
class SpriteRenderPass;

constexpr unsigned int LAST_SEGMENT = 3;

ALIGN16
class GameScene : public Scene
{	


public:
	GameScene();
	virtual ~GameScene();

	void Initialize() override;
	void InitializeObjects() override;
	void InitializeGUI() override;
	void InitializeInterior();
	void OnActivate() override;
	void OnDeactivate() override;
	void SetSignPositions(SaveState& state);

	void Update(const float& deltaTime) override;
	void FixedUpdate(const float& fixedDeltaTime) override;
	void Render() override;

#if USE_IMGUI
	void OnIMGUIFrame() override;
#endif
	static bool immortal;
	static bool drawColliders;

	ALIGN16_ALLOC;

private:
	World world;
	Object* house;

	Particlesys* testParticles;

	dx::XMFLOAT3 interiorPosition;
	Object* leftSign;
	Object* rightSign;
	Object* roadSign;
	NodeWalkerComp* nodeWalker;

	float fogCol;

	bool onceTest = true;
	bool end;
};