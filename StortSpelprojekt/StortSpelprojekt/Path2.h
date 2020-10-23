#pragma once
#include <vector>
#include <DirectXMath.h>
namespace dx = DirectX;

#include "Chunk.h"

class Path2
{
public:
	Path2();
	Path2(const std::vector<dx::XMINT2>& indexes);
	virtual ~Path2();

	void Clear();
	float ClosestDistance(const dx::XMFLOAT2& position) const;

	std::vector<dx::XMFLOAT2> GetPoints() const { return this->points; }
	dx::XMFLOAT2 GetPoint(size_t index) { return this->points[index]; }
	size_t CountPoints() const { return this->points.size(); }

	std::vector<dx::XMINT2> GetIndexes() const { return this->indexes; }

private:
	void SetPointsFromIndexes(const std::vector<dx::XMINT2>& indexes);
	float DistanceToLineSqr(float px, float py, float line0x, float line0y, float line1x, float line1y) const;
	//float Distance(const dx::XMFLOAT2& point, const dx::XMFLOAT2& t0, const dx::XMFLOAT2& t1) const;

	std::vector<dx::XMINT2> indexes;
	std::vector<dx::XMFLOAT2> points;
};