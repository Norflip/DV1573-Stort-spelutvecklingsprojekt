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
	PSRenderPass(int priority, std::string pixelShaderPath, LPCSTR pixelShaderEntry = "main") : RenderPass(priority), path(pixelShaderPath), entry(pixelShaderEntry) {}

	void m_Initialize(ID3D11Device* device) override 
	{
		Shader* shader = new Shader;
		shader->SetPixelShader(path, entry);
		shader->SetVertexShader("Shaders/ScreenQuad_vs.hlsl");
		shader->Compile(device);
		material = Material(shader);
	}

	bool Pass(Renderer* renderer, RenderTexture& inTexture, RenderTexture& outTexture) override
	{
		renderer->DrawScreenQuad(material);
		return true;
	}

private:
	Material material;

	//LPCWSTR path;
	std::string path;
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

