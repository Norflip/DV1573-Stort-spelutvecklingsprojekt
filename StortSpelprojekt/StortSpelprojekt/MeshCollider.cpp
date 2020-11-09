#include "stdafx.h"
#include "MeshCollider.h"
#include "Engine.h"

MeshCollider::MeshCollider(Mesh* mesh, dx::XMFLOAT3 position)
	:Collider(position), mesh(mesh)
{

}

MeshCollider::MeshCollider(Mesh* mesh, std::vector<dx::XMFLOAT3> positions)
	:Collider(positions), mesh(mesh)
{

}

void MeshCollider::InitializeCollider(Physics* physics)
{
	size_t i = 0;

	for (size_t i = 0; i < colliderInformations.size(); i++)
	{
		qh::ConvexHull<float> hull;

		auto find = m_hullCache.find(mesh);
		if (find == m_hullCache.end())
		{
			std::vector<qh::Vector3<float>> points;
			std::vector<Mesh::Vertex>& vertices = mesh->GetVertices();

			for (size_t i = 0; i < vertices.size(); i++)
			{
				dx::XMFLOAT3 position = vertices[i].position;
				points.push_back(qh::Vector3<float>(position.x, position.y, position.z));
			}

			qh::QuickHull<float> qhull;
			hull = qhull.getConvexHull(points, true, false);
			m_hullCache.insert({ mesh, hull });
		}
		else
		{
			hull = m_hullCache[mesh];
		}

		const auto& indexBuffer = hull.getIndexBuffer();
		const auto& vertexBuffer = hull.getVertexBuffer();

		float* flatVertexBuffer = new float[vertexBuffer.size() * 3];
		for (size_t i = 0; i < vertexBuffer.size(); i++)
		{
			flatVertexBuffer[i * 3 + 0] = vertexBuffer[i].x;
			flatVertexBuffer[i * 3 + 1] = vertexBuffer[i].y;
			flatVertexBuffer[i * 3 + 2] = vertexBuffer[i].z;
		}

		rp::PhysicsCommon& common = physics->GetCommon();

		size_t faceCount = indexBuffer.size() / 3;
		rp::PolygonVertexArray::PolygonFace* faces = new rp::PolygonVertexArray::PolygonFace[faceCount];

		for (size_t i = 0; i < faceCount; i++)
		{
			faces[i].indexBase = i * 3;
			faces[i].nbVertices = 3;
		}

		rp::PolygonVertexArray* vertexArray = new rp::PolygonVertexArray(vertexBuffer.size(), flatVertexBuffer, 3 * sizeof(float), indexBuffer.data(), sizeof(int), faceCount, faces,
			rp::PolygonVertexArray::VertexDataType::VERTEX_FLOAT_TYPE,
			rp::PolygonVertexArray::IndexDataType::INDEX_INTEGER_TYPE);

		rp::PolyhedronMesh* polyhedronMesh = common.createPolyhedronMesh(vertexArray);
		rp::ConvexMeshShape* convexMeshShape = common.createConvexMeshShape(polyhedronMesh);

		colliderInformations[i].shape = convexMeshShape;
	}
}

//void MeshCollider::Update(const float& deltaTime) const
//{
//#if DRAW_COLLIDERS
//	
//	dx::XMFLOAT3 color = { 1,0,0 };
//	dx::XMFLOAT3 p0, p1, p2;
//	
//	for (size_t i = 0; i < colliderInformations.size(); i++)
//	{
//		size_t triangles = mesh->GetTriangleCount();
//		std::vector<Mesh::Vertex> vertices = mesh->GetVertices();
//		std::vector<size_t> indices = mesh->GetIndices();
//		dx::XMVECTOR p = dx::XMLoadFloat3(&colliderInformations[i].position);
//
//		for (size_t i = 0; i < triangles; i++)
//		{
//			dx::XMStoreFloat3(&p0, dx::XMVectorAdd(dx::XMLoadFloat3(&vertices[indices[i * 3 + 0]].position), p));
//			dx::XMStoreFloat3(&p1, dx::XMVectorAdd(dx::XMLoadFloat3(&vertices[indices[i * 3 + 1]].position), p));
//			dx::XMStoreFloat3(&p2, dx::XMVectorAdd(dx::XMLoadFloat3(&vertices[indices[i * 3 + 2]].position), p));
//
//			DShape::DrawLine(p0, p1, color);
//			DShape::DrawLine(p1, p2, color);
//			DShape::DrawLine(p2, p0, color);
//		}
//	}
//
//#endif
