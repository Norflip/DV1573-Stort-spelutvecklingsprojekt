#include "stdafx.h"
#include "FireTextureComponent.h"

FireTextureComponent::FireTextureComponent(Renderer* renderer, Shader* shader)
{
	this->renderer = renderer;
	this->fireShader = shader;
	this->meshMaterial = new Material(fireShader);

}

FireTextureComponent::~FireTextureComponent()
{
}

void FireTextureComponent::Initialize(ID3D11Device* device)
{
}

void FireTextureComponent::Shutdown()
{
}

void FireTextureComponent::Update(const float& deltaTime)
{
}

void FireTextureComponent::Draw(Renderer* renderer, CameraComponent* camera)
{
}

void FireTextureComponent::LoadTextures(ID3D11Device* device)
{
}

void FireTextureComponent::InitializeBuffers(ID3D11Device* device)
{
}
