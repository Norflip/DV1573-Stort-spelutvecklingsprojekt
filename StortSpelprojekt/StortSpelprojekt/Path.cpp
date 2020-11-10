#include "stdafx.h"
#include "Path.h"
#include "Noise.h"
#include <algorithm>

Path::Path() {}
Path::Path(const std::vector<dx::XMINT2>& indexes)
{
	SetPointsFromIndexes(indexes);
	CreateLineSegments();
}

Path::~Path()
{
}

void Path::Clear()
{
	indexes.clear();
	points.clear();
}

float Path::SampleInfluence(const dx::XMFLOAT2& position) const
{
	if (points.size() == 0)
		return 0.0f;

	const float SAMPLE_DISTANCE = 1.0f;

	float shortest = FLT_MAX;
	float influence = FLT_MAX;
	float totResult = 0.0f;
	int samples = 0;
	float t;

	for (int y = -1; y <= 1; y++)
	{
		for (int x = -1; x <= 1; x++)
		{
			
		}
	}

	int x = 0;
	int y = 0;

	for (size_t i = 0; i < segments.size(); i++)
	{
		float distance = segments[i].GetDistance(position.x + x * SAMPLE_DISTANCE, position.y + y * SAMPLE_DISTANCE, t);
		if (distance < shortest)
		{
			shortest = distance;
			float result = Math::Lerp(segments[i].start.influence, segments[i].end.influence, t);
			influence = (shortest - result) / (INFLUENCE_FADE_DISTANCE);
		}
	}

	//totResult /= FCAST(samples);
	//std::cout << "IN: " << totResult << std::endl;
	//result = Math::Clamp(result, 0.0f, 1.0f);

	return Math::Clamp01(influence);
}

void Path::SetPointsFromIndexes(const std::vector<dx::XMINT2>& indexes)
{
	Noise::Settings settings(0);
	settings.octaves = 2;
	settings.lacunarity = 0.5f;
	settings.persistance = 1.0f;
	settings.scale = 2.0f;

	this->indexes = indexes;
	const size_t cuts = 2;

	std::vector<PathPoint> newPoints;
	newPoints.clear();

	size_t indexCount = indexes.size();
	std::vector<PathPoint> basePoints;

	for (size_t i = 0; i < indexCount; i++)
	{
		PathPoint p = IndexToPoint(indexes[i]);
		p.influence = Math::Lerp(MIN_INFLUENCE, MAX_INFLUENCE, Random::Value());
		basePoints.push_back(p);
	}

	//newPoints.push_back(basePoints[0]);
	for (size_t i = 0; i < basePoints.size() - 1; i++)
	{
		PathPoint pointA = basePoints[i];
		PathPoint pointB = basePoints[i + 1];

		dx::XMFLOAT2 direction = dx::XMFLOAT2(pointB.x - pointA.x, pointB.y - pointA.y);
		float length = sqrtf(direction.x * direction.x + direction.y + direction.y);
		direction.x /= length;
		direction.y /= length;
		dx::XMFLOAT2 right = dx::XMFLOAT2(-direction.y, direction.x);

		for (int j = 0; j < cuts; j++)
		{
			float t = (float)j / (float)cuts;
			PathPoint point = Lerp(pointA, pointB, t);
			float offset = 5.0f;

			float curve = Math::Lerp(0.5f * t, 0.5f + 0.5f * t, t);
			float noise = (Random::Value() + 1.0f) / 2.0f;
			noise *= curve;

			point.x += right.x * noise * offset;
			point.y += right.y * noise * offset;
			newPoints.push_back(point);
		}
	}

	// add last
	newPoints.push_back(basePoints[basePoints.size() - 1]);
	points = SmoothPoints(SmoothPoints(SmoothPoints(newPoints)));
}

void Path::CreateLineSegments()
{
	segments.clear();




	for (size_t i = 0; i < points.size() - 1; i++)
	{
		LineSegment segment(points[i], points[i + 1]);
		if (i == 0)
		{
			dx::XMFLOAT2 direction = segment.Direction();
			float x = points[i].x + (-direction.x) * 100.0f;
			float y = points[i].y + (-direction.y) * 100.0f;
			PathPoint offsetPoint(x, y, points[i].influence);
			segments.push_back(LineSegment(offsetPoint, points[i]));
		}

		segments.push_back(segment);
	}

	LineSegment& lastSegment = segments[segments.size() - 1];
	PathPoint& lastPoint = lastSegment.end;

	dx::XMFLOAT2 direction = lastSegment.Direction();
	dx::XMFLOAT2 right = dx::XMFLOAT2(-direction.y, direction.x);

	const float distanceOffset = 100.0f;

	dx::XMFLOAT2 leftDirection = Math::Lerp(direction, dx::XMFLOAT2(-right.x, -right.y), 0.5f);
	dx::XMFLOAT2 rightDirection = Math::Lerp(direction, right, 0.5f);

	PathPoint endLeft(lastPoint.x + leftDirection.x * distanceOffset, lastPoint.y + leftDirection.y * distanceOffset, lastPoint.influence);
	PathPoint endRight(lastPoint.x + (rightDirection.x) * distanceOffset, lastPoint.y + (rightDirection.y) * distanceOffset, lastPoint.influence);

	segments.push_back(LineSegment(lastPoint, endLeft));
	segments.push_back(LineSegment(lastPoint, endRight));

}

PathPoint Path::Lerp(const PathPoint& a, const PathPoint& b, float t) const
{
	return PathPoint(Math::Lerp(a.x, b.x, t), Math::Lerp(a.y, b.y, t), Math::Lerp(a.influence, b.influence, t));
}

PathPoint Path::IndexToPoint(const dx::XMINT2& index) const
{
	dx::XMFLOAT2 position = Chunk::IndexToWorldXZ(index);
	position.x += FCAST(CHUNK_SIZE) / 2.0f;
	position.y += FCAST(CHUNK_SIZE) / 2.0f;
	return PathPoint(position.x, position.y, MIN_INFLUENCE);
}

std::vector<PathPoint> Path::SmoothPoints(std::vector<PathPoint> ppoints) const
{
	std::vector<PathPoint> smoothPoints;
	smoothPoints.push_back(ppoints[0]);

	for (size_t i = 1; i < ppoints.size() - 1; i++)
	{
		PathPoint previous = ppoints[i - 1];
		PathPoint next = ppoints[i + 1];

		PathPoint newPoint = Lerp(previous, next, 0.5f);
		newPoint.influence = ppoints[i].influence;

		smoothPoints.push_back(newPoint);
	}

	smoothPoints.push_back(ppoints[ppoints.size() - 1]);
	return smoothPoints;
}
