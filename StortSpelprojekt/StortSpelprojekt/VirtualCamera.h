#pragma once


class VirtualCamera
{
public:
	enum CameraMode
	{
		ORTHOGRAPHIC,
		PERSPECTIVE
	};

public:
	VirtualCamera(CameraMode mode, size_t width, size_t height, float nearZ, float farZ, float fov = 60.0f);
	virtual ~VirtualCamera();

	virtual dx::XMMATRIX GetViewMatrix(const dx::XMMATRIX& transform) const;
	virtual dx::XMMATRIX GetProjectionMatrix() const;

	virtual bool InView(const Bounds& bounds, const dx::XMMATRIX world, std::vector<dx::XMFLOAT4>& planes) const;
	virtual std::vector<dx::XMFLOAT4> GetFrustumPlanes(const dx::XMMATRIX& transform) const;	
	void UpdateProjectionMatrix(size_t width, size_t height, float value, float nearZ, float farZ);
	
protected:
	dx::XMFLOAT4X4 projection;
	dx::XMFLOAT4X4 view;
	
	void Swap(float& a, float& b) const;

	CameraMode mode;
	size_t width, height;
	float farZ, nearZ;
	float lensValue;
};