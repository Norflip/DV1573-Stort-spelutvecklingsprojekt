#include "stdafx.h"
#include "Path.h"
#include "Noise.h"
#include <algorithm>

Path::Path() {}
Path::Path(const std::vector<dx::XMINT2>& indexes)
{
	SetPointsFromIndexes(indexes);
	CreateLineSegments();
	CreateLanternPoints();
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

	float shortest = 0.0f;
	float influence = 0.0f;
	float totResult = 0.0f;
	float t, result;

	for (size_t i = 0; i < segments.size(); i++)
	{
		float distance = segments[i].GetDistance(position.x, position.y, t);
		if (distance < shortest || i == 0)
		{
			shortest = distance;
			result = Math::Lerp(segments[i].start.influence, segments[i].end.influence, t);
			influence = (shortest - result) / (INFLUENCE_FADE_DISTANCE);
		}
	}

	return Math::Clamp01(influence);
}

void Path::GetLanternInformation(std::vector<dx::XMFLOAT2>& positions, std::vector<float>& axisAngle) const
{
	positions = this->lanternPoints;
	axisAngle = this->lanternAxisAngle;
}

void Path::DrawDebug()
{
	const float height = 1.0f;
	auto segments =GetLineSegments();
	for (size_t i = 0; i < segments.size(); i++)
	{
		dx::XMFLOAT3 worldPoint0(segments[i].start.x, height, segments[i].start.z);
		dx::XMFLOAT3 worldPoint1(segments[i].end.x, height, segments[i].end.z);

		DShape::DrawSphere(worldPoint0, 0.4f, { 1,0,1 });
		//DShape::DrawWireSphere(worldPoint0, segments[i].start.influence, { 0,1,0.2f });
		//DShape::DrawWireSphere(worldPoint0, segments[i].start.influence + (INFLUENCE_FADE_DISTANCE / 2.0f), { 0,1,0.8f });

		DShape::DrawLine(worldPoint0, worldPoint1, { 1,0,0 });

	}

	dx::XMFLOAT3 signWorldPos (signPosition.x, height, signPosition.y);
	DShape::DrawSphere(signWorldPos, 2.0f, { 1,1,1 });

	for (size_t i = 0; i < lanternPoints.size(); i++)
	{
		dx::XMFLOAT3 worldPoint(lanternPoints[i].x, height, lanternPoints[i].y);
		dx::XMFLOAT3 worldPoint0(lanternPoints[i].x, height + 10.0f, lanternPoints[i].y);

		DShape::DrawSphere(worldPoint, 0.4f, { 0.1f,0.2f,1 });
		DShape::DrawLine(worldPoint0, worldPoint, { 1,0,0 });

	}
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

	for (size_t i = 0; i < basePoints.size() - 1; i++)
	{
		PathPoint pointA = basePoints[i];
		PathPoint pointB = basePoints[i + 1];

		dx::XMFLOAT2 direction = dx::XMFLOAT2(pointB.x - pointA.x, pointB.z - pointA.z);
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
			point.z += right.y * noise * offset;
			newPoints.push_back(point);
		}
	}

	newPoints.push_back(basePoints[basePoints.size() - 1]);
	points = SmoothPoints(SmoothPoints(newPoints));
}

void Path::CreateLineSegments()
{
	segments.clear();
	const float distanceOffset = CHUNK_SIZE * 5.0f;

	for (size_t i = 0; i < points.size() - 1; i++)
	{
		LineSegment segment(points[i], points[i + 1]);
		segment.start.x = roundf(segment.start.x);
		segment.start.z = roundf(segment.start.z);
		segment.end.x = roundf(segment.end.x);
		segment.end.z = roundf(segment.end.z);

		if (i == 0)
		{
			dx::XMFLOAT2 direction = segment.Direction();
			float x = points[i].x + (-direction.x) * distanceOffset;
			float y = points[i].z + (-direction.y) * distanceOffset;
			PathPoint offsetPoint(x, y, points[i].influence);
			segments.push_back(LineSegment(offsetPoint, points[i]));
			extraSegments.push_back(LineSegment(offsetPoint, points[i]));
		}

		segments.push_back(segment);
	}

	LineSegment lastSegment = segments[segments.size() - 1];
	LineSegment firstSegment = segments[0];

	PathPoint lastPoint = lastSegment.end;
	PathPoint firstPoint = firstSegment.start;

	dx::XMFLOAT2 lastSegDirection = lastSegment.Direction();
	dx::XMFLOAT2 firstSegmentDirection = firstSegment.Direction();

	dx::XMFLOAT2 lastRight = dx::XMFLOAT2(-lastSegDirection.y, lastSegDirection.x);
	dx::XMFLOAT2 firstRight = dx::XMFLOAT2(-firstSegmentDirection.y, firstSegmentDirection.x);

	dx::XMFLOAT2 lastLeftDirection = Math::Lerp(lastSegDirection, dx::XMFLOAT2(-lastRight.x, -lastRight.y), 0.5f);
	dx::XMFLOAT2 firstLeftDirection = Math::Lerp(firstSegmentDirection, dx::XMFLOAT2(-firstRight.x, -firstRight.y), 0.5f);

	PathPoint endLeft(lastPoint.x + lastLeftDirection.x * distanceOffset, lastPoint.z + lastLeftDirection.y * distanceOffset, lastPoint.influence);
	PathPoint firstLeft(firstPoint.x + firstLeftDirection.x * distanceOffset, firstPoint.z + firstLeftDirection.y * distanceOffset, firstPoint.influence);

	dx::XMFLOAT2 endRightDirection = Math::Lerp(lastSegDirection, lastRight, 0.5f);
	dx::XMFLOAT2 firstRightDirection = Math::Lerp(firstSegmentDirection, firstRight, 0.5f);

	PathPoint endRight(lastPoint.x + (endRightDirection.x) * distanceOffset, lastPoint.z + (endRightDirection.y) * distanceOffset, lastPoint.influence);
	PathPoint startRight(firstPoint.x + (firstRightDirection.x) * distanceOffset, firstPoint.z + (firstSegmentDirection.y) * distanceOffset, firstPoint.influence);
	
	//Sign
	signPosition.x = lastPoint.x + lastSegDirection.x * (MAX_INFLUENCE * 0.5f);
	signPosition.y = lastPoint.z + lastSegDirection.y * (MAX_INFLUENCE * 0.5f);

	signRotation.x = lastPoint.x + lastSegDirection.x * MAX_INFLUENCE;
	signRotation.y = lastPoint.z + lastSegDirection.y * MAX_INFLUENCE;

	//Player
	playerSwitchPosition.x = firstPoint.x + firstSegmentDirection.x * 1.5f;
	playerSwitchPosition.y = firstPoint.z + firstSegmentDirection.y * 1.5f;


	segments.push_back(LineSegment(lastPoint, endLeft));
	segments.push_back(LineSegment(lastPoint, endRight));

	extraSegments.push_back(LineSegment(lastPoint, endLeft));
	extraSegments.push_back(LineSegment(lastPoint, endRight));
	// add level select here

}

void Path::CreateLanternPoints()
{
	int sign = 1;
	const float lanternOffset = 20.0f;

	float offset = lanternOffset;
	float tmpDistance, rightX, rightY;
	dx::XMFLOAT2 tmpDirection;

	for (size_t i = 0; i < points.size() - 1; i++)
	{
		GetDistanceDirection(points[i], points[i + 1], tmpDistance, tmpDirection);
		offset -= tmpDistance;
		
		if (offset < 0.0f)
		{
			offset = lanternOffset;
			rightX = -tmpDirection.y;
			rightY = tmpDirection.x;

			float offsetX = points[i].x + rightX * (points[i].influence + INFLUENCE_FADE_DISTANCE * 0.3f) * sign;
			float offsetZ = points[i].z + rightY * (points[i].influence + INFLUENCE_FADE_DISTANCE * 0.3f) * sign;

			float dirX = (points[i].x - offsetX);
			float dirY = (points[i].z - offsetX);

			float angle = Math::NormalizeAngle(atan2(rightY, rightX) + (Math::PI * 0.5f * -1.0f * sign));

			lanternAxisAngle.push_back(angle);
			lanternPoints.push_back(dx::XMFLOAT2(offsetX, offsetZ));
			sign *= (Random::Value() > 0.5f ? 1 : -1);
		}
	}

	//std::cout << "LANTERNS: " << lanternPoints.size() << std::endl;
}

void Path::GetDistanceDirection (const PathPoint& point0, const PathPoint& point1, float& distance, dx::XMFLOAT2& direction) const
{
	float dx = point1.x - point0.x;
	float dz = point1.z - point0.z;
	distance = sqrtf(dx * dx + dz * dz);

	direction.x = dx / distance;
	direction.y = dz / distance;
}

PathPoint Path::Lerp(const PathPoint& a, const PathPoint& b, float t) const
{
	return PathPoint(Math::Lerp(a.x, b.x, t), Math::Lerp(a.z, b.z, t), Math::Lerp(a.influence, b.influence, t));
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
