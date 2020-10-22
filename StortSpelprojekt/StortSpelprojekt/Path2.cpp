#include "Path2.h"

Path2::Path2() {}
Path2::Path2(const std::vector<dx::XMINT2>& indexes)
{
	SetPointsFromIndexes(indexes);
}

Path2::~Path2()
{
}

void Path2::Clear()
{
	indexes.clear();
	points.clear();
}

float Path2::ClosestDistance(const dx::XMFLOAT2& position) const
{
	if (points.size() == 0)
		return 0.0f;

	float shortest = FLT_MAX;
	const float offset = CHUNK_SIZE / 2.0f;

	for (size_t i = 0; i < points.size() - 1; i++)
	{
		dx::XMFLOAT2 p0 = points[i];
		dx::XMFLOAT2 p1 = points[i + 1];

		float tmpDistance = DistanceToLineSqr(position.x, position.y,
			p0.x + offset, p0.y + offset,
			p1.x + offset, p1.y + offset
		);

		if (tmpDistance < shortest)
			shortest = tmpDistance;
	}

	//std::cout << shortest << std::endl;
	return sqrtf(shortest);
}

void Path2::SetPointsFromIndexes(const std::vector<dx::XMINT2>& indexes)
{
	const size_t cuts = CHUNK_SIZE;
	points.clear();

	for (size_t i = 0; i < indexes.size() - 1; i++)
	{
		dx::XMFLOAT2 pointA = Chunk::IndexToWorldXZ(indexes[i]);
		dx::XMFLOAT2 pointB = Chunk::IndexToWorldXZ(indexes[i + 1]);

		for (int j = 0; j < cuts; j++)
		{
			float t = (float)j / (float)cuts;
			points.push_back(Math::Lerp(pointA, pointB, t));
		}
	}

	// add last
	points.push_back(Chunk::IndexToWorldXZ(indexes[indexes.size() - 1]));
}

float Path2::DistanceToLineSqr(float x, float y, float x1, float y1, float x2, float y2) const
{
	// JESUS FUCK VA

	float edge1ToPointX = x - x1;
	float edge1ToPointY = y - y1;

	float edge1ToEdge2X = x2 - x1;
	float edge1ToEdge2Y = y2 - y1;

	float dot = edge1ToPointX * edge1ToEdge2X + edge1ToPointY * edge1ToEdge2Y;
	float lengthSqr = edge1ToEdge2X * edge1ToEdge2X + edge1ToEdge2Y * edge1ToEdge2Y;
	float t = (lengthSqr != 0)? dot / lengthSqr  : -1;

	float xx, yy;
	if (t < 0) 
	{
		xx = x1;
		yy = y1;
	}
	else if (t > 1) 
	{
		xx = x2;
		yy = y2;
	}
	else {
		xx = x1 + t * edge1ToEdge2X;
		yy = y1 + t * edge1ToEdge2Y;
	}

	float dx = x - xx;
	float dy = y - yy;
	return sqrtf(dx * dx + dy * dy);
}
