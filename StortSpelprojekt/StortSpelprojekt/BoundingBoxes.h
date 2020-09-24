#pragma once
#include "Mesh.h"
struct AABB //OBB can be added too if someone knows how.
{
	dx::XMFLOAT3 min;
	dx::XMFLOAT3 max;

	dx::XMVECTOR c; //centerpoint
	dx::XMVECTOR h; //positive half diagonal vector

	

};
class BoundingBoxes
{

private:
	AABB aabb; 
	Mesh basicMesh;
	
public:
	BoundingBoxes(const Mesh& mesh);
	virtual ~BoundingBoxes();

	void SetAABB(const AABB& aabb) { this->aabb=aabb; }
	
	const AABB& GetAABB() { return this->aabb; }

	
	void CalcAABB(const Mesh& mesh); // can also be used to calc OBB if someone knows how.
	void CalcAABB();
	
};

//Right now the bb are static. Not dynamic.