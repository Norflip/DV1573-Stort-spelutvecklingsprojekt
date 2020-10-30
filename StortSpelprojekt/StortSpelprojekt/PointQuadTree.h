#pragma once
#include <vector>
#include <DirectXMath.h>
#include <assert.h>
#include "DShape.h"

namespace dx = DirectX;

class PointQuadTree
{
	const size_t MAX_LEVELS = 8;
	const size_t MAX_POINTS = 8;

public:
	PointQuadTree(dx::XMFLOAT2 min, dx::XMFLOAT2 max);
	PointQuadTree(size_t level, dx::XMFLOAT2 min, dx::XMFLOAT2 max);

	virtual ~PointQuadTree();

	void Insert(dx::XMFLOAT2 point);
	void Clear();

	void SetMinMax(dx::XMFLOAT2 min, dx::XMFLOAT2 max);

	std::vector<dx::XMFLOAT2> GetInRadius(dx::XMFLOAT2 point, float radius);
	std::vector<dx::XMFLOAT2> GetInArea(dx::XMFLOAT2 min, dx::XMFLOAT2 max);
	void Draw(dx::XMFLOAT3 offset);

	dx::XMFLOAT2 GetMin() const { return this->min; }
	dx::XMFLOAT2 GetMax() const { return this->max; }

private:
	void RecursiveAddToArea(dx::XMFLOAT2 min, dx::XMFLOAT2 max, std::vector<dx::XMFLOAT2>& positions);
	void Split();
	
	bool BoxBoxOverlap(dx::XMFLOAT2 min0, dx::XMFLOAT2 max0, dx::XMFLOAT2 min1, dx::XMFLOAT2 max1) const;
	bool BoxContainsPoint(dx::XMFLOAT2 min, dx::XMFLOAT2 max, dx::XMFLOAT2 point) const;

	bool ContainsPoint(dx::XMFLOAT2 point);
	void GetChildrenBounds(dx::XMFLOAT2[]) const;
	int GetChildIndex(dx::XMFLOAT2 point) const;

private:
	PointQuadTree** children;
	std::vector<dx::XMFLOAT2> points;
	dx::XMFLOAT2 min, max;
	size_t level;
};