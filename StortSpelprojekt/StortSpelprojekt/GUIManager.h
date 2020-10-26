#pragma once
#include <unordered_map>
#include <SpriteBatch.h>
#include <SpriteFont.h>
#include <string>

#include "DirectXHelpers.h"
#include "Renderer.h"
#include "RenderPass.h"
#include "Input.h"

namespace dx = DirectX;


enum class GuiState
{
	Lore, Options, HowToPlay, Intro
};

// Virtual base class to inherit from
class GUIObject
{
public:
	GUIObject() {};
	virtual ~GUIObject() {};
	virtual void Draw(DirectX::SpriteBatch*) = 0;
	virtual void SetPosition(float x, float y) = 0;
	virtual void Update() = 0;
	void SetActive(bool set = true) { this->active = set; };
	bool GetActive() { return this->active; };
	void SetVisible(bool set = true) { this->active = set; };
	bool GetVisible() { return this->active; };
	void SetGroup(GuiState group) { this->group = group; };

private:
	std::string name;
	DirectX::SpriteBatch* spriteBatch; //hmm
	float xPos;
	float yPos;
	bool active;
	bool visible = true;
	GuiState group;
};

class GUIManager : public RenderPass
{
public:
	GUIManager(Renderer* renderer, int priority);
	void AddGUIObject(GUIObject* addObj, std::string name);
	void AddGUIObject(GUIObject* addObj, std::string name, float x, float y);
	void AddGUIObject(std::string texture, std::string name, float x, float y);
	GUIObject* GetGUIObject(std::string name);
	void RemoveGUIObject(std::string name);
	void Pass(Renderer* renderer, RenderTexture& inTexture, RenderTexture& outTexture) override;
	void ChangeGuiState(GuiState state);
	void ClearGui();
	void UpdateAll();

private:
	
	bool active = false;
	dx::SpriteBatch* spriteBatch;
	Renderer* renderer;
	std::unordered_map<std::string, GUIObject*> GUIObjects;
	ID3D11DepthStencilState* depthStencilState;
	ID3D11BlendState* blendOn;
};
