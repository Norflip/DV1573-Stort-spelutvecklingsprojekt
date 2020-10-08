#pragma once
#include <DirectXMath.h>
namespace dx = DirectX;
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

class GrassComponent :
    public Component
{
public:
	GrassComponent(ID3D11Device* device, std::vector<Mesh::Vertex>& vertex, std::vector<unsigned int>& index, Shader* shader);
	virtual ~GrassComponent();

	void InitializeGrass(std::vector<Mesh::Vertex>& vertices, std::vector<unsigned int>& indices, ID3D11Device* device, ID3D11DeviceContext* context);

	void Draw(Renderer* renderer, CameraComponent* camera) override;

	Material& GetMaterial();

	void SetModel(dx::XMMATRIX& model);


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
	
};

