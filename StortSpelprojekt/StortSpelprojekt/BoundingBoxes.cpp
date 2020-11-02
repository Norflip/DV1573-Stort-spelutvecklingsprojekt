#include "stdafx.h"
#include "BoundingBoxes.h"

BoundingBox::BoundingBox()
{
}

BoundingBox::~BoundingBox()
{
}

void BoundingBox::CalculateAABB(const std::vector<Mesh*>& meshes)
{
	DirectX::XMFLOAT3 tempMin, tempMax;
	for (size_t i = 0; i < meshes.size(); i++)
	{
		meshes[i]->CalculateMinMax(tempMin, tempMax);
		if (i == 0)
		{
			this->min = tempMin;
			this->max = tempMax;
		}
		else
		{
			this->min.x = std::min(this->min.x, tempMin.x);
			this->min.y = std::min(this->min.y, tempMin.y);
			this->min.z = std::min(this->min.z, tempMin.z);
			this->max.x = std::max(this->max.x, tempMax.x);
			this->max.y = std::max(this->max.y, tempMax.y);
			this->max.z = std::max(this->max.z, tempMax.z);
		}
	}
}

void BoundingBox::CalculateAABB(const Mesh* mesh)
{
	mesh->CalculateMinMax(min, max);
}

void BoundingBox::GetCorners(dx::XMFLOAT3 corners[8])
{
	size_t i = 0;
	for (size_t z = 0; z <= 1; z++)
	{
		for (size_t y = 0; y <= 1; y++)
		{
			for (size_t x = 0; x <= 1; x++)
			{
				corners[i].x = Math::Lerp(min.x, max.x, FCAST(x));
				corners[i].y = Math::Lerp(min.y, max.y, FCAST(y));
				corners[i].z = Math::Lerp(min.z, max.z, FCAST(z));
				i++;
			}
		}
	}
}

dx::XMFLOAT3 BoundingBox::GetCenter() const
{
	return Math::Lerp(min, max, 0.5f);
}

dx::XMFLOAT3 BoundingBox::GetExtends() const
{
	float extendX = (max.x - min.x) / 2.0f;
	float extendY = (max.y - min.y) / 2.0f;
	float extendZ = (max.z - min.z) / 2.0f;
	return dx::XMFLOAT3(extendX, extendY, extendZ);
}
