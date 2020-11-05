#pragma once
#include "Mesh.h"
#include <algorithm>

struct AABB //OBB can be added too if someone knows how.
{
	dx::XMFLOAT3 min;
	dx::XMFLOAT3 max;
};


class BoundingBox
{
private:
	AABB aabb; 
	dx::XMFLOAT3 min, max;

public:
	BoundingBox();
	virtual ~BoundingBox();
	
	void CalculateAABB(const std::vector<Mesh*>& meshes);
	void CalculateAABB(const Mesh* mesh);

	const AABB& GetAABB() const { return this->aabb; }	
	
	dx::XMFLOAT3 GetMin() const { return this->min; }
	dx::XMFLOAT3 GetMax() const { return this->max; }
	void GetCorners(dx::XMFLOAT3 corners[8]);

	dx::XMFLOAT3 GetCenter() const;
	dx::XMFLOAT3 GetExtends() const;

};

//Right now the bb are static. Not dynamic.