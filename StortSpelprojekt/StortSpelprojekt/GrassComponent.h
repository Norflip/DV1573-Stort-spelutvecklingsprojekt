#pragma once
#include "Chunk.h"
#include "Mesh.h"
#include "Component.h"
#include "Texture.h"
#include "Material.h"
#include "Renderer.h"



struct GrassStraw
{
	dx::XMFLOAT4 position;
	dx::XMFLOAT4 normal;
	dx::XMFLOAT4 uv;
};

class GrassComponent : public Component
{
public:
	GrassComponent(size_t chunkTriangleCount, ID3D11Device* device, Shader* shader);
	virtual ~GrassComponent();

	void InitializeGrass(Mesh& chunkMesh, ID3D11Device* device, ID3D11DeviceContext* context);
	void Draw(Renderer* renderer, CameraComponent* camera) override;

	Material& GetMaterial();

private:
	Texture height;
	Texture diffuse;
	Texture noise;
	Material grassMat;
	Mesh grassMesh;


	ID3D11Buffer* grassBfr = nullptr;
	ID3D11ShaderResourceView* grassSrv = nullptr;

	ID3D11Buffer* grassIndexBfr = nullptr;
	ID3D11ShaderResourceView* grassIndexSrv = nullptr;

	ID3D11Buffer* grassBCBfr = nullptr;

	ID3D11ShaderResourceView* grassBCSRV = nullptr;
	ID3D11Buffer* grassCBuffer = nullptr;
	//ChunkType chunkType;
};

