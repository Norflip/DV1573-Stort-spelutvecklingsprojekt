#pragma once
#include <vector>
#include <DirectXMath.h>
#include <assert.h>
#include "DShape.h"

namespace dx = DirectX;

class QuadTree
{
public:
	struct Quad
	{
		dx::XMFLOAT2 min;
		dx::XMFLOAT2 max;
	};

public:
	QuadTree(dx::XMFLOAT2 min, dx::XMFLOAT2 max, size_t maxLevels = 8, size_t maxItems = 8);
	QuadTree(size_t level, dx::XMFLOAT2 min, dx::XMFLOAT2 max, size_t maxLevels, size_t maxItems);

	virtual ~QuadTree();

	void Insert(const Bounds& bounds, const dx::XMMATRIX& transform);
	void Insert(dx::XMFLOAT2 point);
	void Insert(dx::XMFLOAT2 min, dx::XMFLOAT2 max);
	void Insert(Quad quad);
	void Clear();

	void SetMinMax(dx::XMFLOAT2 min, dx::XMFLOAT2 max);
	std::vector<Quad> GetInArea(dx::XMFLOAT2 min, dx::XMFLOAT2 max) const;
	size_t CountInRange(dx::XMFLOAT2 point, float radius) const;

	void DebugDraw(dx::XMFLOAT3 offset);

	dx::XMFLOAT2 GetMin() const { return this->min; }
	dx::XMFLOAT2 GetMax() const { return this->max; }

private:
	void RecursiveAddToArea(dx::XMFLOAT2 min, dx::XMFLOAT2 max, std::vector<Quad>& foundQuads) const;
	void Split();
	
	bool BoxBoxOverlap(dx::XMFLOAT2 min0, dx::XMFLOAT2 max0, dx::XMFLOAT2 min1, dx::XMFLOAT2 max1) const;
	bool BoxContainsPoint(dx::XMFLOAT2 min, dx::XMFLOAT2 max, dx::XMFLOAT2 point) const;
	float DistanceToBox(const dx::XMFLOAT2& point, const dx::XMFLOAT2& min, const dx::XMFLOAT2& max) const;

	bool ContainsPoint(dx::XMFLOAT2 point);
	void GetChildrenBounds(dx::XMFLOAT2[]) const;
	int GetChildIndex(const Quad& quad) const;

private:
	QuadTree** children;
	std::vector<Quad> quads;
	dx::XMFLOAT2 min, max;
	size_t level;
	size_t maxLevels;
	size_t maxItems;
};