#include "stdafx.h"
#include "CameraComponent.h"

CameraComponent* CameraComponent::mainCamera = nullptr;

CameraComponent::CameraComponent(float fieldOfView, bool main) : fieldOfView(fieldOfView)
{
	if (main) mainCamera = this;
}
CameraComponent::~CameraComponent() {}

void CameraComponent::SetFOV(float fieldOFiew)
{
	this->fieldOfView = fieldOFiew;
	UpdateProjectionMatrix();
}

void CameraComponent::Resize(size_t width, size_t height)
{
	this->width = width;
	this->height = height;
	UpdateProjectionMatrix();
}

dx::XMMATRIX CameraComponent::GetViewMatrix() const
{
	Transform& transform = GetOwner()->GetTransform();

	dx::XMVECTOR forward = transform.TransformDirection({ 0,0,1 });
	dx::XMVECTOR up = transform.TransformDirection({ 0,1,0 });

	return dx::XMMatrixLookToLH(transform.GetPosition(), forward, up);
}

dx::XMMATRIX CameraComponent::GetProjectionMatrix() const
{
	return dx::XMLoadFloat4x4(&projection);
}

void CameraComponent::UpdateProjectionMatrix()
{
	const float toRadians = 0.0174532925f;
	float aspect = static_cast<float>(width) / static_cast<float>(height);
	dx::XMStoreFloat4x4(&projection, DirectX::XMMatrixPerspectiveFovLH(fieldOfView * toRadians, aspect, CAMERA_NEAR_Z, CAMERA_FAR_Z));
}

std::vector<dx::XMFLOAT4>& CameraComponent::GetFrustumPlanes()
{
	//if (GetOwner()->GetTransform().ChangedThisFrame() || frustumPlanes.size() == 0)
	//{
		//std::cout << "update frustum" << std::endl;

		// x, y, z, and w represent A, B, C and D in the plane equation
		// where ABC are the xyz of the planes normal, and D is the plane constant
	if (frustumPlanes.size() != 6)
		frustumPlanes = std::vector<dx::XMFLOAT4>(6);
	//r means row

	dx::XMMATRIX vp = dx::XMMatrixMultiply(GetViewMatrix(), GetProjectionMatrix());
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
	//	}
	return frustumPlanes;
}

bool CameraComponent::InView(const BoundingBox& bounds, const dx::XMMATRIX world)
{
	auto& planes = GetFrustumPlanes();
	bool inViewResult = true;

	DirectX::XMFLOAT3 min, max, vmin, vmax;
	dx::XMStoreFloat3(&min, dx::XMVector3Transform(dx::XMLoadFloat3(&bounds.GetMin()), world));
	dx::XMStoreFloat3(&max, dx::XMVector3Transform(dx::XMLoadFloat3(&bounds.GetMax()), world));

	for (int i = 0; i < 6; i++)
	{
		DirectX::XMFLOAT4& plane = frustumPlanes[i];
		vmin = min;
		vmax = max;

		if (plane.x >= 0)
			std::swap(vmin.x, vmax.x);

		if (plane.y >= 0)
			std::swap(vmin.y, vmax.y);

		if (plane.z >= 0)
			std::swap(vmin.z, vmax.z);

		// INSIDE
		float d1 = plane.x * vmin.x + plane.y * vmin.y + plane.z * vmin.z;
		if (d1 + plane.w < 0.0f)
			return false;

		// ON FRUSTUM BORDER
		float d2 = plane.x * vmax.x + plane.y * vmax.y + plane.z * vmax.z;
		if (d2 + plane.w <= 0)
			return true;
	}

	return inViewResult;
}

Ray CameraComponent::MouseToRay(const unsigned int& x, const unsigned int& y) const
{
	dx::XMVECTOR screenPosition = dx::XMVectorSet(FCAST(x), FCAST(y), 0.0f, 0.0f);
	dx::XMVECTOR worldPosition = dx::XMVector3Unproject(screenPosition, 0, 0, (float)width, (float)height, 0.0f, 1.0f, GetProjectionMatrix(), GetViewMatrix(), dx::XMMatrixIdentity());

	dx::XMVECTOR position = GetOwner()->GetTransform().GetPosition();
	dx::XMVECTOR dir = dx::XMVectorSubtract(worldPosition, position);

	dx::XMFLOAT3 direction, origin;
	dx::XMStoreFloat3(&direction, dx::XMVector3Normalize(dir));
	dx::XMStoreFloat3(&origin, position);
	return Ray(origin, direction);
}
