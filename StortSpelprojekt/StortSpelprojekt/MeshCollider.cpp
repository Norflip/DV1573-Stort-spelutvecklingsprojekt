#include "stdafx.h"
#include "MeshCollider.h"
#include "Engine.h"

MeshCollider::MeshCollider(Mesh* mesh, dx::XMFLOAT3 position)
	:Collider(position), mesh(mesh)
{

}

MeshCollider::MeshCollider(Mesh* mesh, std::vector<dx::XMFLOAT3> positions)
	: Collider(positions), mesh(mesh)
{

}

#define CURSED FALSE

void MeshCollider::InitializeCollider(Physics* physics)
{
#if CURSED

	size_t triangles = mesh->GetTriangleCount();
	std::vector<Mesh::Vertex> vertices = mesh->GetVertices();
	std::vector<size_t> indices = mesh->GetIndices();
	dx::XMVECTOR p1, p2, p3, t, u, v;

	for (size_t i = 0; i < triangles; i++)
	{
		p1 = dx::XMLoadFloat3(&vertices[indices[i * 3 + 0]].position);
		p2 = dx::XMLoadFloat3(&vertices[indices[i * 3 + 0]].position);
		p3 = dx::XMLoadFloat3(&vertices[indices[i * 3 + 0]].position);
		t = dx::XMVectorSubtract(p2, p1);
		u = dx::XMVectorSubtract(p3, p1);
		v = dx::XMVectorSubtract(p3, p2);
		dx::XMVECTOR w = dx::XMVector3Cross(t, u);

		dx::XMVECTOR iwsl2 = dx::XMVectorDivide(dx::XMVectorSplatOne(), dx::XMVectorMultiply(dx::XMVector3Length(w), { 2,2,2 }));

		dx::XMVECTOR tt = dx::XMVectorMultiply(t, t);
		dx::XMVECTOR uu = dx::XMVectorMultiply(u, u);
		dx::XMVECTOR vv = dx::XMVectorMultiply(v, v);
		dx::XMFLOAT3 wat;
		dx::XMStoreFloat3(&wat, dx::XMVectorMultiply(dx::XMVectorMultiply(tt, dx::XMVectorMultiply(uu, vv)), dx::XMVectorMultiply(iwsl2, { 0.5f, 0.5f, 0.5f, 0.5f })));

		dx::XMFLOAT3 d;
		dx::XMStoreFloat3(&d, dx::XMVectorDivide(dx::XMVectorAdd(p1, dx::XMVectorAdd(p2, p3)), { 3,3,3,3 }));
		GetOwner()->AddComponent<SphereColliderComponent>(sqrtf(wat.x), d);
	}

#else



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

#endif // CURSED

}

void MeshCollider::Update(const float& deltaTime)
{
#if DRAW_COLLIDERS && !CURSED

	const size_t COUNT = 3;
	const float OFFSET = 1.02f;

	dx::XMFLOAT3 color = { 1,0,0 };
	dx::XMFLOAT3 positions[COUNT];
	size_t triangles = mesh->GetTriangleCount();
	std::vector<Mesh::Vertex> vertices = mesh->GetVertices();
	std::vector<size_t> indices = mesh->GetIndices();

	dx::XMMATRIX model = GetOwner()->GetTransform().GetWorldMatrix();

	for (size_t i = 0; i < colliderInformations.size(); i++)
	{
		dx::XMVECTOR p = dx::XMLoadFloat3(&colliderInformations[i].position);
		dx::XMVECTOR r = dx::XMLoadFloat4(&colliderInformations[i].rotation);

		for (size_t j = 0; j < triangles; j++)
		{
			for (size_t k = 0; k < COUNT; k++)
			{
				dx::XMVECTOR vector = dx::XMVectorScale(dx::XMLoadFloat3(&vertices[indices[j * COUNT + k]].position), OFFSET);
				dx::XMStoreFloat3(&positions[k], dx::XMVector3Transform(vector, model));
			}

			size_t i0 = COUNT - 1;
			for (size_t i1 = 0; i1 < COUNT; i1++)
			{
				DShape::DrawLine(positions[i0], positions[i1], color);
				i0 = i1;
			}
		}
	}

#endif
}
