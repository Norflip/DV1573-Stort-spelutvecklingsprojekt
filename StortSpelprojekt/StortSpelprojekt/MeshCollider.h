#pragma once

#include "Component.h"
#include "Collider.h"
#include "ThirdParty\quickhull\QuickHull.hpp"

namespace dx = DirectX;
namespace qh = quickhull;

class MeshCollider : public Component, public Collider
{
public:
	MeshCollider(Mesh* mesh, dx::XMFLOAT3 position);
	MeshCollider(Mesh* mesh, std::vector<dx::XMFLOAT3> positions);
	virtual~MeshCollider();
	void DeleteShapes() override;

	void InitializeCollider(Physics* physics) override;
	void Update(const float& deltaTime) override;
	
private:
	Mesh* mesh;
	std::vector<rp::PolyhedronMesh*> polyhedronMeshes;
};

static std::unordered_map<Mesh*, qh::ConvexHull<float>> m_hullCache;