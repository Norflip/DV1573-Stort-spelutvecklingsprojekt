#pragma once
#include <d3d11.h>  
#include <DirectXMath.h>
#include <vector>
#include <map>
#include <string>
#include "SkeletonAni.h"
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
		DirectX::XMFLOAT3 binormal;
		DirectX::XMUINT3 boneID;
		DirectX::XMFLOAT3 skinWeight;
		bool operator==(const Vertex& other)
		{

			return (this->position.x == other.position.x && this->position.y == other.position.y && this->position.z == other.position.z); //To convert from indices, positions need to be compared.
			
		}
	};
private:
	dx::XMMATRIX worldMatrix;
	std::string meshName= "null";
	std::string materialName = "null"; //Only one material per mesh.
	std::map<std::string, unsigned int> boneIDMap;

public:
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	D3D11_PRIMITIVE_TOPOLOGY topology;


	Mesh(ID3D11Device* device, std::vector<Vertex> vertices, std::vector<unsigned int> indices, D3D11_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST) : 
		vertices(vertices), indices(indices), vertexBuffer(nullptr), indexBuffer(nullptr), boneIDMap(),meshName(),materialName(),worldMatrix()
	{
		this->vertices = vertices;
		this->indices = indices;
		this->topology = topology;

		DXHelper::CreateVertexBuffer(device, vertices.size(), sizeof(Mesh::Vertex), vertices.data(), &vertexBuffer);
		DXHelper::CreateIndexBuffer(device, indices.size(), indices.data(), &indexBuffer);
	}

	std::map<std::string, unsigned int>& GetBoneIDS()
	{
		return boneIDMap;
	}
	void SetBoneIDS(std::map<std::string, unsigned int> bones)
	{
		boneIDMap = bones;
	}

	void SetMeshName(const std::string& name)
	{
		meshName = name;
	}

	std::string& GetMeshName()
	{
		return meshName;
	}

	void SetWorldMatrix(const dx::XMMATRIX& matrix)
	{
		worldMatrix = matrix;
	}

	dx::XMMATRIX& GetWorldMatrix()
	{
		return worldMatrix;
	}

	void SetMaterialName(const std::string& name)
	{
		materialName = name;
	}

	const std::string& GetMaterialName()const
	{
		return materialName;
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