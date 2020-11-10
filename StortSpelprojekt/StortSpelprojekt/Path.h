#pragma once
#include <vector>
#include <DirectXMath.h>
#include "Chunk.h"

namespace dx = DirectX;

constexpr float MIN_INFLUENCE = 1.0f;
constexpr float MAX_INFLUENCE = 8.0f;
constexpr float INFLUENCE_FADE_DISTANCE = 10.0f;

struct PathPoint
{
	float x, y, influence;

	PathPoint(float x, float y, float influence)
		: x(x), y(y), influence(influence) {}

	dx::XMFLOAT2 AsFloat2() { return dx::XMFLOAT2(x, y); }
};

struct LineSegment
{
	PathPoint start;
	PathPoint end;

	LineSegment(PathPoint start, PathPoint end) : start(start), end(end) {}

	PathPoint Lerp(float t) const
	{
		return PathPoint(Math::Lerp(start.x, end.x, t), Math::Lerp(start.y, end.y, t), Math::Lerp(start.influence, end.influence, t));
	}

	inline float Length() const
	{
		float dx = (end.x - start.x);
		float dy = (end.y - start.y);
		return sqrtf(dx * dx + dy * dy);
	}

	inline dx::XMFLOAT2 Direction() const
	{
		float dx = (end.x - start.x);
		float dy = (end.y - start.y);
		float length = sqrtf(dx * dx + dy * dy);
		return dx::XMFLOAT2(dx / length, dy / length);
	}

	inline float GetDistance(const float& x, const float& y, float& t) const
	{
		float edge1ToPointX = x - start.x;
		float edge1ToPointY = y - start.y;

		float edge1ToEdge2X = end.x - start.x;
		float edge1ToEdge2Y = end.y - start.y;

		float dot = edge1ToPointX * edge1ToEdge2X + edge1ToPointY * edge1ToEdge2Y;
		float lengthSqr = edge1ToEdge2X * edge1ToEdge2X + edge1ToEdge2Y * edge1ToEdge2Y;
		
		t = dot / lengthSqr;
		t = Math::Clamp01(t);
		
		float dx = x - (start.x + t * edge1ToEdge2X);
		float dy = y - (start.y + t * edge1ToEdge2Y);
		return sqrtf(dx * dx + dy * dy);
	}
};

class Path
{
public:
	Path();
	Path(const std::vector<dx::XMINT2>& indexes);
	virtual ~Path();

	void Clear();
	float SampleInfluence(const dx::XMFLOAT2& position) const;

	std::vector<LineSegment> GetLineSegments() const { return this->segments; }
	std::vector<PathPoint> GetPoints() const { return this->points; }
	PathPoint GetPoint(size_t index) { return this->points[index]; }
	size_t CountPoints() const { return this->points.size(); }

	std::vector<dx::XMINT2> GetIndexes() const { return this->indexes; }

	size_t GetFirstPointIndex() const { return 0; }
	size_t GetLastPointIndex() const { return points.size() - 1; }

private:
	void SetPointsFromIndexes(const std::vector<dx::XMINT2>& indexes);
	void CreateLineSegments();

	//dx::XMFLOAT2 DistanceToLineSqr(const float& px, const float& py, const float& line0x, const float& line0y, const float& line1x, const float& line1y) const;

	PathPoint Lerp(const PathPoint& a, const PathPoint& b, float t) const;
	PathPoint IndexToPoint(const dx::XMINT2& index) const;

	std::vector<PathPoint> SmoothPoints(std::vector<PathPoint> points) const;

	//std::vector<Point> SmoothPoints() const;
	//std::vector<Point> SmoothCurve(Point previous, Point current, Point next, float smoothness) const;

	std::vector<dx::XMINT2> indexes;
	std::vector<PathPoint> points;
	std::vector<LineSegment> segments;
};