#pragma once
#include <d3d11.h>  
#include <DirectXMath.h>
#include <vector>
#include <map>
#include <string>

struct Mesh
{
	struct Vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT2 uv;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT3 tangent;
		Vertex() : position(0, 0, 0), uv(0, 0), normal(0, 0, 0), tangent(0, 0, 0) {}
		Vertex(DirectX::XMFLOAT3 position, DirectX::XMFLOAT2 uv, DirectX::XMFLOAT3 normal, DirectX::XMFLOAT3 tangent) : position(position), uv(uv), normal(normal), tangent(tangent) {}
	};

	std::vector<Vertex> vertexes;
	std::vector<unsigned int> indices;
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;

	Mesh() : Mesh(std::vector<Vertex>(), std::vector<unsigned int>()) {}
	Mesh(std::vector<Vertex> vertexes, std::vector<unsigned int> indices) : vertexes(vertexes), indices(indices), vertexBuffer(nullptr), indexBuffer(nullptr)
	{
		this->vertexes = vertexes;
		this->indices = indices;
		this->vertexBuffer = nullptr;
		this->indexBuffer = nullptr;
	}

	void Release()
	{
		if (vertexBuffer)
			vertexBuffer->Release();
		if (indexBuffer)
			indexBuffer->Release();

		vertexBuffer = indexBuffer = nullptr;
	}
};

namespace MeshCreator
{
	inline Mesh CreateMesh(std::vector<Mesh::Vertex> vertices, std::vector<unsigned int> indices, ID3D11Device* device)
	{
		Mesh mesh(vertices, indices);

		// creates vertex buffer
		D3D11_BUFFER_DESC vertexBufferDescription;
		ZeroMemory(&vertexBufferDescription, sizeof(vertexBufferDescription));
		vertexBufferDescription.Usage = D3D11_USAGE_DEFAULT;
		vertexBufferDescription.ByteWidth = (UINT)(sizeof(Mesh::Vertex) * mesh.vertexes.size());
		vertexBufferDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA vertexBuffer_subResource;
		ZeroMemory(&vertexBuffer_subResource, sizeof(D3D11_SUBRESOURCE_DATA));
		vertexBuffer_subResource.pSysMem = vertices.data();
		vertexBuffer_subResource.SysMemPitch = 0;
		vertexBuffer_subResource.SysMemSlicePitch = 0;

		HRESULT vertexBufferResult = device->CreateBuffer(&vertexBufferDescription, &vertexBuffer_subResource, &mesh.vertexBuffer);
		assert(SUCCEEDED(vertexBufferResult));

		// creates index buffer
		D3D11_BUFFER_DESC indexBufferDescription;
		indexBufferDescription.Usage = D3D11_USAGE_DEFAULT;
		indexBufferDescription.ByteWidth = (UINT)(sizeof(unsigned int) * indices.size());
		indexBufferDescription.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDescription.CPUAccessFlags = 0;
		indexBufferDescription.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA indexBuffer_subResource;
		indexBuffer_subResource.pSysMem = indices.data();
		indexBuffer_subResource.SysMemPitch = 0;
		indexBuffer_subResource.SysMemSlicePitch = 0;

		HRESULT indexBufferResult = device->CreateBuffer(&indexBufferDescription, &indexBuffer_subResource, &mesh.indexBuffer);
		assert(SUCCEEDED(indexBufferResult));
		return mesh;
	}
}