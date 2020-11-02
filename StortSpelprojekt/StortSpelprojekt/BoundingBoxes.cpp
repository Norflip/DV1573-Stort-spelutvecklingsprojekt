#include "stdafx.h"
#include "BoundingBoxes.h"

BoundingBoxes::BoundingBoxes(Mesh* mesh)
{
	this->basicMesh = mesh;
	aabb.c = dx::XMVectorZero();// {(0, 0, 0) };
	aabb.h = dx::XMVectorZero();
	aabb.min = dx::XMFLOAT3(0, 0, 0);
	aabb.max = dx::XMFLOAT3(0, 0, 0) ;
}

BoundingBoxes::~BoundingBoxes()
{
}




void BoundingBoxes::CalcAABB(const Mesh& mesh)
{

	DirectX::XMFLOAT3 minVertex, maxVertex;
	mesh.CalculateMinMax(minVertex, maxVertex);
	
	
	aabb.max = maxVertex;
	aabb.min = minVertex;

	DirectX::XMVECTOR min = DirectX::XMLoadFloat3(&minVertex);

	DirectX::XMVECTOR max = DirectX::XMLoadFloat3(&maxVertex);

	aabb.c = DirectX::XMVectorAdd(min, max);

	aabb.c = DirectX::XMVectorScale(aabb.c, 1.0f / 2.0f);

	aabb.h = DirectX::XMVectorSubtract(max, min);

	aabb.h = DirectX::XMVectorScale(aabb.h, 1.0f / 2.0f);

	aabb.radius = dx::XMVectorGetX(dx::XMVector3Length(aabb.h));
	
	aabb.height = aabb.max.y - aabb.min.y;
	
	aabb.halfX = (aabb.max.x - aabb.min.x) * 0.5f;

	aabb.halfY = (aabb.max.y - aabb.min.y) * 0.5f;

	aabb.halfZ = (aabb.max.z - aabb.min.z) * 0.5f;
}

void BoundingBoxes::CalcAABB()
{
	DirectX::XMFLOAT3 minVertex = DirectX::XMFLOAT3(FLT_MAX, FLT_MAX, FLT_MAX);
	DirectX::XMFLOAT3 maxVertex = DirectX::XMFLOAT3(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	basicMesh->CalculateMinMax(minVertex, maxVertex);

	aabb.max = maxVertex;
	aabb.min = minVertex;

	DirectX::XMVECTOR min = DirectX::XMLoadFloat3(&minVertex);

	DirectX::XMVECTOR max = DirectX::XMLoadFloat3(&maxVertex);

	aabb.c = DirectX::XMVectorAdd(min, max);

	aabb.c = DirectX::XMVectorScale(aabb.c, 1.0f / 2.0f);

	aabb.h = DirectX::XMVectorSubtract(max, min);

	aabb.h = DirectX::XMVectorScale(aabb.h, 1.0f / 2.0f);

	
	aabb.radius = dx::XMVectorGetX(dx::XMVector3Length(aabb.h));

	aabb.height = aabb.max.y - aabb.min.y;

	aabb.halfX = (aabb.max.x - aabb.min.x) * 0.5f;

	aabb.halfY = (aabb.max.y - aabb.min.y) * 0.5f;

	aabb.halfZ = (aabb.max.z - aabb.min.z) * 0.5f;

	
}
