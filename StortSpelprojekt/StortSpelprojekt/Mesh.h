#pragma once
#include <d3d11.h>

#include <DirectXMath.h>
#include <vector>
#include <map>
#include <string>

#include "SkeletonAni.h"

struct Mesh
{
	struct Vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT2 uv;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT3 tangent;
		DirectX::XMFLOAT3 boneID; //The LOD level should not be over 3.
		DirectX::XMFLOAT3 skinWeight;
		Vertex() : position(0, 0, 0), uv(0, 0), normal(0, 0, 0), tangent(0, 0, 0), boneID(0,0,0), skinWeight(0,0,0) {}
		Vertex(DirectX::XMFLOAT3 position, DirectX::XMFLOAT2 uv, DirectX::XMFLOAT3 normal, DirectX::XMFLOAT3 tangent, DirectX::XMFLOAT3 boneID,
			DirectX::XMFLOAT3 skinWeight) : position(position), uv(uv), normal(normal), tangent(tangent), boneID(boneID),skinWeight(skinWeight){}
		bool operator==(const Vertex& other)
		{
			if (this->position.x == other.position.x && this->position.y == other.position.y && this->position.z == other.position.z) //this is to compare indexed verts.
			{
				return true;
			}
			else
			{
				return false;
			}
		}
	};

	std::vector<Vertex> vertexes;
	std::vector<unsigned int> indices;
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;

	//Added By Emil
	std::vector<SkeletonAni> skeletonAnis;

	Mesh() : Mesh(std::vector<Vertex>(), std::vector<unsigned int>()) {}
	Mesh(std::vector<Vertex> vertexes, std::vector<unsigned int> indices) : vertexes(vertexes), indices(indices), vertexBuffer(nullptr), indexBuffer(nullptr)
	{
		this->vertexes = vertexes;
		this->indices = indices;
		this->vertexBuffer = nullptr;
		this->indexBuffer = nullptr;

		this->skeletonAnis.clear();
	}
	void setAnimationTrack(const SkeletonAni& track)
	{
		skeletonAnis.push_back(track);
	}
	const SkeletonAni& getSkeletonAniTrack(unsigned int trackNr)
	{
		return skeletonAnis[trackNr];
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