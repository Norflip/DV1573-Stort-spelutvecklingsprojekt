#pragma once
#include <DirectXMath.h>
#include <queue>

#include "DXHelper.h"
#include "Mesh.h"
#include "CameraComponent.h"
#include "ConstantBuffer.h"

namespace dx = DirectX;

#define SHAPES_DEFAULT_COLOR dx::XMFLOAT3(1.0f,1.0f,1.0f)
#define TYPE_COUNT 5
#define MAX_LINES 256
#define CONVERT_COLOR(color,alpha) dx::XMFLOAT4(color.x, color.y, color.z, alpha)

class DShape
{
	// AUTO GENERATED
	std::vector<unsigned int> sphere_indices = { 0,13,12,1,13,15,0,12,17,0,17,19,0,19,16,1,15,22,2,14,24,3,18,26,4,20,28,5,21,30,1,22,25,2,24,27,3,26,29,4,28,31,5,30,23,6,32,37,7,33,39,8,34,40,9,35,41,10,36,38,38,41,11,38,36,41,36,9,41,41,40,11,41,35,40,35,8,40,40,39,11,40,34,39,34,7,39,39,37,11,39,33,37,33,6,37,37,38,11,37,32,38,32,10,38,23,36,10,23,30,36,30,9,36,31,35,9,31,28,35,28,8,35,29,34,8,29,26,34,26,7,34,27,33,7,27,24,33,24,6,33,25,32,6,25,22,32,22,10,32,30,31,9,30,21,31,21,4,31,28,29,8,28,20,29,20,3,29,26,27,7,26,18,27,18,2,27,24,25,6,24,14,25,14,1,25,22,23,10,22,15,23,15,5,23,16,21,5,16,19,21,19,4,21,19,20,4,19,17,20,17,3,20,17,18,3,17,12,18,12,2,18,15,16,5,15,13,16,13,0,16,12,14,2,12,13,14,13,1,14 };
	std::vector<dx::XMFLOAT3> sphere_vertexes = { dx::XMFLOAT3(0.000000f, -1.000000f, 0.000000f),dx::XMFLOAT3(0.723607f, -0.447220f, 0.525725f),dx::XMFLOAT3(-0.276388f, -0.447220f, 0.850649f),dx::XMFLOAT3(-0.894426f, -0.447216f, 0.000000f),dx::XMFLOAT3(-0.276388f, -0.447220f, -0.850649f),dx::XMFLOAT3(0.723607f, -0.447220f, -0.525725f),dx::XMFLOAT3(0.276388f, 0.447220f, 0.850649f),dx::XMFLOAT3(-0.723607f, 0.447220f, 0.525725f),dx::XMFLOAT3(-0.723607f, 0.447220f, -0.525725f),dx::XMFLOAT3(0.276388f, 0.447220f, -0.850649f),dx::XMFLOAT3(0.894426f, 0.447216f, 0.000000f),dx::XMFLOAT3(0.000000f, 1.000000f, 0.000000f),dx::XMFLOAT3(-0.162456f, -0.850654f, 0.499995f),dx::XMFLOAT3(0.425323f, -0.850654f, 0.309011f),dx::XMFLOAT3(0.262869f, -0.525738f, 0.809012f),dx::XMFLOAT3(0.850648f, -0.525736f, 0.000000f),dx::XMFLOAT3(0.425323f, -0.850654f, -0.309011f),dx::XMFLOAT3(-0.525730f, -0.850652f, 0.000000f),dx::XMFLOAT3(-0.688189f, -0.525736f, 0.499997f),dx::XMFLOAT3(-0.162456f, -0.850654f, -0.499995f),dx::XMFLOAT3(-0.688189f, -0.525736f, -0.499997f),dx::XMFLOAT3(0.262869f, -0.525738f, -0.809012f),dx::XMFLOAT3(0.951058f, 0.000000f, 0.309013f),dx::XMFLOAT3(0.951058f, 0.000000f, -0.309013f),dx::XMFLOAT3(0.000000f, 0.000000f, 1.000000f),dx::XMFLOAT3(0.587786f, 0.000000f, 0.809017f),dx::XMFLOAT3(-0.951058f, 0.000000f, 0.309013f),dx::XMFLOAT3(-0.587786f, 0.000000f, 0.809017f),dx::XMFLOAT3(-0.587786f, 0.000000f, -0.809017f),dx::XMFLOAT3(-0.951058f, 0.000000f, -0.309013f),dx::XMFLOAT3(0.587786f, 0.000000f, -0.809017f),dx::XMFLOAT3(0.000000f, 0.000000f, -1.000000f),dx::XMFLOAT3(0.688189f, 0.525736f, 0.499997f),dx::XMFLOAT3(-0.262869f, 0.525738f, 0.809012f),dx::XMFLOAT3(-0.850648f, 0.525736f, 0.000000f),dx::XMFLOAT3(-0.262869f, 0.525738f, -0.809012f),dx::XMFLOAT3(0.688189f, 0.525736f, -0.499997f),dx::XMFLOAT3(0.162456f, 0.850654f, 0.499995f),dx::XMFLOAT3(0.525730f, 0.850652f, 0.000000f),dx::XMFLOAT3(-0.425323f, 0.850654f, 0.309011f),dx::XMFLOAT3(-0.425323f, 0.850654f, -0.309011f),dx::XMFLOAT3(0.162456f, 0.850654f, -0.499995f) };
	std::vector<unsigned int> cube_indices = { 0,1,2,2,1,3,2,3,4,4,3,5,4,5,6,6,5,7,6,7,0,0,7,1,1,7,3,3,7,5,6,0,4,4,0,2,8,9,10,10,9,11,10,11,12,12,11,13,12,13,14,14,13,15,14,15,8,8,15,9,9,15,11,11,15,13,14,8,12,12,8,10 };
	std::vector<dx::XMFLOAT3> cube_vertexes = { dx::XMFLOAT3(-0.500000f, -0.500000f, 0.500000f),dx::XMFLOAT3(0.500000f, -0.500000f, 0.500000f),dx::XMFLOAT3(-0.500000f, 0.500000f, 0.500000f),dx::XMFLOAT3(0.500000f, 0.500000f, 0.500000f),dx::XMFLOAT3(-0.500000f, 0.500000f, -0.500000f),dx::XMFLOAT3(0.500000f, 0.500000f, -0.500000f),dx::XMFLOAT3(-0.500000f, -0.500000f, -0.500000f),dx::XMFLOAT3(0.500000f, -0.500000f, -0.500000f),dx::XMFLOAT3(-0.500000f, -0.500000f, 0.500000f),dx::XMFLOAT3(0.500000f, -0.500000f, 0.500000f),dx::XMFLOAT3(-0.500000f, 0.500000f, 0.500000f),dx::XMFLOAT3(0.500000f, 0.500000f, 0.500000f),dx::XMFLOAT3(-0.500000f, 0.500000f, -0.500000f),dx::XMFLOAT3(0.500000f, 0.500000f, -0.500000f),dx::XMFLOAT3(-0.500000f, -0.500000f, -0.500000f),dx::XMFLOAT3(0.500000f, -0.500000f, -0.500000f) };

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
	virtual ~DShape();

	static void DrawLine(dx::XMFLOAT3 a, dx::XMFLOAT3 b, dx::XMFLOAT3 color = SHAPES_DEFAULT_COLOR);

	static void DrawBox(dx::XMFLOAT3 center, dx::XMFLOAT3 extends, dx::XMFLOAT3 color = SHAPES_DEFAULT_COLOR);
	static void DrawWireBox(dx::XMFLOAT3 center, dx::XMFLOAT3 extends, dx::XMFLOAT3 color = SHAPES_DEFAULT_COLOR);

	static void DrawSphere(dx::XMFLOAT3 center, float radius, dx::XMFLOAT3 color = SHAPES_DEFAULT_COLOR);
	static void DrawWireSphere(dx::XMFLOAT3 center, float radius, dx::XMFLOAT3 color = SHAPES_DEFAULT_COLOR);

	void m_Initialize(ID3D11Device* device);
	void m_Draw(dx::XMMATRIX cameraVP, ID3D11DeviceContext* context);

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
	void RenderQueue(Type type, const Mesh* mesh, dx::XMMATRIX cameraVP, ID3D11DeviceContext* context);
	void DrawMesh(const Mesh* mesh, const Shape& shape, dx::XMMATRIX cameraVP, ID3D11DeviceContext* context);
	void DrawLines(dx::XMMATRIX cameraVP, ID3D11DeviceContext* context);

	Mesh* boxMesh;
	Mesh* sphereMesh;

	ID3D11RasterizerState* wireOnRS;
	ID3D11RasterizerState* wireOffRS;

	ID3D11PixelShader* pixelShader;
	ID3D11VertexShader* vertexShader;
	ID3D11InputLayout* inputLayout;

	ConstantBuffer<ShapeData> buffer;

	ShapeData data;
	ID3D11Buffer* lineVBuffer;

	size_t shapeCount;
	std::queue<Shape> queues[TYPE_COUNT];
	std::vector<Shape> lines;
};