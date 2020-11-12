#include "stdafx.h"
#include "VirtualCamera.h"

VirtualCamera::VirtualCamera(CameraMode mode, size_t width, size_t height, float nearZ, float farZ, float lensValue) : width(width), height(height),  lensValue(lensValue), mode(mode), farZ(farZ), nearZ(nearZ)
{
	UpdateProjectionMatrix(width, height, lensValue, nearZ, farZ);
}

VirtualCamera::~VirtualCamera() {}

dx::XMMATRIX VirtualCamera::GetViewMatrix(const dx::XMMATRIX& transform) const
{
	dx::XMVECTOR pos, rot, scale;
	dx::XMMatrixDecompose(&scale, &rot, &pos, transform);

	dx::XMVECTOR forward = DirectX::XMVector3Rotate(dx::XMVector3Normalize({ 0,0,1 }), rot);	// rotates the direction with the matrix
	dx::XMVECTOR up = DirectX::XMVector3Rotate(dx::XMVector3Normalize({ 0,1,0 }), rot);

	return dx::XMMatrixLookToLH(pos, forward, up);
}

dx::XMMATRIX VirtualCamera::GetProjectionMatrix() const
{
	return dx::XMLoadFloat4x4(&projection);
}

bool VirtualCamera::InView(const Bounds& bounds, const dx::XMMATRIX world, std::vector<dx::XMFLOAT4>& planes) const
{
	bool inViewResult = true;
	int count = 0;

	DirectX::XMFLOAT3 min, max, vmin, vmax;
	bounds.TransformMinMax(world, min, max);

	for (int i = 0; i < 6; i++)
	{
		DirectX::XMFLOAT4& plane = planes[i];
		vmin = min;
		vmax = max;

		if (plane.x >= 0)
			Swap(vmin.x, vmax.x);

		if (plane.y >= 0)
			Swap(vmin.y, vmax.y);

		if (plane.z >= 0)
			Swap(vmin.z, vmax.z);

		// OUTSIDE
		float d1 = plane.x * vmin.x + plane.y * vmin.y + plane.z * vmin.z;	//dot
		if (d1 + plane.w <= 0.0f)
			return false;

		//// ON FRUSTUM BORDER
		//float d2 = plane.x * vmax.x + plane.y * vmax.y + plane.z * vmax.z;
		//if (d2 + plane.w <= 0)
		//	inViewResult = true;
	}

	return inViewResult;
}


void VirtualCamera::UpdateProjectionMatrix(size_t width, size_t height, float value, float nearZ, float farZ) 
{
	this->width = width;
	this->height = height;
	this->lensValue = value;
	this->farZ = farZ;
	this->nearZ = nearZ;

	float fWidth = FCAST(width);
	float fHeight = FCAST(height);

	if (mode == CameraMode::PERSPECTIVE)
	{
		const float toRadians = 0.0174532925f;
		float fieldOfView = value * toRadians;
		dx::XMStoreFloat4x4(&projection, dx::XMMatrixPerspectiveFovLH(fieldOfView, fWidth / fHeight, nearZ, farZ));
	}
	else
	{
		dx::XMStoreFloat4x4(&projection, dx::XMMatrixOrthographicLH(fWidth, fHeight, nearZ, farZ));
	}
}

void VirtualCamera::Swap(float& a, float& b) const
{
	float tmp = a;
	a = b;
	b = tmp;
}

std::vector<dx::XMFLOAT4> VirtualCamera::GetFrustumPlanes(const dx::XMMATRIX& transform) const
{
	std::vector<dx::XMFLOAT4> frustumPlanes(6);

	dx::XMMATRIX vp = dx::XMMatrixMultiply(GetViewMatrix(transform), GetProjectionMatrix());
	DirectX::XMFLOAT4X4 mat;
	XMStoreFloat4x4(&mat, vp);

	frustumPlanes[0] = { mat._14 - mat._11, mat._24 - mat._21, mat._34 - mat._31, mat._44 - mat._41 }; //right
	frustumPlanes[1] = { mat._14 + mat._11, mat._24 + mat._21, mat._34 + mat._31, mat._44 + mat._41 }; //left
	frustumPlanes[2] = { mat._14 + mat._12, mat._24 + mat._22, mat._34 + mat._32, mat._44 + mat._42 }; //bottom
	frustumPlanes[3] = { mat._14 - mat._12, mat._24 - mat._22, mat._34 - mat._32, mat._44 - mat._42 }; //top
	frustumPlanes[4] = { mat._13, mat._23, mat._33, mat._43 }; //near
	frustumPlanes[5] = { mat._14 - mat._13, mat._24 - mat._23, mat._34 - mat._33, mat._44 - mat._43 }; //far

	DirectX::XMFLOAT4 n;

	for (size_t i = 0; i < 6; i++)
	{
		n = frustumPlanes[i];
		float d = (float)sqrtf(n.x * n.x + n.y * n.y + n.z * n.z);

		frustumPlanes[i].x /= d;
		frustumPlanes[i].y /= d;
		frustumPlanes[i].z /= d;
		frustumPlanes[i].w /= d;
	}
	//}
	return frustumPlanes;
}