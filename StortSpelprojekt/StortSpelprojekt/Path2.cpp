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
	auto A = x - x1;
	auto B = y - y1;
	auto C = x2 - x1;
	auto D = y2 - y1;

	auto dot = A * C + B * D;
	auto len_sq = C * C + D * D;
	auto param = -1;

	if (len_sq != 0) //in case of 0 length line
		param = dot / len_sq;

	float xx, yy;

	if (param < 0) {
		xx = x1;
		yy = y1;
	}
	else if (param > 1) {
		xx = x2;
		yy = y2;
	}
	else {
		xx = x1 + param * C;
		yy = y1 + param * D;
	}

	auto dx = x - xx;
	auto dy = y - yy;
	return sqrtf(dx * dx + dy * dy);

	//float dx = line1x - line0x;
	//float dy = line1y - line0y;

	//if ((dx == 0) && (dy == 0))
	//{
	//	//std::cout << line0x << " | " << line1x << "\t" << line0y << " | " << line1y << std::endl;

	//	// It's a point not a line segment.
	//	dx = px - line0x;
	//	dy = py - line0y;
	//	return sqrtf(dx * dx + dy * dy);
	//}
	//else
	//{
	//	// Calculate the t that minimizes the distance.
	//	float t = ((px - line0x) * dx + (py - line0y) * dy) / (dx * dx + dy * dy);

	//	// See if this represents one of the segment's
	//	// end points or a point in the middle.
	//	if (t < 0)
	//	{
	//		dx = px - line0x;
	//		dy = py - line0y;
	//	}
	//	else if (t > 1)
	//	{
	//		dx = px - line1x;
	//		dy = py - line1y;
	//	}
	//	else
	//	{
	//		float cx = line0x + t * dx;
	//		float cy = line0y + t * dx;

	//		dx = px - cx;
	//		dy = py - cy;
	//	}

	//	return sqrtf(dx * dx + dy * dy);
	//}
}
