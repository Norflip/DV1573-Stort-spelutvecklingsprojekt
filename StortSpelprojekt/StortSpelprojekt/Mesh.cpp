#include "stdafx.h"


Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, D3D11_PRIMITIVE_TOPOLOGY topology)
	: vertices(vertices), indices(indices), topology(topology), initialized(false), transform(dx::XMMatrixIdentity()), indexBuffer(nullptr), vertexBuffer(nullptr)
{

}

Mesh::~Mesh()
{
	RELEASE(vertexBuffer);
	RELEASE(indexBuffer);
}

void Mesh::Initialize(ID3D11Device* device)
{
	if (!initialized)
	{
		assert(vertices.size() != 0);
		assert(indices.size() != 0);

		DXHelper::CreateVertexBuffer(device, vertices.size(), sizeof(Mesh::Vertex), vertices.data(), &vertexBuffer);
		DXHelper::CreateIndexBuffer(device, indices.size(), indices.data(), &indexBuffer);
		this->initialized = true;
	}
}


void Mesh::SetSRT(const dx::XMFLOAT3& S, const dx::XMFLOAT3& R, const dx::XMFLOAT3& T)
{
	SetSRT(dx::XMLoadFloat3(&S), dx::XMLoadFloat3(&R), dx::XMLoadFloat3(&T));
}

void Mesh::SetSRT(const dx::XMVECTOR& S, const dx::XMVECTOR& R, const dx::XMVECTOR& T)
{
	this->transform = dx::XMMatrixScalingFromVector(S) * dx::XMMatrixRotationQuaternion(R) * dx::XMMatrixTranslationFromVector(T);
}

dx::XMVECTOR Mesh::GetS() const
{
	dx::XMVECTOR pos, rot, scale;
	dx::XMMatrixDecompose(&scale, &rot, &pos, transform);
	return scale;
}

dx::XMVECTOR Mesh::GetR() const
{
	dx::XMVECTOR pos, rot, scale;
	dx::XMMatrixDecompose(&scale, &rot, &pos, transform);
	return rot;
}

dx::XMVECTOR Mesh::GetT() const
{
	dx::XMVECTOR pos, rot, scale;
	dx::XMMatrixDecompose(&scale, &rot, &pos, transform);
	return pos;
}

void Mesh::CalculateMinMax(dx::XMFLOAT3& min, dx::XMFLOAT3& max) const
{
	size_t vertexCount = vertices.size();
	if (vertexCount > 0)
	{
		min = vertices[0].position;
		max = vertices[0].position;

		for (size_t v = 1; v < vertices.size(); v++)
		{
			min.x = std::min(min.x, vertices[v].position.x);    // Find smallest x value in model
			min.y = std::min(min.y, vertices[v].position.y);    // Find smallest y value in model
			min.z = std::min(min.z, vertices[v].position.z);    // Find smallest z value in model

			//Get the largest vertex 
			max.x = std::max(max.x, vertices[v].position.x);    // Find largest x value in model
			max.y = std::max(max.y, vertices[v].position.y);    // Find largest y value in model
			max.z = std::max(max.z, vertices[v].position.z);    // Find largest z value in model
		}
	}
}
