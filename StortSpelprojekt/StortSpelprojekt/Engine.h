#pragma once
#include <chrono>
#include <thread>

#include "Window.h"
//#include "Scene.h"

#include "GameScene.h"
#include "IntroScene.h"
#include "GameOverScene.h"
#include "WinScene.h"

constexpr int FIXED_FPS = 60;
constexpr float TARGET_FIXED_DELTA = 1.0f / FIXED_FPS;

#define MULTITHREAD_PHYSICS FALSE

class Engine
{
public:
	Engine(HINSTANCE hInstance);
	virtual ~Engine();

	void Run();
	void Exit();
	bool IsRunning() const { return this->running; }

	Scene* GetActiveScene() const { return this->activeScene; }
	void RegisterScene(size_t id, Scene* scene);
	void UnregisterScene(size_t id);
	void SwitchScene(size_t id);

	Physics* GetPhysics() const { return this->physics; }
	ResourceManager* GetResources () const { return this->resourceManager; }

	void Pause(bool pause) { this->pause = pause; }
	bool IsPaused() const { return pause; }

	ALIGN16_ALLOC;
	static Engine const* Instance;

private:
	static void FixedUpdateLoop(Engine* engine);

private:
	bool running;
	std::unordered_map<size_t, Scene*> scenes;
	Scene* activeScene;
	Scene* previousScene;
	bool pause;

	Window window;
	Renderer* renderer;
	Physics* physics;
	ResourceManager* resourceManager;

};