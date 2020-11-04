#pragma once
#include <d3d11.h>
#include "SkeletonAni.h"
#include "DXHelper.h"
#include "Resource.h"

#define DEFAULT_TOPOLOGY D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST

ALIGN16
class Mesh : public Resource
{
public:
	struct InstanceData
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

		bool operator==(const Vertex& other) { return Mesh::CompareVertex(*this, other); }
	};

public:

	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, D3D11_PRIMITIVE_TOPOLOGY topology = DEFAULT_TOPOLOGY);
	virtual ~Mesh();

	void Initialize(ID3D11Device* device);

	ID3D11Buffer* GetVertexBuffer() const { return this->vertexBuffer; }
	ID3D11Buffer* GetIndexBuffer() const { return this->indexBuffer; }
	

	size_t GetIndexCount() const { return this->indices.size(); }
	dx::XMMATRIX GetTransformMatrix() const { return transform; }

	D3D11_PRIMITIVE_TOPOLOGY GetTopology() const { return this->topology; }
	
	size_t GetTriangleCount() const { return this->GetIndexCount() / 3; }


	std::vector<Vertex>& GetVertices() { return this->vertices; }
	std::vector<unsigned int>& GetIndices() { return this->indices; }
	
	void SetSRT(const dx::XMFLOAT3& S, const dx::XMFLOAT3& R, const dx::XMFLOAT3& T);
	void SetSRT(const dx::XMVECTOR& S, const dx::XMVECTOR& R, const dx::XMVECTOR& T);
	dx::XMVECTOR GetS() const;
	dx::XMVECTOR GetR() const;
	dx::XMVECTOR GetT() const;
	std::map<std::string, unsigned int>& GetBoneIDS() { return boneIDMap; }
	void SetBoneIDS(std::map<std::string, unsigned int> bones){ boneIDMap = bones; }
	void SetMeshName(std::string name) { this->name = name; }
	std::string GetMeshName() { return this->name; }

	void CalculateMinMax(dx::XMFLOAT3& min, dx::XMFLOAT3& max) const;
	
	static inline bool CompareVertex(Vertex left, Vertex right){ return (left.position.x == right.position.x && left.position.y == right.position.y && left.position.z == right.position.z); }
	
	ALIGN16_ALLOC;

private:
	std::string name;
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;

	D3D11_PRIMITIVE_TOPOLOGY topology;
	dx::XMMATRIX transform;
	bool initialized;

	std::map<std::string, unsigned int> boneIDMap;

};
