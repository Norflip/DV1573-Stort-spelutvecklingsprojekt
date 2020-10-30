#include "stdafx.h"
#include "GrassComponent.h"


GrassComponent::GrassComponent(size_t chunkTriangleCount, ID3D11Device* device, Shader* shader)
	: grassMat(shader)
{
	std::vector<Mesh::Vertex> grassV;
	std::vector<unsigned int> grassI;

	Mesh::Vertex v;
	grassV.push_back(v);

	for (size_t triangle = 0; triangle < chunkTriangleCount; triangle++)
	{
		grassI.push_back(triangle);
	}

	grassMesh = Mesh(device, grassV, grassI, D3D11_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST);

	height.LoadTexture(device, L"Textures/grassHeight.png");
	diffuse.LoadTexture(device, L"Textures/grassDiff.png");
	noise.LoadTexture(device, L"Textures/noise.png");


	grassMat.SetTexture(diffuse, TEXTURE_DIFFUSE_SLOT, ShaderBindFlag::DOMAINS);
	grassMat.SetTexture(noise, TEXTURE_NOISE_SLOT, ShaderBindFlag::DOMAINS);
	grassMat.SetTexture(height, TEXTURE_HEIGHT_SLOT, ShaderBindFlag::HULL);
	grassMat.SetTexture(height, TEXTURE_HEIGHT_SLOT, ShaderBindFlag::DOMAINS);



	auto sampler = DXHelper::CreateSampler(D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_WRAP, device);
	grassMat.SetSampler(sampler, 0, ShaderBindFlag::HULL);
	grassMat.SetSampler(sampler, 0, ShaderBindFlag::DOMAINS);
	grassMat.SetSampler(sampler, 0, ShaderBindFlag::PIXEL);

}

GrassComponent::~GrassComponent()
{
	if (grassBfr)
	{
		grassBfr->Release();
		grassBfr = nullptr;
	}
	if (grassSrv)
	{
		grassSrv->Release();
		grassSrv = nullptr;
	}
	if (grassIndexBfr)
	{
		grassIndexBfr->Release();
		grassIndexBfr = nullptr;
	}
	if (grassIndexSrv)
	{
		grassIndexSrv->Release();
		grassIndexSrv = nullptr;
	}

	if (grassBCBfr)
	{
		grassBCBfr->Release();
		grassBCBfr = nullptr;
	}
	if (grassBCSRV)
	{
		grassBCSRV->Release();
		grassBCSRV = nullptr;
	}
	if (grassCBuffer)
	{

		grassCBuffer->Release();
		grassCBuffer = nullptr;
	}
}

void GrassComponent::InitializeGrass(Mesh& chunkMesh, ID3D11Device* device, ID3D11DeviceContext* context)
{
	std::vector<Mesh::Vertex>& vertices = chunkMesh.vertices;
	std::vector<unsigned int>& indices = chunkMesh.indices;

	std::vector<GrassStraw> pData(vertices.size());
	std::vector<dx::XMFLOAT4> indexData(indices.size());

	for (size_t i = 0; i < vertices.size(); i++)
	{
		dx::XMFLOAT3 position = vertices[i].position;
		dx::XMFLOAT2 uv = vertices[i].uv;
		dx::XMFLOAT3 normal = vertices[i].normal;

		pData[i].position.x = position.x;
		pData[i].position.y = position.y;
		pData[i].position.z = position.z;
		pData[i].uv.x = uv.x;
		pData[i].uv.y = uv.y;
		pData[i].normal.x = normal.x;
		pData[i].normal.y = normal.y;
		pData[i].normal.z = normal.z;


	}

	DXHelper::CreateStructuredBuffer(device, &grassBfr, pData, sizeof(GrassStraw), pData.size(), &grassSrv);

	DXHelper::BindStructuredBuffer(context, grassBfr, pData, GRASS_STRAWS_SRV_SLOT, ShaderBindFlag::VERTEX, &grassSrv);
	DXHelper::BindStructuredBuffer(context, grassBfr, pData, GRASS_STRAWS_SRV_SLOT, ShaderBindFlag::HULL, &grassSrv);
	DXHelper::BindStructuredBuffer(context, grassBfr, pData, GRASS_STRAWS_SRV_SLOT, ShaderBindFlag::DOMAINS, &grassSrv);

	for (size_t i = 0; i < indices.size(); i++)
	{
		indexData[i].x = (float)indices[i];
	}

	DXHelper::CreateStructuredBuffer(device, &grassIndexBfr, indexData, sizeof(dx::XMFLOAT4), indices.size(), &grassIndexSrv);

	DXHelper::BindStructuredBuffer(context, grassIndexBfr, indexData, GRASS_INDICES_SRV_SLOT, ShaderBindFlag::VERTEX, &grassIndexSrv);
	DXHelper::BindStructuredBuffer(context, grassIndexBfr, indexData, GRASS_INDICES_SRV_SLOT, ShaderBindFlag::HULL, &grassIndexSrv);
	DXHelper::BindStructuredBuffer(context, grassIndexBfr, indexData, GRASS_INDICES_SRV_SLOT, ShaderBindFlag::DOMAINS, &grassIndexSrv);

	std::vector<dx::XMFLOAT4> bcData(MAX_STRANDS);
	for (size_t i = 0; i < MAX_STRANDS; i++) //maximum strands per triangle.
	{
		float barycentric1 = rand() / (float)(RAND_MAX);
		float barycentric2 = rand() / (float)(RAND_MAX);
		float barycentric3 = 0;


		if (barycentric1 + barycentric2 >= 1)
		{
			barycentric1 = 1 - barycentric1;
			barycentric2 = 1 - barycentric2;
		}
		barycentric3 = 1 - barycentric1 - barycentric2;

		dx::XMFLOAT4 barycentricCoord;
		barycentricCoord.x = barycentric1;
		barycentricCoord.y = barycentric2;
		barycentricCoord.z = barycentric3;

		bcData[i] = barycentricCoord;
	}

	DXHelper::CreateStructuredBuffer(device, &grassBCBfr, bcData, sizeof(dx::XMFLOAT4), MAX_STRANDS, &grassBCSRV);
	DXHelper::BindStructuredBuffer(context, grassBCBfr, bcData, GRASS_COORD_SRV_SLOT, ShaderBindFlag::DOMAINS, &grassBCSRV);

	float pixelScale = tanf(0.5f * (dx::XM_PI / 2.0f)) / (float)(900); //the height of the window.

	cb_grass grassCBufferData;
	grassCBufferData.pixelSize = pixelScale;
	grassCBufferData.grassDisplacement = 2;
	grassCBufferData.grassRadius = 0.5;
	grassCBufferData.grassWidth = 1.5;

	DXHelper::CreateConstBuffer(device, &grassCBuffer, &grassCBufferData, sizeof(cb_grass));
	DXHelper::BindConstBuffer(context, grassCBuffer, &grassCBufferData, CB_GRASS_PARAMETERS_SLOT, ShaderBindFlag::DOMAINS);
	DXHelper::BindConstBuffer(context, grassCBuffer, &grassCBufferData, CB_GRASS_PARAMETERS_SLOT, ShaderBindFlag::GEOMETRY);

}

void GrassComponent::Draw(Renderer* renderer, CameraComponent* camera)
{


	renderer->DrawGrass(*camera, grassMesh, grassMat, this->GetOwner()->GetTransform().GetWorldMatrix());

}

Material& GrassComponent::GetMaterial()
{
	return grassMat;
}


