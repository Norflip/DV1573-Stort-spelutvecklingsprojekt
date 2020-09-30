#include "BoundingBoxes.h"

BoundingBoxes::BoundingBoxes(const Mesh& mesh)
	:basicMesh(mesh)
{
	aabb.c = {( 0, 0, 0) };
	aabb.h = { (0, 0, 0) };
	aabb.min = dx::XMFLOAT3(0, 0, 0);
	aabb.max = dx::XMFLOAT3(0, 0, 0) ;
}

BoundingBoxes::~BoundingBoxes()
{
}




void BoundingBoxes::CalcAABB(const Mesh& mesh)
{

	DirectX::XMFLOAT3 minVertex = DirectX::XMFLOAT3(FLT_MAX, FLT_MAX, FLT_MAX);
	DirectX::XMFLOAT3 maxVertex = DirectX::XMFLOAT3(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	



	for (UINT v = 0; v < mesh.vertices.size(); v++)
	{
		minVertex.x = min(minVertex.x, mesh.vertices[v].position.x);    // Find smallest x value in model
		minVertex.y = min(minVertex.y, mesh.vertices[v].position.y);    // Find smallest y value in model
		minVertex.z = min(minVertex.z, mesh.vertices[v].position.z);    // Find smallest z value in model

		//Get the largest vertex 
		maxVertex.x = max(maxVertex.x, mesh.vertices[v].position.x);    // Find largest x value in model
		maxVertex.y = max(maxVertex.y, mesh.vertices[v].position.y);    // Find largest y value in model
		maxVertex.z = max(maxVertex.z, mesh.vertices[v].position.z);    // Find largest z value in model
	}

	

	aabb.max = maxVertex;
	aabb.min = minVertex;

	DirectX::XMVECTOR min = DirectX::XMLoadFloat3(&minVertex);

	DirectX::XMVECTOR max = DirectX::XMLoadFloat3(&maxVertex);

	aabb.c = DirectX::XMVectorAdd(min, max);

	aabb.c = DirectX::XMVectorScale(aabb.c, 1.0f / 2.0f);

	aabb.h = DirectX::XMVectorSubtract(max, min);

	aabb.h = DirectX::XMVectorScale(aabb.h, 1.0f / 2.0f);

	
	//More calculations are needed for OBB but I don't know what.

	


}

void BoundingBoxes::CalcAABB()
{
	DirectX::XMFLOAT3 minVertex = DirectX::XMFLOAT3(FLT_MAX, FLT_MAX, FLT_MAX);
	DirectX::XMFLOAT3 maxVertex = DirectX::XMFLOAT3(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	

	for (UINT v = 0; v < basicMesh.vertices.size(); v++)
	{
		minVertex.x = min(minVertex.x, basicMesh.vertices[v].position.x);    // Find smallest x value in model
		minVertex.y = min(minVertex.y, basicMesh.vertices[v].position.y);    // Find smallest y value in model
		minVertex.z = min(minVertex.z, basicMesh.vertices[v].position.z);    // Find smallest z value in model

		//Get the largest vertex 
		maxVertex.x = max(maxVertex.x, basicMesh.vertices[v].position.x);    // Find largest x value in model
		maxVertex.y = max(maxVertex.y, basicMesh.vertices[v].position.y);    // Find largest y value in model
		maxVertex.z = max(maxVertex.z, basicMesh.vertices[v].position.z);    // Find largest z value in model
	}

	

	aabb.max = maxVertex;
	aabb.min = minVertex;

	DirectX::XMVECTOR min = DirectX::XMLoadFloat3(&minVertex);

	DirectX::XMVECTOR max = DirectX::XMLoadFloat3(&maxVertex);

	aabb.c = DirectX::XMVectorAdd(min, max);

	aabb.c = DirectX::XMVectorScale(aabb.c, 1.0f / 2.0f);

	aabb.h = DirectX::XMVectorSubtract(max, min);

	aabb.h = DirectX::XMVectorScale(aabb.h, 1.0f / 2.0f);

	
	//More calculations are needed for OBB but I don't know what.

	
}
