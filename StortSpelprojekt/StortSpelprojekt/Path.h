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
	float x, z, influence;

	PathPoint(float x, float y, float influence)
		: x(x), z(y), influence(influence) {}

	dx::XMFLOAT2 AsFloat2() { return dx::XMFLOAT2(x, z); }
	dx::XMFLOAT3 AsFloat3(float y) { return dx::XMFLOAT3(x, y, this->z); }
};

struct LineSegment
{
	PathPoint start;
	PathPoint end;

	LineSegment(PathPoint start, PathPoint end) : start(start), end(end) {}

	PathPoint Lerp(float t) const
	{
		return PathPoint(Math::Lerp(start.x, end.x, t), Math::Lerp(start.z, end.z, t), Math::Lerp(start.influence, end.influence, t));
	}

	inline float Length() const
	{
		float dx = (end.x - start.x);
		float dz = (end.z - start.z);
		return sqrtf(dx * dx + dz * dz);
	}

	inline dx::XMFLOAT2 Direction() const
	{
		float dx = (end.x - start.x);
		float dz = (end.z - start.z);
		float length = sqrtf(dx * dx + dz * dz);
		return dx::XMFLOAT2(dx / length, dz / length);
	}

	inline float GetDistance(const float& x, const float& y, float& t) const
	{
		float edge1ToPointX = x - start.x;
		float edge1ToPointY = y - start.z;

		float edge1ToEdge2X = end.x - start.x;
		float edge1ToEdge2Y = end.z - start.z;

		float dot = edge1ToPointX * edge1ToEdge2X + edge1ToPointY * edge1ToEdge2Y;
		float lengthSqr = edge1ToEdge2X * edge1ToEdge2X + edge1ToEdge2Y * edge1ToEdge2Y;
		
		t = dot / lengthSqr;
		t = Math::Clamp01(t);
		
		float dx = x - (start.x + t * edge1ToEdge2X);
		float dz = y - (start.z + t * edge1ToEdge2Y);
		return sqrtf(dx * dx + dz * dz);
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
	void DrawDebug();

private:
	void SetPointsFromIndexes(const std::vector<dx::XMINT2>& indexes);
	void CreateLineSegments();

	PathPoint Lerp(const PathPoint& a, const PathPoint& b, float t) const;
	PathPoint IndexToPoint(const dx::XMINT2& index) const;
	std::vector<PathPoint> SmoothPoints(std::vector<PathPoint> points) const;

	std::vector<dx::XMINT2> indexes;
	std::vector<PathPoint> points;
	std::vector<LineSegment> segments;
};