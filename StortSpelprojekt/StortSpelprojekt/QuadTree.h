#pragma once
#include <vector>
#include <DirectXMath.h>
#include <assert.h>
#include "DShape.h"
#include "Bounds.h"

namespace dx = DirectX;

class QuadTree
{
public:
	struct Node
	{
		dx::XMFLOAT2 min;
		dx::XMFLOAT2 max;
		void* data;
	};

public:
	QuadTree(Bounds bounds, size_t maxLevels = 8, size_t maxItems = 8);
	QuadTree(dx::XMFLOAT2 min, dx::XMFLOAT2 max, size_t maxLevels = 8, size_t maxItems = 8);
	QuadTree(size_t level, dx::XMFLOAT2 min, dx::XMFLOAT2 max, size_t maxLevels, size_t maxItems);
	virtual ~QuadTree();

	void Insert(const Bounds& bounds, const dx::XMMATRIX& transform, void* data );
	void Insert(dx::XMFLOAT2 point, float radius, void* data);
	void Insert(dx::XMFLOAT2 min, dx::XMFLOAT2 max, void* data);
	void Insert(Node node);
	void Clear();

	void Remove(Node node);

	void SetMinMax(dx::XMFLOAT2 min, dx::XMFLOAT2 max);
	std::vector<Node> GetNodesInArea(dx::XMFLOAT2 min, dx::XMFLOAT2 max) const;

	size_t CountInRange(dx::XMFLOAT2 point, float radius) const;

	void DebugDraw(dx::XMFLOAT3 offset = dx::XMFLOAT3(0,0,0));

	dx::XMFLOAT2 GetMin() const { return this->min; }
	dx::XMFLOAT2 GetMax() const { return this->max; }

private:
	void RecursiveAddToArea(dx::XMFLOAT2 min, dx::XMFLOAT2 max, std::vector<Node>& found) const;
	void Split();

	bool BoxBoxOverlap(dx::XMFLOAT2 min0, dx::XMFLOAT2 max0, dx::XMFLOAT2 min1, dx::XMFLOAT2 max1) const;
	bool BoxContainsPoint(dx::XMFLOAT2 min, dx::XMFLOAT2 max, dx::XMFLOAT2 point) const;
	float DistanceToBox(const dx::XMFLOAT2& point, const dx::XMFLOAT2& min, const dx::XMFLOAT2& max) const;

	bool ContainsPoint(dx::XMFLOAT2 point);
	void GetChildrenBounds(dx::XMFLOAT2[]) const;
	int GetChildIndex(const Node& node) const;

private:
	QuadTree** children;
	std::vector<Node> nodes;
	dx::XMFLOAT2 min, max;
	size_t level;
	size_t maxLevels;
	size_t maxItems;
};