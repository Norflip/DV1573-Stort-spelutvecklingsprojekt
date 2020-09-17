#pragma once
#include <d3d11.h>  
#include <DirectXMath.h>
#include <vector>
#include <map>
#include <string>

#include "DXHelper.h"


// denna bör ses över.. sudo class där allt är public. Går mot kodstandard.

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
};