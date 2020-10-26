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

// Virtual base class to inherit from
class GUIObject
{
public:
	GUIObject() {};
	virtual ~GUIObject() {};
	virtual void Draw(DirectX::SpriteBatch*) = 0;
	virtual void SetPosition(float x, float y) = 0;
	virtual void Update() = 0;

private:
	std::string name;
	DirectX::SpriteBatch* spriteBatch; //hmm
	float xPos;
	float yPos;
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

	void UpdateAll();

private:
	dx::SpriteBatch* spriteBatch;
	Renderer* renderer;
	std::unordered_map<std::string, GUIObject*> GUIObjects;
	ID3D11DepthStencilState* depthStencilState;
	ID3D11BlendState* blendOn;
};
