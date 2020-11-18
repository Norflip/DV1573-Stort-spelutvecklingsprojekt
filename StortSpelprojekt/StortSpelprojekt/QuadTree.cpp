#include "stdafx.h"
#include "QuadTree.h"

QuadTree::QuadTree(dx::XMFLOAT2 min, dx::XMFLOAT2 max, size_t maxLevels, size_t maxItems) 
	: QuadTree(0, min, max, maxLevels, maxItems) 
{

}

QuadTree::QuadTree(size_t level, dx::XMFLOAT2 min, dx::XMFLOAT2 max, size_t maxLevels, size_t maxItems) 
	: level(level), children(nullptr), maxLevels(maxLevels), maxItems(maxItems)
{
	SetMinMax(min, max);
}

QuadTree::~QuadTree() {}

void QuadTree::Insert(const Bounds& bounds, const dx::XMMATRIX& transform)
{
	dx::XMFLOAT3 min, max;
	bounds.TransformMinMax(transform, min, max);
	Insert(dx::XMFLOAT2(min.x, min.z), dx::XMFLOAT2(max.x, max.z));
}

void QuadTree::Insert(dx::XMFLOAT2 point)
{
	Insert(point, point);
}

void QuadTree::Insert(dx::XMFLOAT2 min, dx::XMFLOAT2 max)
{
	Quad quad;
	quad.min = min;
	quad.max = max;
	Insert(quad);
}

void QuadTree::Insert(Quad quad)
{
	if (children != nullptr)
	{
		int index = GetChildIndex(quad);
		for (int b = 0; b < 4 && index != -1; b++)
		{
			if ((index & (1 << b)) == (1 << b))
			{
				children[b]->Insert(min, max);
			}
		}
		return;
	}

	quads.push_back(quad);

	if (quads.size() >= maxItems && level < maxLevels)
	{
		if (children == nullptr)
			Split();

		for (int i = quads.size() - 1; i >= 0; i--)
		{
			int index = GetChildIndex(quads[i]);
			if (index != -1)
				children[index]->Insert(quad);
		}

		quads.clear();
	}
}

void QuadTree::Clear()
{
	quads.clear();

	if (children != nullptr)
	{
		for (size_t i = 0; i < 4; i++)
		{
			children[i]->Clear();
			delete children[i];
		}

		delete[]children;
		children = nullptr;
	}
}

void QuadTree::SetMinMax(dx::XMFLOAT2 min, dx::XMFLOAT2 max)
{
	this->min = min;
	this->max = max;
}

std::vector<QuadTree::Quad> QuadTree::GetInArea(dx::XMFLOAT2 min, dx::XMFLOAT2 max) const
{
	std::vector<Quad> quads;
	RecursiveAddToArea(min, max, quads);
	return quads;
}

size_t QuadTree::CountInRange(dx::XMFLOAT2 point, float radius) const
{
	std::vector<QuadTree::Quad> inArea = GetInArea(dx::XMFLOAT2(point.x - radius, point.y - radius), dx::XMFLOAT2(point.x + radius, point.y + radius));
	std::vector<QuadTree::Quad> inRange;
	
	for (size_t i = 0; i < inArea.size(); i++)
	{
		float distance = DistanceToBox(point, inArea[i].min, inArea[i].max);
		if (distance <= radius)
		{
			inRange.push_back(inArea[i]);
		}
	}

	return inRange.size();
}

void QuadTree::DebugDraw(dx::XMFLOAT3 offset)
{
	float height = 0.0f;
	float xExt = (max.x - min.x);
	float yExt = (max.y - min.y);

	float x = offset.x + min.x + xExt / 2.0f;
	float y = offset.y + height / 2.0f;
	float z = offset.z + min.y + yExt / 2.0f;

	DShape::DrawWireBox(dx::XMFLOAT3(x, y, z), dx::XMFLOAT3(xExt, height, yExt), dx::XMFLOAT3(1, 0, 0));


	if (children != nullptr)
	{
		for (size_t i = 0; i < 4; i++)
			children[i]->DebugDraw(offset);
	}
	else
	{
		for (int i = quads.size() - 1; i >= 0; i--)
		{
			float x = Math::Lerp(quads[i].min.x, quads[i].max.x, 0.5f);
			float z = Math::Lerp(quads[i].min.y, quads[i].max.y, 0.5f);
			dx::XMFLOAT3 position = dx::XMFLOAT3(offset.x + x, offset.y, offset.z + z);
			DShape::DrawSphere(position, 0.1f, dx::XMFLOAT3(1, 0, 1));
		}
	}
}

void QuadTree::RecursiveAddToArea(dx::XMFLOAT2 min, dx::XMFLOAT2 max, std::vector<Quad>& foundQuads) const
{
	if (children != nullptr)
	{
		for (size_t i = 0; i < 4; i++)
		{
			if (BoxBoxOverlap(min, max, children[i]->min, children[i]->max))
				children[i]->RecursiveAddToArea(min, max, foundQuads);
		}
	}
	else
	{
		for (int i = quads.size() - 1; i >= 0; i--)
		{
			if (BoxBoxOverlap(min, max, quads[i].min, quads[i].max))
				foundQuads.push_back(quads[i]);
		}
	}
}

void QuadTree::Split()
{
	dx::XMFLOAT2 minMax[8];
	GetChildrenBounds(minMax);

	children = new QuadTree * [4];
	for (size_t i = 0; i < 4; i++)
		children[i] = new QuadTree(level + 1, minMax[i * 2], minMax[i * 2 + 1], maxLevels, maxItems);
}

bool QuadTree::BoxBoxOverlap(dx::XMFLOAT2 min0, dx::XMFLOAT2 max0, dx::XMFLOAT2 min1, dx::XMFLOAT2 max1) const
{
	return !(min0.x > max1.x || max0.x < min1.x || min0.y > max1.y || max0.y < min1.y);
}

bool QuadTree::BoxContainsPoint(dx::XMFLOAT2 min, dx::XMFLOAT2 max, dx::XMFLOAT2 point) const
{
	return BoxBoxOverlap(min, max, point, point);
}

float QuadTree::DistanceToBox(const dx::XMFLOAT2& point, const dx::XMFLOAT2& min, const dx::XMFLOAT2& max) const
{
	float dx = std::max(min.x - point.x, point.x - max.x);
	float dy = std::max(min.y - point.y, point.y - max.y);
	return sqrtf(dx * dx + dy * dy);
}

bool QuadTree::ContainsPoint(dx::XMFLOAT2 point)
{
	return BoxContainsPoint(min, max, point);
}

void QuadTree::GetChildrenBounds(dx::XMFLOAT2 minmax[]) const
{
	float width = (max.x - min.x) / 2.0f;
	float height = (max.y - min.y) / 2.0f;

	//BOTTOM RIGHT
	minmax[0] = dx::XMFLOAT2(min.x + width, min.y);
	minmax[1] = dx::XMFLOAT2(max.x, min.y + height);

	//BOTTOM LEFT
	minmax[2] = dx::XMFLOAT2(min.x, min.y);
	minmax[3] = dx::XMFLOAT2(min.x + width, min.y + height);

	//TOP LEFT
	minmax[4] = dx::XMFLOAT2(min.x, min.y + height);
	minmax[5] = dx::XMFLOAT2(min.x + width, max.y);

	// TOP RIGHT
	minmax[6] = dx::XMFLOAT2(min.x + width, min.y + height);
	minmax[7] = dx::XMFLOAT2(max.x, max.y);
}

int QuadTree::GetChildIndex(const Quad& quad) const
{
	int index = -1;
	dx::XMFLOAT2 minMax[8];
	GetChildrenBounds(minMax);

	for (int i = 0; i < 4 && index == -1; i++)
	{
		if (BoxBoxOverlap(minMax[i * 2], minMax[i * 2 + 1], quad.min, quad.max))
			index != (1 << i);
	}

	return index;
}
