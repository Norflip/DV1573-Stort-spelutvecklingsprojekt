#pragma once
#include "Object.h"
struct AABB //This can be changed to OBB in the future if someone knows how.
{
	dx::XMFLOAT3 min;
	dx::XMFLOAT3 max;

	dx::XMVECTOR c; //centerpoint
	dx::XMVECTOR h; //positive half diagonal vector

	UINT nrOfTrianglesContained;

};
class BoundingBoxComponent : public Component
{

private:
	std::vector<AABB> aabbs;

public:
	BoundingBoxComponent();
	virtual ~BoundingBoxComponent();

	void AddAABB(const AABB& aabb) { this->aabbs.push_back(aabb); }
	
	const std::vector<AABB>& GetAABBs() { return this->aabbs; }

	UINT GetNrOfAABBs() const { return (UINT)aabbs.size(); }
	void CalcAABB(const Mesh& mesh);


};

