#pragma once
#include <SpriteBatch.h>
#include <SpriteFont.h>

#include "DirectXHelpers.h"
#include "Renderer.h"
#include "RenderPass.h"
class GUICompass;

enum class GuiGroup : unsigned int
{
	None = 0,
	Default = 1 << 0,
	Lore = 1 << 1,
	Options = 1 << 2,
	HowToPlay = 1 << 3,
	Intro = 1 << 4,
	Font = 1 << 5
	
};
DEFINE_ENUM_FLAG_OPERATORS(GuiGroup)

// Virtual base class to inherit from
ALIGN16 
class GUIObject
{
public:
	GUIObject() {};
	virtual ~GUIObject() {};
	virtual void Draw(DirectX::SpriteBatch*) = 0;
	virtual void SetPosition(float x, float y) = 0;
	virtual void Update() = 0;
	void SetActivated(bool set = true) { this->active = set; };
	bool GetActivated() { return this->active; };
	void SetVisible(bool set = true) { this->visible = set; };
	bool GetVisible() { return this->visible; };
	bool HasGroup(GuiGroup flag) const;
	void AddGroup(GuiGroup flag);
	void RemoveGroup(GuiGroup flag);
	GuiGroup GetGroup(){ return this->group; };
private:
	std::string name;
	DirectX::SpriteBatch* spriteBatch; //hmm
	float xPos;
	float yPos;
	bool active;
	bool visible = true;
protected:
	GuiGroup group = GuiGroup::None;
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
	void Pass(Renderer* renderer, CameraComponent* camera, RenderTexture& inTexture, RenderTexture& outTexture) override;
	void ChangeGuiGroup(GuiGroup state);
	void ClearGui();
	void UpdateAll();

private:
	GUICompass* compass;
	ID3D11RasterizerState* testState;
	ID3D11SamplerState* samplerState;
	bool active = false;
	dx::SpriteBatch* spriteBatch;
	Renderer* renderer;
	std::unordered_map<std::string, GUIObject*> GUIObjects;
	ID3D11DepthStencilState* depthStencilState;
	ID3D11BlendState* blendOn;
};
