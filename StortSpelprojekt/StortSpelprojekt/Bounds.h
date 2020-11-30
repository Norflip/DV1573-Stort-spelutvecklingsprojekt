#pragma once
#include "Mesh.h"
#include <algorithm>

class Bounds
{
public:
	Bounds();
	Bounds(dx::XMFLOAT3 min, dx::XMFLOAT3 max);
	virtual ~Bounds();
	
	void CalculateAABB(const std::vector<Mesh*>& meshes);
	void CalculateAABB(const Mesh* mesh);
	
	void Encapsulate(const Bounds& bounds);
	void Encapsulate(dx::XMFLOAT3 point);
	void Encapsulate(dx::XMFLOAT3 min, dx::XMFLOAT3 max);
	
	void SetMinMax(dx::XMFLOAT3 min, dx::XMFLOAT3 max);
	dx::XMFLOAT3 GetMin() const { return this->min; }
	dx::XMFLOAT3 GetMax() const { return this->max; }
	dx::XMFLOAT3 GetSize() const;
	float GetRadiusXZ() const;

	void TransformMinMax(dx::XMMATRIX worldTransform, dx::XMFLOAT3& min, dx::XMFLOAT3& max) const;

	void GetCorners(dx::XMFLOAT3 corners[8]) const;
	dx::XMFLOAT3 GetCenter() const;
	dx::XMFLOAT3 GetExtends() const;

private:
	void Float3Min(dx::XMFLOAT3& target, const dx::XMFLOAT3& value0, const dx::XMFLOAT3& value1) const;
	void Float3Max(dx::XMFLOAT3& target, const dx::XMFLOAT3& value0, const dx::XMFLOAT3& value1) const;

	dx::XMFLOAT3 min, max;
};

//Right now the bb are static. Not dynamic.