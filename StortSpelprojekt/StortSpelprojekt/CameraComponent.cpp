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

std::vector<dx::XMFLOAT4> CameraComponent::GetFrustumPlanes()
{
	if (GetOwner()->GetTransform().ChangedThisFrame() || frustumPlanes.size() == 0)
	{
		// x, y, z, and w represent A, B, C and D in the plane equation
		// where ABC are the xyz of the planes normal, and D is the plane constant
		if (frustumPlanes.size() != 6)
			frustumPlanes.resize(6);
		//r means row

		dx::XMMATRIX vp = dx::XMMatrixMultiply(GetViewMatrix(), GetProjectionMatrix());

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
	bool inViewResult = true;

	DirectX::XMFLOAT3 min, max, vmin, vmax;
	min = aabb.min;
	max = aabb.max;

	min.x += worldPos.x;
	min.y += worldPos.y;
	min.z += worldPos.z;

	max.x += worldPos.x;
	max.y += worldPos.y;
	max.z += worldPos.z;

	for (int i = 0; i < 6; i++)
	{
		DirectX::XMFLOAT4 plane = frustumPlanes[i];

		// X axis
		if (plane.x < 0)
		{
			vmin.x = min.x;
			vmax.x = max.x;
		}
		else
		{
			vmin.x = max.x;
			vmax.x = min.x;
		}

		// Y axis
		if (plane.y < 0)
		{
			vmin.y = min.y;
			vmax.y = max.y;
		}
		else
		{
			vmin.y = max.y;
			vmax.y = min.y;
		}

		// Z axis
		if (plane.z < 0)
		{
			vmin.z = min.z;
			vmax.z = max.z;
		}
		else
		{
			vmin.z = max.z;
			vmax.z = min.z;
		}

		float d1 = plane.x * vmin.x + plane.y * vmin.y + plane.z * vmin.z;
		if (d1 + plane.w < 0)
			return false;

		bool intersectsFrustum = false;

		if (intersectsFrustum)
		{
			float d2 = plane.x * vmax.x + plane.y * vmax.y + plane.z * vmax.z;
			if (d2 + plane.w <= 0)
				inViewResult = true;
		}
	}

	return !inViewResult;
}

