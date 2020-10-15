#pragma once
#include "Scene.h"

class IntroScene : public Scene
{
private:

public:
	IntroScene();
	~IntroScene();

	void Initialize(Renderer* renderer) override;
	void InitializeObjects() override;

	void OnActivate() override {}
	void OnDeactivate() override {}

	void Update(const float& deltaTime) override;
	void FixedUpdate(const float& fixedDeltaTime) override;
	void Render() override;
};