#pragma once
#include "Chunk.h"
#include "Mesh.h"
#include "Component.h"
#include "Texture.h"
#include "Material.h"
#include "Renderer.h"

#include "Bounds.h"

struct GrassStraw
{
	dx::XMFLOAT4 position;
	dx::XMFLOAT4 normal;
	dx::XMFLOAT4 uv;
};

ALIGN16
class GrassComponent : public Component
{
	const size_t MAX_STRANDS = 128;


public:
	GrassComponent(size_t chunkTriangleCount, ID3D11Device* device, Shader* shader, Bounds bounds);
	virtual ~GrassComponent();

	void InitOnce(Mesh* chunkMesh, ID3D11Device* device, ID3D11Buffer** grassBfr, ID3D11ShaderResourceView** grassSrv, ID3D11Buffer** grassIndexBfr, ID3D11ShaderResourceView** grassIndexSrv
		, ID3D11Buffer** grassBCBfr, ID3D11ShaderResourceView** grassBCSRV);
	void InitializeGrass(Mesh* chunkMesh, ID3D11Device* device, ID3D11DeviceContext* context, ID3D11Buffer** grassBfr, ID3D11ShaderResourceView** grassSrv, ID3D11Buffer** grassIndexBfr
		, ID3D11ShaderResourceView** grassIndexSrv, ID3D11Buffer** grassBCBfr, ID3D11ShaderResourceView** grassBCSRV);
	void Draw(Renderer* renderer, CameraComponent* camera) override;

	Material* GetMaterial() const { return this->grassMat; }
	ALIGN16_ALLOC;

private:
	Texture height;
	Texture diffuse;
	Texture noise;
	Material* grassMat;
	Mesh* grassMesh;

	Bounds bounds;
	//ChunkType chunkType;
};

