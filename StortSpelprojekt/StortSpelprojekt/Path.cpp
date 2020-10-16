#include "Path.h"
#include "Chunk.h"

Path::Path(int seed) : distribution(0.0f, 1.0f)
{
	Reseed(seed);
}

Path::~Path()
{
}

void Path::Walk(int indexSteps, int directionalSteps)
{
	CalculateIndexes(indexSteps, directionalSteps);
	CalculatePoints();
}

void Path::Clear()
{
	indexes.clear();
	points.clear();
}

void Path::Reseed(int seed)
{
	rngEngine.seed(seed);
}

float Path::ClosestDistance(const dx::XMFLOAT2& position) const
{
	float shortest = FLT_MAX;
	const float offset = CHUNK_SIZE / 2.0f;

	if (points.size() == 0)
		return 0.0f;

	for (size_t i = 0; i < points.size() - 1; i++)
	{
		dx::XMFLOAT2 p0 = points[i];
		dx::XMFLOAT2 p1 = points[i + 1];

		float tmpDistance = Math::DistanceToLineSqr(position.x, position.y,
			p0.x + offset, p0.y + offset,
			p1.x + offset, p1.y + offset
		);

		if (tmpDistance < shortest)
			shortest = tmpDistance;
	}

	std::cout << shortest << std::endl;
	return sqrtf(shortest);
}

void Path::CalculateIndexes(int steps, int directionalSteps)
{
	dx::XMINT2 direction = { 0,1 };
	dx::XMINT2 current = { 0,0 };
	dx::XMINT2 tmp = { 0,0 };

	size_t maxSteps = steps;
	std::uniform_real_distribution<float> distribution(0.0f, 1.0f);

	while (maxSteps > 0)
	{
		for (size_t i = 0; i < directionalSteps && maxSteps > 0; i++)
		{
			tmp.x = current.x + direction.x;
			tmp.y = current.y + direction.y;
			indexes.push_back(tmp);

			maxSteps--;
			current = tmp;
		}

		UpdateDirection(direction);
	}
}

void Path::CalculatePoints()
{
	const size_t cuts = 2;
	points.resize(indexes.size() * cuts);

	for (size_t i = 0; i < indexes.size() - 1; i++)
	{
		dx::XMFLOAT2 pointA = Chunk::IndexToXZ(indexes[i]);
		dx::XMFLOAT2 pointB = Chunk::IndexToXZ(indexes[i + 1]);

		for (int j = 0; j < cuts; j++)
		{
			float t = (float)j / (float)cuts;
			points.push_back(Math::Lerp(pointA, pointB, t));
		}
	}

	// add last
	points.push_back(Chunk::IndexToXZ(indexes[indexes.size() - 1]));

//	points = SmoothPoints(points, 3.0f, 1.0f);


	const int a = 0;
}

void Path::UpdateDirection(dx::XMINT2& direction)
{
	float value = distribution(rngEngine);

	if (direction.y == 0)
	{
		if (value < 0.8f)
			direction = { 0, 1 };
	}
	else
	{
		if (value < 0.33f)
			direction = { -1, 0 };
		else if (value > 0.66f)
			direction = { 1, 0 };
	}
}

std::vector<dx::XMFLOAT2> Path::SmoothPoints(std::vector<dx::XMFLOAT2>& points, float smoothness, float cornerRadius)
{
	std::vector<dx::XMFLOAT2> smoothedPoints;
	smoothedPoints.push_back(points[0]);
	
	for (size_t i = 1; i < points.size() - 1; i++)
	{
		dx::XMFLOAT2 previous = points[Wrap(i - 1, points.size())];
		previous = Math::Move(points[i], previous, cornerRadius);

		dx::XMFLOAT2 next = points[Wrap(i + 1, points.size())];
		next = Math::Move(points[i], next, cornerRadius);

		std::vector<dx::XMFLOAT2> curve = Math::SmoothCurve(previous, next, points[i], smoothness);
		smoothedPoints.insert(smoothedPoints.end(), curve.begin(), curve.end());
	}

	return smoothedPoints;
}
