#include "CameraComponent.h"

CameraComponent::CameraComponent(float fieldOfView) : fieldOfView(fieldOfView) {}
CameraComponent::~CameraComponent() {}

void CameraComponent::Resize(size_t width, size_t height)
{
	this->width = width;
	this->height = height;
	UpdateProjectionMatrix();
}

dx::XMMATRIX CameraComponent::GetViewMatrix() const
{
	Transform& transform = GetOwner()->GetTransform();

	return dx::XMMatrixLookToLH(
		transform.GetPosition(),
		transform.TransformDirection({ 0,0,1 }),
		transform.TransformDirection({ 0,1,0 })
	);
}

void CameraComponent::UpdateProjectionMatrix()
{
	const float toRadians = 0.0174532925f;
	float aspect = static_cast<float>(width) / static_cast<float>(height);
	this->projection = DirectX::XMMatrixPerspectiveFovLH(fieldOfView * toRadians, aspect, CAMERA_NEAR_Z, CAMERA_FAR_Z);
}

std::vector<dx::XMFLOAT4> CameraComponent::GetFrustumPlanes()
{
	if (GetOwner()->GetTransform().ChangedThisFrame())
	{
		// x, y, z, and w represent A, B, C and D in the plane equation
		// where ABC are the xyz of the planes normal, and D is the plane constant
		frustumPlanes.resize(6);
		//r means row

		dx::XMMATRIX vp = GetVIewAndProjectionMatrix();

		std::cout << "Updating planes" << std::endl;


		// Left Frustum Plane
	   // Add first column of the matrix to the fourth column
		frustumPlanes[0].x = vp.r[0].m128_f32[3] + vp.r[0].m128_f32[0];
		frustumPlanes[0].y = vp.r[1].m128_f32[3] + vp.r[1].m128_f32[0];
		frustumPlanes[0].z = vp.r[2].m128_f32[3] + vp.r[2].m128_f32[0];
		frustumPlanes[0].w = vp.r[3].m128_f32[3] + vp.r[3].m128_f32[0];

		// Right Frustum Plane
		// Subtract first column of matrix from the fourth column

		frustumPlanes[1].x = vp.r[0].m128_f32[3] - vp.r[0].m128_f32[0];
		frustumPlanes[1].y = vp.r[1].m128_f32[3] - vp.r[1].m128_f32[0];
		frustumPlanes[1].z = vp.r[2].m128_f32[3] - vp.r[2].m128_f32[0];
		frustumPlanes[1].w = vp.r[3].m128_f32[3] - vp.r[3].m128_f32[0];

		// Top Frustum Plane
		// Subtract second column of matrix from the fourth column

		frustumPlanes[2].x = vp.r[0].m128_f32[3] - vp.r[0].m128_f32[1];
		frustumPlanes[2].y = vp.r[1].m128_f32[3] - vp.r[1].m128_f32[1];
		frustumPlanes[2].z = vp.r[2].m128_f32[3] - vp.r[2].m128_f32[1];
		frustumPlanes[2].w = vp.r[3].m128_f32[3] - vp.r[3].m128_f32[1];

		// Bottom Frustum Plane
		// Add second column of the matrix to the fourth column

		frustumPlanes[3].x = vp.r[0].m128_f32[3] + vp.r[0].m128_f32[1];
		frustumPlanes[3].y = vp.r[1].m128_f32[3] + vp.r[1].m128_f32[1];
		frustumPlanes[3].z = vp.r[2].m128_f32[3] + vp.r[2].m128_f32[1];
		frustumPlanes[3].w = vp.r[3].m128_f32[3] + vp.r[3].m128_f32[1];


		// Near Frustum Plane
		// We could add the third column to the fourth column to get the near plane,
		// but we don't have to do this because the third column IS the near plane


		frustumPlanes[4].x = vp.r[0].m128_f32[2];
		frustumPlanes[4].y = vp.r[1].m128_f32[2];
		frustumPlanes[4].z = vp.r[2].m128_f32[2];
		frustumPlanes[4].w = vp.r[3].m128_f32[2];

		// Far Frustum Plane
		// Subtract third column of matrix from the fourth column

		frustumPlanes[5].x = vp.r[0].m128_f32[3] - vp.r[0].m128_f32[2];
		frustumPlanes[5].y = vp.r[1].m128_f32[3] - vp.r[1].m128_f32[2];
		frustumPlanes[5].z = vp.r[2].m128_f32[3] - vp.r[2].m128_f32[2];
		frustumPlanes[5].w = vp.r[3].m128_f32[3] - vp.r[3].m128_f32[2];


		// Normalize plane normals (A, B and C (xyz))
	   // Also take note that planes face inward

		for (int i = 0; i < 6; ++i)
		{
			float length = sqrt((frustumPlanes[i].x * frustumPlanes[i].x) + (frustumPlanes[i].y * frustumPlanes[i].y) + (frustumPlanes[i].z * frustumPlanes[i].z));
			frustumPlanes[i].x /= length;
			frustumPlanes[i].y /= length;
			frustumPlanes[i].z /= length;
			frustumPlanes[i].w /= length;
		}
	}

	return frustumPlanes;
}

bool CameraComponent::CullAgainstAABB(const AABB& aabb, const dx::XMFLOAT3 worldPos)
{
	auto planes = GetFrustumPlanes();

	for (unsigned int plane = 0; plane < 6; ++plane)
	{
		DirectX::XMVECTOR planeNormal = DirectX::XMVectorSet(planes[plane].x, planes[plane].y, planes[plane].z, 0.0f);
		float planeConstant = planes[plane].w;

		DirectX::XMFLOAT3 diagonal;
		
		if (planes[plane].x < 0.0f)
		{
			diagonal.x = aabb.min.x + worldPos.x;
		}
		else
		{
			diagonal.x = aabb.max.x + worldPos.x;
		}
		if (planes[plane].y < 0.0f)
		{
			diagonal.y = aabb.min.y + worldPos.y;
		}
		else
		{
			diagonal.y = aabb.max.y + worldPos.y;
		}
		if (planes[plane].z < 0.0f)
		{
			diagonal.z = aabb.min.z + worldPos.z;
		}
		else
		{
			diagonal.z = aabb.max.z + worldPos.z;
		}

		if (DirectX::XMVectorGetX(DirectX::XMVector3Dot(planeNormal, DirectX::XMLoadFloat3(&diagonal))) + planeConstant < 0.0f)
		{
			return true;
		}
	}

	return false;
}

