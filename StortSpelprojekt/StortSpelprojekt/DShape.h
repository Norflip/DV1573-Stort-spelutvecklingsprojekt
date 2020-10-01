#pragma once
#include <DirectXMath.h>
#include <queue>

#include "DXHelper.h"
#include "Mesh.h"
#include "CameraComponent.h"
#include "ShittyOBJLoader.h"

namespace dx = DirectX;

#define SHAPES_DEFAULT_COLOR dx::XMFLOAT3(1.0f,1.0f,1.0f)
#define TYPE_COUNT 5
#define MAX_LINES 256
#define CONVERT_COLOR(color,alpha) dx::XMFLOAT4(color.x, color.y, color.z, alpha)

class DShape
{
	__declspec(align(16))
	struct ShapeData
	{
		dx::XMFLOAT4X4 mvp;
		dx::XMFLOAT4 color;
		int isLine;
	};

	struct LineVertex
	{
		dx::XMFLOAT3 position;
		dx::XMFLOAT4 color;
	};

	enum Type { LINE, BOX, SPHERE, BOX_WIRE, SPHERE_WIRE, LINE_THICC };
	struct Shape
	{
		dx::XMFLOAT3 t0, t1;
		dx::XMFLOAT3 color;
		Type type;
		Shape(dx::XMFLOAT3 t0, dx::XMFLOAT3 t1, dx::XMFLOAT3 c, Type type) : t0(t0), t1(t1), color(c), type(type) {}
	};

public:
	DShape() {};
	virtual ~DShape() {};

	static void DrawLine(dx::XMFLOAT3 a, dx::XMFLOAT3 b, dx::XMFLOAT3 color = SHAPES_DEFAULT_COLOR);

	static void DrawBox(dx::XMFLOAT3 center, dx::XMFLOAT3 extends, dx::XMFLOAT3 color = SHAPES_DEFAULT_COLOR);
	static void DrawWireBox(dx::XMFLOAT3 center, dx::XMFLOAT3 extends, dx::XMFLOAT3 color = SHAPES_DEFAULT_COLOR);

	static void DrawSphere(dx::XMFLOAT3 center, float radius, dx::XMFLOAT3 color = SHAPES_DEFAULT_COLOR);
	static void DrawWireSphere(dx::XMFLOAT3 center, float radius, dx::XMFLOAT3 color = SHAPES_DEFAULT_COLOR);

	void m_Initialize(ID3D11Device* device);
	void m_Draw(ID3D11DeviceContext* context);

	static DShape& Instance() // singleton
	{
		static DShape instance;
		return instance;
	}

	DShape(DShape const&) = delete;
	void operator=(DShape const&) = delete;

private:
	void AddShape(const Shape& shape);
	void CompileShaders(ID3D11Device* device);
	void RenderQueue(Type type, const Mesh& mesh, const CameraComponent* camera, ID3D11DeviceContext* context);
	void DrawMesh(const Mesh& mesh, const Shape& shape, const CameraComponent* camera, ID3D11DeviceContext* context);
	void DrawLines(const CameraComponent* camera, ID3D11DeviceContext* context);

	Mesh boxMesh;
	Mesh sphereMesh;

	ID3D11RasterizerState* wireOnRS;
	ID3D11RasterizerState* wireOffRS;

	ID3D11PixelShader* pixelShader;
	ID3D11VertexShader* vertexShader;
	ID3D11InputLayout* inputLayout;

	ShapeData data;
	ID3D11Buffer* buffer;

	ID3D11Buffer* lineVBuffer;

	size_t shapeCount;
	std::queue<Shape> queues[TYPE_COUNT];
	std::vector<Shape> lines;
};