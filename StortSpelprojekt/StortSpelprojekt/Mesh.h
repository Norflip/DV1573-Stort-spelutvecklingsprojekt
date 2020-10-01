#pragma once
#include <d3d11.h>  
#include <DirectXMath.h>
#include <vector>
#include <map>
#include <string>
#include "SkeletonAni.h"
#include "DXHelper.h"


// denna b�r ses �ver.. sudo class d�r allt �r public. G�r mot kodstandard.

struct Mesh
{
	struct InstanceData // can add more stuff in here, like color.
	{
		dx::XMFLOAT4X4 instanceWorld;
		dx::XMFLOAT3 instancePosition;
	};


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
	
	size_t instances = 0;
	dx::XMFLOAT3 scale;
	dx::XMFLOAT3 rotation;
	dx::XMFLOAT3 translation;
	std::string meshName= "null";
	std::string materialName = "null"; //Only one material per mesh. If someone knows how to use multiple let me know.
	std::map<std::string, unsigned int> boneIDMap;

public:
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<InstanceData> instanceData;
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	ID3D11Buffer* instanceBuffer;
	ID3D11Buffer* vertexAndInstance[2];
	D3D11_PRIMITIVE_TOPOLOGY topology;


	Mesh(ID3D11Device* device, std::vector<Vertex> vertices, std::vector<unsigned int> indices, D3D11_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST) : 
		vertices(vertices), indices(indices), vertexBuffer(nullptr), indexBuffer(nullptr), boneIDMap(),meshName(),materialName(),scale(0,0,0), rotation(0, 0, 0), translation(0, 0, 0)
	{
		this->vertices = vertices;
		this->indices = indices;
		this->topology = topology;
		this->instanceData.clear();

		DXHelper::CreateVertexBuffer(device, vertices.size(), sizeof(Mesh::Vertex), vertices.data(), &vertexBuffer);
		DXHelper::CreateIndexBuffer(device, indices.size(), indices.data(), &indexBuffer);
	}
	Mesh(ID3D11Device* device, std::vector<Vertex> vertices, std::vector<InstanceData>& instanceData, std::vector<unsigned int> indices, D3D11_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST) :
		vertices(vertices), indices(indices), vertexBuffer(nullptr), indexBuffer(nullptr), boneIDMap(), meshName(), materialName(), scale(0, 0, 0), rotation(0, 0, 0), translation(0, 0, 0)
	{
		this->vertices = vertices;
		this->indices = indices;
		this->topology = topology;
		this->instanceData = instanceData;
		DXHelper::CreateVertexBuffer(device, vertices.size(), sizeof(Mesh::Vertex), vertices.data(), &vertexBuffer);
		DXHelper::CreateIndexBuffer(device, indices.size(), indices.data(), &indexBuffer);
		DXHelper::CreateInstanceBuffer(device, instanceData.size(), sizeof(Mesh::InstanceData), instanceData.data(), &instanceBuffer);
		vertexAndInstance[0] = vertexBuffer;
		vertexAndInstance[1] = instanceBuffer;
	}
	Mesh()
	{
		this->vertices.clear();
		this->indices.clear();
		this->instanceData.clear();
	}
	void CreateInstanceBuffer(ID3D11Device* device, std::vector<InstanceData>& instanceData)
	{
		this->instanceData = instanceData;
		DXHelper::CreateInstanceBuffer(device, instanceData.size(), sizeof(Mesh::InstanceData), instanceData.data(), &instanceBuffer);
		
		vertexAndInstance[0] = vertexBuffer;
		vertexAndInstance[1] = instanceBuffer;
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

	void SetSRT(const dx::XMFLOAT3& S, const dx::XMFLOAT3& R, const dx::XMFLOAT3& T)
	{
		scale = S;
		rotation = R;
		translation = T;
	}
	const dx::XMFLOAT3& GetS() const
	{
		return scale;
	}
	const dx::XMFLOAT3& GetR() const
	{
		return rotation;
	}
	const dx::XMFLOAT3& GetT() const
	{
		return translation;
	}
	
	void SetMaterialName(const std::string& name)
	{
		materialName = name;
	}

	const std::string& GetMaterialName()const
	{
		return materialName;
	}

	void SetInstanceNr(size_t nr)
	{
		this->instances = nr;
	}
	size_t GetInstanceNr() const
	{
		return this->instances;
	}
	ID3D11Buffer** GetVertexAndInstanceBfr()
	{
		vertexAndInstance[0] = vertexBuffer;
		vertexAndInstance[1] = instanceBuffer;
		return vertexAndInstance;
	}

	// DECONSTRUCTOR?! WAT DO 
	void Release()
	{
		if (vertexBuffer)
			vertexBuffer->Release();
		if (indexBuffer)
			indexBuffer->Release();
		if (instanceBuffer)
			instanceBuffer->Release();
		if (vertexAndInstance[0])
			vertexAndInstance[0]->Release();
		if (vertexAndInstance[1])
			vertexAndInstance[1]->Release();
		vertexBuffer = indexBuffer = instanceBuffer = vertexAndInstance[0] = vertexAndInstance[1] = nullptr;
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
