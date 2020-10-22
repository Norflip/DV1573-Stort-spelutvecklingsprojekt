#include "PointQuadTree.h"

PointQuadTree::PointQuadTree(dx::XMFLOAT2 min, dx::XMFLOAT2 max) : PointQuadTree(0, min, max) {}
PointQuadTree::PointQuadTree(size_t level, dx::XMFLOAT2 min, dx::XMFLOAT2 max) : level(level), children(nullptr)
{
	SetMinMax(min, max);
}

PointQuadTree::~PointQuadTree()
{

}

void PointQuadTree::Insert(dx::XMFLOAT2 point)
{
	if (children != nullptr)
	{
		int index = GetChildIndex(point);
		if (index != -1)
			children[index]->Insert(point);
		return;
	}

	points.push_back(point);

	if (points.size() >= MAX_POINTS && level < MAX_LEVELS)
	{
		if (children == nullptr)
			Split();


		for (int i = points.size() - 1; i >= 0; i--)
		{
			int index = GetChildIndex(points[i]);
			if (index != -1)
			{
				children[index]->Insert(point);
			}
		}

		points.clear();

	}

}

void PointQuadTree::Clear()
{
	points.clear();

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

void PointQuadTree::SetMinMax(dx::XMFLOAT2 min, dx::XMFLOAT2 max)
{
	this->min = min;
	this->max = max;
}

std::vector<dx::XMFLOAT2> PointQuadTree::GetInRadius(dx::XMFLOAT2 point, float radius)
{
	dx::XMFLOAT2 min(point.x - radius, point.y - radius);
	dx::XMFLOAT2 max(point.x + radius, point.y + radius);

	std::vector<dx::XMFLOAT2> inRadius;
	std::vector<dx::XMFLOAT2> inArea;
	inArea = GetInArea(min, max);

	// kolla 
	for (int i = inArea.size() - 1; i >= 0; i--)
	{
		// flytta om dom är inom radius
		float dx = point.x - inArea[i].x;
		float dy = point.y - inArea[i].y;
		float distance = dx * dx + dy * dy;

		if (distance < radius * radius)
			inRadius.push_back(inArea[i]);
	}

	return inRadius;
}

std::vector<dx::XMFLOAT2> PointQuadTree::GetInArea(dx::XMFLOAT2 min, dx::XMFLOAT2 max)
{
	std::vector<dx::XMFLOAT2> points;
	RecursiveAddToArea(min, max, points);
	return points;
}

void PointQuadTree::Draw(dx::XMFLOAT3 offset)
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
			children[i]->Draw(offset);
	}
	else
	{
		for (int i = points.size() - 1; i >= 0; i--)
		{
			dx::XMFLOAT3 position = dx::XMFLOAT3(offset.x + points[i].x, offset.y, offset.z + points[i].y);
			DShape::DrawSphere(position, 0.1f, dx::XMFLOAT3(1, 0, 1));
		}
	}
}

void PointQuadTree::RecursiveAddToArea(dx::XMFLOAT2 min, dx::XMFLOAT2 max, std::vector<dx::XMFLOAT2>& positions)
{
	if (children != nullptr)
	{
		for (size_t i = 0; i < 4; i++)
		{
			if (BoxBoxOverlap(min, max, children[i]->min, children[i]->max))
				children[i]->RecursiveAddToArea(min, max, positions);
		}
	}
	else
	{
		for (int i = points.size() - 1; i >= 0; i--)
		{
			if (BoxContainsPoint(min, max, points[i]))
				positions.push_back(points[i]);
		}
	}
}

void PointQuadTree::Split()
{
	dx::XMFLOAT2 minMax[8];
	GetChildrenBounds(minMax);

	children = new PointQuadTree * [4];
	for (size_t i = 0; i < 4; i++)
		children[i] = new PointQuadTree(level + 1, minMax[i * 2], minMax[i * 2 + 1]);
}

bool PointQuadTree::BoxBoxOverlap(dx::XMFLOAT2 min0, dx::XMFLOAT2 max0, dx::XMFLOAT2 min1, dx::XMFLOAT2 max1) const
{
	return !(min0.x > max1.x || max0.x < min1.x || min0.y > max1.y || max0.y < min1.y);
}

bool PointQuadTree::BoxContainsPoint(dx::XMFLOAT2 min, dx::XMFLOAT2 max, dx::XMFLOAT2 point) const
{
	return BoxBoxOverlap(min, max, point, point);
}

bool PointQuadTree::ContainsPoint(dx::XMFLOAT2 point)
{
	return BoxContainsPoint(min, max, point);
}

void PointQuadTree::GetChildrenBounds(dx::XMFLOAT2 minmax[]) const
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

int PointQuadTree::GetChildIndex(dx::XMFLOAT2 point) const
{
	int index = -1;
	dx::XMFLOAT2 minMax[8];
	GetChildrenBounds(minMax);

	for (int i = 0; i < 4 && index == -1; i++)
		if (BoxContainsPoint(minMax[i * 2], minMax[i * 2 + 1], point))
			index = i;

	return index;
}
