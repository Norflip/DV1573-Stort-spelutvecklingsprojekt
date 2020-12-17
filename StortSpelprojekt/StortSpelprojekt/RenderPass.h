#pragma once
#include "Renderer.h"
#include "ResourceManager.h"

class RenderPass
{
public:
	enum class PassType
	{
		PRE_PASS,
		GUI,
		POST_PROCESSING
	};

public:
	RenderPass(int priority, PassType type) : priority(priority), type(type), enabled(true) {}
	virtual ~RenderPass() {}

	virtual void m_Initialize(ID3D11Device*) {};
	virtual void Pass(Renderer* renderer, CameraComponent* camera, RenderTexture& current, RenderTexture& target) = 0;

	int GetPriority() const { return this->priority; }
	bool IsEnabled() const { return this->enabled; }
	void SetEnabled(bool enabled) { this->enabled = enabled; }

	PassType GetType() const { return this->type; }

private:
	int priority;
	bool enabled;
	PassType type;
};

class PSRenderPass : public RenderPass
{
public:
	PSRenderPass(int priority, std::string pixelShaderPath, LPCSTR pixelShaderEntry = "main") : RenderPass(priority, RenderPass::PassType::POST_PROCESSING), path(pixelShaderPath), entry(pixelShaderEntry) {}
	virtual ~PSRenderPass() { delete material; }
	void m_Initialize(ID3D11Device* device) override 
	{
		Shader* shader = new Shader;
		shader->SetPixelShader(path, entry);
		shader->SetVertexShader("Shaders/ScreenQuad_vs.hlsl");
		shader->Compile(device);
		material = new Material(shader);
	}

	void Pass(Renderer* renderer, CameraComponent* camera, RenderTexture& inTexture, RenderTexture& outTexture) override
	{
		renderer->ClearRenderTarget(outTexture, false);
		renderer->SetRenderTarget(outTexture, false);

		renderer->GetContext()->PSSetShaderResources(0, 1, &inTexture.srv);
		renderer->DrawScreenQuad(material);
	}

private:
	Material* material;

	//LPCWSTR path;
	std::string path;
	LPCSTR entry;
};
