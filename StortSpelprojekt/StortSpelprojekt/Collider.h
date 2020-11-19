#pragma once
#include "Component.h"
#include <react3d.h>

namespace dx = DirectX;
class Physics;

class Collider
{
	struct ColliderInformation
	{
		dx::XMFLOAT3 position;
		dx::XMFLOAT4 rotation;
		rp::Transform transform;
		rp::CollisionShape* shape;
	};

public:
	Collider(std::vector<dx::XMFLOAT3> positions);
	Collider(dx::XMFLOAT3 position);
	virtual ~Collider() {} 		// delete shape should be done in the physics world

	virtual rp::CollisionShape* GetCollisionShape(size_t index) const;// { assert(index >= 0 && index < this->colliderInformations.size()); return this->colliderInformations[index].shape; }
	virtual rp::Transform GetTransform(size_t index = 0) const;// { assert(index >= 0 && index < this->colliderInformations.size()); return this->colliderInformations[index].transform; }
	void SetRotation(size_t index, dx::XMFLOAT4 quaternion);

	size_t CountCollisionShapes() const { return this->colliderInformations.size(); }
	bool IsMultiple() const { return this->CountCollisionShapes() > 1; }

	virtual void InitializeCollider(Physics* physics) = 0;

protected:
	std::vector<ColliderInformation> colliderInformations;
};
