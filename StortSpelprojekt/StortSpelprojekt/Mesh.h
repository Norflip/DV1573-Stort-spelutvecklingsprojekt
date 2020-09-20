#pragma once
#include <d3d11.h>  
#include <DirectXMath.h>
#include <vector>
#include <map>
#include <string>

#include "DXHelper.h"


// denna b�r ses �ver.. sudo class d�r allt �r public. G�r mot kodstandard.

struct Mesh
{
	struct Vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT2 uv;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT3 tangent;
	};

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	D3D11_PRIMITIVE_TOPOLOGY topology;

	Mesh(ID3D11Device* device, std::vector<Vertex> vertices, std::vector<unsigned int> indices, D3D11_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST) : 
		vertices(vertices), indices(indices), vertexBuffer(nullptr), indexBuffer(nullptr)
	{
		this->vertices = vertices;
		this->indices = indices;
		this->topology = topology;

		DXHelper::CreateVertexBuffer(device, vertices.size(), sizeof(Mesh::Vertex), vertices.data(), &vertexBuffer);
		DXHelper::CreateIndexBuffer(device, indices.size(), indices.data(), &indexBuffer);
	}

	// DECONSTRUCTOR?! WAT DO 
	void Release()
	{
		if (vertexBuffer)
			vertexBuffer->Release();
		if (indexBuffer)
			indexBuffer->Release();

		vertexBuffer = indexBuffer = nullptr;
	}

	static Mesh CreateScreenQuad(ID3D11Device* device)
	{
		const float size = 1.0f;
		std::vector<Vertex> vertices =
		{
			Vertex{{-size, -size, 0} , {0 ,1 },  {0,0,0} , { 0,0,0 } },		// 0,0
			Vertex{{ size, -size, 0 }, { 1,1 }, { 0,0,0 }, { 0,0,0 }},		// 0, w
			Vertex{{ size, size, 0 }, { 1,0 }, { 0,0,0 }, { 0,0,0 }},		// h, w
			Vertex{{ -size, size, 0 }, { 0,0 }, { 0,0,0 }, { 0,0,0 }}		// h, 0
		};

		std::vector<unsigned int> indices = { 3,2,1, 3,1,0 };
		return Mesh(device, vertices, indices);
	}	
};

