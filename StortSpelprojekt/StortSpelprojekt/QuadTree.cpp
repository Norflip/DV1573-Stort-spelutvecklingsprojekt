#include "stdafx.h"
#include "QuadTree.h"

QuadTree::QuadTree(Bounds bounds, size_t maxLevels, size_t maxItems)
	: QuadTree(0, dx::XMFLOAT2(bounds.GetMin().x, bounds.GetMin().z), dx::XMFLOAT2(bounds.GetMax().x, bounds.GetMax().z), maxLevels, maxItems)
{
}

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

void QuadTree::Insert(size_t id, const Bounds& bounds, const dx::XMMATRIX& transform, void* data)
{
	dx::XMFLOAT3 min, max;
	bounds.TransformMinMax(transform, min, max);
	Insert(id, dx::XMFLOAT2(min.x, min.z), dx::XMFLOAT2(max.x, max.z), data);
}

void QuadTree::Insert(size_t id, dx::XMFLOAT2 point, float radius, void* data)
{
	dx::XMFLOAT2 min(point.x - radius, point.y - radius);
	dx::XMFLOAT2 max(point.x + radius, point.y + radius);
	Insert(id, min, max, data);
}

void QuadTree::Insert(size_t id, dx::XMFLOAT2 min, dx::XMFLOAT2 max, void* data)
{
	Node node;
	node.id = id;
	node.min = min;
	node.max = max;
	node.data = data;
	Insert(node);
}

void QuadTree::Insert(Node node)
{
	if (children != nullptr)
	{
		int index = GetChildIndex(node);
		for (int b = 0; b < 4 && index != -1; b++)
		{
			if ((index & (1 << b)) == (1 << b))
			{
				children[b]->Insert(node);
			}
		}
		return;
	}

	nodes.push_back(node);

	if (nodes.size() >= maxItems && level < maxLevels)
	{
		if (children == nullptr)
			Split();

		for (int i = nodes.size() - 1; i >= 0; i--)
		{
			int index = GetChildIndex(nodes[i]);
			if (index != -1)
				children[index]->Insert(node);
		}

		nodes.clear();
	}
}

void QuadTree::Clear()
{
	nodes.clear();

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

void QuadTree::Remove(Node node)
{
	if (children != nullptr)
	{
		int index = GetChildIndex(node);
		for (int b = 0; b < 4 && index != -1; b++)
		{
			if ((index & (1 << b)) == (1 << b))
			{
				children[b]->Remove(node);
			}
		}
	}
	else
	{
		bool found = false;
		for (size_t i = 0; i < nodes.size() && !found; i++)
		{
			if (nodes[i].id == node.id && 
				nodes[i].min.x != node.min.x && nodes[i].min.y != node.min.y && 
				nodes[i].max.x != node.max.x && nodes[i].max.y != node.max.y)
			{
				found = true;
				nodes.erase(nodes.begin() + i);
			}
		}
	}
}

void QuadTree::SetMinMax(dx::XMFLOAT2 min, dx::XMFLOAT2 max)
{
	this->min = min;
	this->max = max;
}

std::vector<QuadTree::Node> QuadTree::GetNodesInArea(dx::XMFLOAT2 min, dx::XMFLOAT2 max) const
{
	std::vector<Node> nodes;
	RecursiveAddToArea(min, max, nodes);
	return nodes;
}

std::vector<size_t> QuadTree::GetIDsInArea(dx::XMFLOAT2 min, dx::XMFLOAT2 max) const
{
	std::vector<Node> nodes;
	RecursiveAddToArea(min, max, nodes);

	const size_t size = nodes.size();
	std::vector<size_t> ids(size);
	for (size_t i = 0; i < size; i++)
		ids[i] = nodes[i].id;
	return ids;
}

size_t QuadTree::CountInRange(dx::XMFLOAT2 point, float radius) const
{
	std::vector<QuadTree::Node> inArea = GetNodesInArea(dx::XMFLOAT2(point.x - radius, point.y - radius), dx::XMFLOAT2(point.x + radius, point.y + radius));
	std::vector<QuadTree::Node> inRange;

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
		for (int i = nodes.size() - 1; i >= 0; i--)
		{
			float x = Math::Lerp(nodes[i].min.x, nodes[i].max.x, 0.5f);
			float z = Math::Lerp(nodes[i].min.y, nodes[i].max.y, 0.5f);
			dx::XMFLOAT3 position = dx::XMFLOAT3(offset.x + x, offset.y, offset.z + z);

			DShape::DrawWireBox(position, dx::XMFLOAT3(1.0f, 1.0f, 1.0f), dx::XMFLOAT3(1, 0, 1));

		}
	}
}

void QuadTree::RecursiveAddToArea(dx::XMFLOAT2 min, dx::XMFLOAT2 max, std::vector<Node>& found) const
{
	if (children != nullptr)
	{
		for (size_t i = 0; i < 4; i++)
		{
			if (BoxBoxOverlap(min, max, children[i]->min, children[i]->max))
				children[i]->RecursiveAddToArea(min, max, found);
		}
	}
	else
	{
		for (int i = nodes.size() - 1; i >= 0; i--)
		{
			if (BoxBoxOverlap(min, max, nodes[i].min, nodes[i].max))
				found.push_back(nodes[i]);
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

int QuadTree::GetChildIndex(const Node& node) const
{
	int index = -1;
	dx::XMFLOAT2 minMax[8];
	GetChildrenBounds(minMax);

	for (int i = 0; i < 4 && index == -1; i++)
	{
		if (BoxBoxOverlap(minMax[i * 2], minMax[i * 2 + 1], node.min, node.max))
			index |= (1 << i);
	}

	return index;
}
