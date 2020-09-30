#pragma once
#include "Renderer.h"
#include "Texture.h"
#include "GUISprite.h"
class RenderPass
{
public:
	RenderPass(int priority) : priority(priority), enabled(true) {}
	virtual ~RenderPass() {}

	virtual void m_Initialize(ID3D11Device*) {};
	virtual bool Pass(Renderer* renderer, RenderTexture& current, RenderTexture& target) = 0;

	int GetPriority() const { return this->priority; }
	bool IsEnabled() const { return this->enabled; }
	void SetEnabled(bool enabled) { this->enabled = enabled; }

private:
	int priority;
	bool enabled;
};

class PSRenderPass : public RenderPass
{
public:
	PSRenderPass(int priority, LPCWSTR pixelShaderPath, LPCSTR pixelShaderEntry = "main") : RenderPass(priority), path(pixelShaderPath), entry(pixelShaderEntry) {}

	void m_Initialize(ID3D11Device* device) override 
	{
		shader.SetPixelShader(path, entry);
		shader.SetVertexShader(L"Shaders/ScreenQuad_vs.hlsl");
		shader.Compile(device);
	}

	bool Pass(Renderer* renderer, RenderTexture& inTexture, RenderTexture& outTexture) override
	{
		renderer->DrawScreenQuad(shader);
		return true;
	}

private:
	Shader shader;
	LPCWSTR path;
	LPCSTR entry;
};

// GUIMANAGER

class SpriteRenderPass : public RenderPass
{
public:
	SpriteRenderPass(int priority,GUISprite* spiriteTest): RenderPass(priority), spiriteTest(spiriteTest){}
	
	bool Pass(Renderer* renderer, RenderTexture& inTexture, RenderTexture& outTexture) override
	{
		spiriteTest->Draw();
		return true;
	}
private:
	GUISprite* spiriteTest;
};

