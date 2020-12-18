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

MeshCollider::~MeshCollider()
{
}

void MeshCollider::DeleteShapes()
{
	rp::PhysicsCommon& common = Engine::Instance->GetPhysics()->GetCommon();

	for (size_t i = 0; i < colliderInformations.size(); i++)
	{
		common.destroyConvexMeshShape(static_cast<rp::ConvexMeshShape*>(colliderInformations[i].shape));
	}

	for (size_t i = 0; i < meshData.size(); i++)
	{
		common.destroyPolyhedronMesh(meshData[i].polyhedronMesh);
		delete[] meshData[i].flat;
		delete meshData[i].vertexArray;
		delete[] meshData[i].faces;
	}
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
		
		MeshColliderData data;
		
		data.flat = flatVertexBuffer;
		data.faces = new rp::PolygonVertexArray::PolygonFace[faceCount];

		for (size_t i = 0; i < faceCount; i++)
		{
			data.faces[i].indexBase = i * 3;
			data.faces[i].nbVertices = 3;
		}

		data.vertexArray = new rp::PolygonVertexArray(vertexBuffer.size(), flatVertexBuffer, 3 * sizeof(float), indexBuffer.data(), sizeof(int), faceCount, data.faces,
			rp::PolygonVertexArray::VertexDataType::VERTEX_FLOAT_TYPE,
			rp::PolygonVertexArray::IndexDataType::INDEX_INTEGER_TYPE);

		data.polyhedronMesh = common.createPolyhedronMesh(data.vertexArray);
		rp::ConvexMeshShape* convexMeshShape = common.createConvexMeshShape(data.polyhedronMesh);

		meshData.push_back(data);
		colliderInformations[i].shape = convexMeshShape;
	}
}

void MeshCollider::Update(const float& deltaTime)
{
	if (GameScene::drawColliders)
	{
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
	}
}
