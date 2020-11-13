#include "stdafx.h"
#include "Bounds.h"

Bounds::Bounds() : 
	min(D3D11_FLOAT32_MAX, D3D11_FLOAT32_MAX, D3D11_FLOAT32_MAX), 
	max(-D3D11_FLOAT32_MAX, -D3D11_FLOAT32_MAX, -D3D11_FLOAT32_MAX)
{

}

Bounds::~Bounds()
{
}

void Bounds::CalculateAABB(const std::vector<Mesh*>& meshes)
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
			Float3Min(this->min, this->min, tempMin);
			Float3Max(this->max, this->max, tempMax);
		}
	}
}

void Bounds::CalculateAABB(const Mesh* mesh)
{
	mesh->CalculateMinMax(min, max);
}

void Bounds::Encapsulate(const Bounds& bounds)
{
	Encapsulate(bounds.min, bounds.max);
}

void Bounds::Encapsulate(dx::XMFLOAT3 point)
{
	Encapsulate(point, point);
}

void Bounds::Encapsulate(dx::XMFLOAT3 min, dx::XMFLOAT3 max)
{
	Float3Min(this->min, this->min, min);
	Float3Max(this->max, this->max, max);
}

void Bounds::SetMinMax(dx::XMFLOAT3 min, dx::XMFLOAT3 max)
{
	this->min = min;
	this->max = max;
}

void Bounds::TransformMinMax(dx::XMMATRIX worldTransform, dx::XMFLOAT3& min, dx::XMFLOAT3& max) const
{
	dx::XMFLOAT3 storedTransformedCorner;
	dx::XMVECTOR transformedCorner;
	dx::XMFLOAT3 corners[8];
	GetCorners(corners);

	for (size_t i = 0; i < 8; i++)
	{
		transformedCorner = dx::XMVector3Transform(dx::XMLoadFloat3(&corners[i]), worldTransform);
		dx::XMStoreFloat3(&storedTransformedCorner, transformedCorner);

		if (i == 0)
		{
			min = max = storedTransformedCorner;
		}
		else
		{
			Float3Min(min, min, storedTransformedCorner);
			Float3Max(max, max, storedTransformedCorner);
		}
	}
}

void Bounds::GetCorners(dx::XMFLOAT3 corners[8]) const
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

dx::XMFLOAT3 Bounds::GetCenter() const
{
	return Math::Lerp(min, max, 0.5f);
}

dx::XMFLOAT3 Bounds::GetExtends() const
{
	float extendX = (max.x - min.x) / 2.0f;
	float extendY = (max.y - min.y) / 2.0f;
	float extendZ = (max.z - min.z) / 2.0f;
	return dx::XMFLOAT3(extendX, extendY, extendZ);
}

void Bounds::Float3Min(dx::XMFLOAT3& target, const dx::XMFLOAT3& value0, const dx::XMFLOAT3& value1) const
{
	target.x = std::min(value0.x, value1.x);
	target.y = std::min(value0.y, value1.y);
	target.z = std::min(value0.z, value1.z);
}

void Bounds::Float3Max(dx::XMFLOAT3& target, const dx::XMFLOAT3& value0, const dx::XMFLOAT3& value1) const
{
	target.x = std::max(value0.x, value1.x);
	target.y = std::max(value0.y, value1.y);
	target.z = std::max(value0.z, value1.z);
}
