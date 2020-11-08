#pragma once

#include "Component.h"
#include "Collider.h"
#include "ThirdParty\quickhull\QuickHull.hpp"

namespace dx = DirectX;
namespace qh = quickhull;

class MeshCollider : public Component, public Collider
{
	struct ConvexMeshData
	{
		std::vector<dx::XMFLOAT3> vertices;
		std::vector<unsigned int> indices;
	};

public:
	MeshCollider(Mesh* mesh, dx::XMFLOAT3 position);
	MeshCollider(Mesh* mesh, std::vector<dx::XMFLOAT3> positions);
	virtual~MeshCollider() {};

	void InitializeCollider(Physics* physics) override;

private:
	Mesh* mesh;
	std::vector<ConvexMeshData> persistenMeshData;
};

static std::unordered_map<Mesh*, qh::ConvexHull<float>> m_hullCache;