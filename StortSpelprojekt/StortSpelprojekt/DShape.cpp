#include "stdafx.h"
#include "DShape.h"

DShape::~DShape()
{
	wireOnRS->Release();
	wireOffRS->Release();

	pixelShader->Release();
	vertexShader->Release();
	inputLayout->Release();

	buffer->Release();
	lineVBuffer->Release();
}

void DShape::DrawLine(dx::XMFLOAT3 a, dx::XMFLOAT3 b, dx::XMFLOAT3 color)
{
	DShape::Instance().AddShape(Shape(a, b, color, Type::LINE));
}

void DShape::DrawBox(dx::XMFLOAT3 center, dx::XMFLOAT3 extends, dx::XMFLOAT3 color)
{
	DShape::Instance().AddShape(Shape(center, extends, color, Type::BOX));
}

void DShape::DrawWireBox(dx::XMFLOAT3 center, dx::XMFLOAT3 extends, dx::XMFLOAT3 color)
{
	DShape::Instance().AddShape(Shape(center, extends, color, Type::BOX_WIRE));
}

void DShape::DrawSphere(dx::XMFLOAT3 center, float radius, dx::XMFLOAT3 color)
{
	DShape::Instance().AddShape(Shape(center, { radius, radius, radius }, color, Type::SPHERE));
}

void DShape::DrawWireSphere(dx::XMFLOAT3 center, float radius, dx::XMFLOAT3 color)
{
	DShape::Instance().AddShape(Shape(center, { radius, radius, radius }, color, Type::SPHERE_WIRE));
}

void DShape::m_Initialize(ID3D11Device* device)
{
	wireOnRS = DXHelper::CreateRasterizerState(D3D11_CULL_BACK, D3D11_FILL_WIREFRAME, device);
	wireOffRS = DXHelper::CreateRasterizerState(D3D11_CULL_BACK, D3D11_FILL_SOLID, device);
	DXHelper::CreateConstBuffer(device, &buffer, &data, sizeof(ShapeData));

	std::vector<Mesh::Vertex> boxVertexes;
	for (size_t i = 0; i < cube_vertexes.size(); i++)
	{
		Mesh::Vertex v;
		v.position = cube_vertexes[i];
		boxVertexes.push_back(v);
	}

	boxMesh = new Mesh(boxVertexes, cube_indices);
	boxMesh->Initialize(device);

	std::vector<Mesh::Vertex> sphereVertexes;
	for (size_t i = 0; i < sphere_vertexes.size(); i++)
	{
		Mesh::Vertex v;
		v.position = sphere_vertexes[i];
		sphereVertexes.push_back(v);
	}
	sphereMesh = new Mesh(sphereVertexes, sphere_indices);
	sphereMesh->Initialize(device);

	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.ByteWidth = sizeof(LineVertex) * MAX_LINES;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	device->CreateBuffer(&desc, nullptr, &lineVBuffer);
	CompileShaders(device);
}

void DShape::m_Draw(ID3D11DeviceContext* context)
{
	if (shapeCount > 0)
	{
		CameraComponent* camera = CameraComponent::mainCamera;

		if (camera == nullptr)
		{
#if _DEBUG
			std::cout << "Missing main camera" << std::endl;
#endif
			return;
		}

		context->IASetInputLayout(inputLayout);
		context->VSSetShader(vertexShader, 0, 0);
		context->PSSetShader(pixelShader, 0, 0);

		ID3D11RasterizerState* defaultRasterizerState;
		context->RSGetState(&defaultRasterizerState);

		// SOLID
		context->RSSetState(wireOffRS);
		DrawLines(camera, context);
		lines.clear();

		RenderQueue(Type::BOX, boxMesh, camera, context);
		RenderQueue(Type::SPHERE, sphereMesh, camera, context);

		//WIRE
		context->RSSetState(wireOnRS);
		RenderQueue(Type::BOX_WIRE, boxMesh, camera, context);
		RenderQueue(Type::SPHERE_WIRE, sphereMesh, camera, context);

		shapeCount = 0;
		context->RSSetState(defaultRasterizerState);
	}
}

void DShape::AddShape(const Shape& shape)
{
	shapeCount++;

	if (shape.type == Type::LINE)
		lines.push_back(shape);
	else
		queues[(int)(shape.type)].push(shape);
}

void DShape::CompileShaders(ID3D11Device* device)
{
	ID3DBlob* psBlob, * vsBlob, * errorBlob;

	DWORD shaderCompilationFlag = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
	shaderCompilationFlag = shaderCompilationFlag | D3DCOMPILE_DEBUG;
#endif

	std::string psstr =
		"struct PSInput { float4 position : SV_POSITION; float4 color : COLOR; } ; \
	float4 main(PSInput input) : SV_TARGET \
	{ \
		float4 c = input.color; \
		return float4(c.r, c.g, c.b, 1.0f); \
	}";

	HRESULT psResult0 = D3DCompile(psstr.c_str(), psstr.length(), nullptr, nullptr, nullptr, "main", "ps_5_0", D3DCOMPILE_ENABLE_STRICTNESS, 0, &psBlob, &errorBlob);
	assert(SUCCEEDED(psResult0));

#ifdef _DEBUG
	if (FAILED(psResult0) && errorBlob)
	{
		OutputDebugStringA((char*)errorBlob->GetBufferPointer());
		errorBlob->Release();
	}
#endif

	HRESULT psResult1 = device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &this->pixelShader);
	assert(SUCCEEDED(psResult1));


	std::string vsstr =
		"cbuffer cb_ShapeData : register (b0){row_major matrix mvp; float4 color; int isLine; float3 pad;}; \
	struct VSInput { float4 position : SV_POSITION; float4 color : COLOR; };\
	struct VSOutput { float4 position : SV_POSITION; float4 color : COLOR;};\
	\
	VSOutput main(VSInput input) { \
		VSOutput output;\
		output.position = mul(mvp, input.position); \
		output.color = lerp(color, input.color, isLine); \
		return output; \
	}";


	//HRESULT	VSCompileResult = D3DCompileFromFile(L"Shaders/Shapes_vs.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "vs_5_0", shaderCompilationFlag, 0, &vsBlob, &errorBlob);
	HRESULT vsResult0 = D3DCompile(vsstr.c_str(), vsstr.length(), nullptr, nullptr, nullptr, "main", "vs_5_0", D3DCOMPILE_ENABLE_STRICTNESS, 0, &vsBlob, &errorBlob);
	assert(SUCCEEDED(vsResult0));

#ifdef _DEBUG
	if (FAILED(psResult1) && errorBlob)
	{
		OutputDebugStringA((char*)errorBlob->GetBufferPointer());
		errorBlob->Release();
	}
#endif

	HRESULT vsResult1 = device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &this->vertexShader);
	assert(SUCCEEDED(vsResult1));

	D3D11_INPUT_ELEMENT_DESC DEFAULT_INPUT_LAYOUTd[]{
		{"SV_POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,	 D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	HRESULT createIAResult = device->CreateInputLayout(DEFAULT_INPUT_LAYOUTd, ARRAYSIZE(DEFAULT_INPUT_LAYOUTd), vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &this->inputLayout);
	assert(SUCCEEDED(createIAResult));
}

void DShape::RenderQueue(Type type, const Mesh* mesh, const CameraComponent* camera, ID3D11DeviceContext* context)
{
	std::queue<Shape>& queue = queues[(int)type];
	while (!queue.empty())
	{
		DrawMesh(mesh, queue.front(), camera, context);
		queue.pop();
	}
}

void DShape::DrawMesh(const Mesh* mesh, const Shape& shape, const CameraComponent* camera, ID3D11DeviceContext* context)
{
	dx::XMMATRIX model = dx::XMMatrixScalingFromVector(dx::XMLoadFloat3(&shape.t1)) * dx::XMMatrixTranslationFromVector(dx::XMLoadFloat3(&shape.t0));

	dx::XMMATRIX mvp = dx::XMMatrixMultiply(model, camera->GetViewMatrix() * camera->GetProjectionMatrix());
	dx::XMStoreFloat4x4(&data.mvp, dx::XMMatrixTranspose(mvp));
	data.color = dx::XMFLOAT4(shape.color.x, shape.color.y, shape.color.z, 1.0f);
	data.isLine = FALSE;

	DXHelper::BindConstBuffer(context, buffer, &data, 0, ShaderBindFlag::VERTEX);

	UINT stride = sizeof(Mesh::Vertex);
	UINT offset = 0;
	ID3D11Buffer* vertexBuffer = mesh->GetVertexBuffer();
	context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	context->IASetIndexBuffer(mesh->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);
	context->IASetPrimitiveTopology(mesh->GetTopology());

	context->DrawIndexed(mesh->GetIndexCount(), 0, 0);
}

void DShape::DrawLines(const CameraComponent* camera, ID3D11DeviceContext* context)
{
	const size_t verts = lines.size() * 2;
	LineVertex* vertices = new LineVertex[verts];

	for (size_t i = 0; i < lines.size(); i++)
	{
		const Shape& shape = lines[i];
		LineVertex lv0, lv1;
		lv0.position = shape.t0;
		lv1.position = shape.t1;
		lv0.color = lv1.color = CONVERT_COLOR(shape.color, 1.0f);

		vertices[i * 2 + 0] = lv0;
		vertices[i * 2 + 1] = lv1;
	}

	D3D11_MAPPED_SUBRESOURCE resource;
	context->Map(lineVBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	memcpy(resource.pData, vertices, sizeof(LineVertex) * verts);
	context->Unmap(lineVBuffer, 0);
	delete[] vertices;

	dx::XMMATRIX mvp = dx::XMMatrixIdentity() * camera->GetViewMatrix() * camera->GetProjectionMatrix();
	dx::XMStoreFloat4x4(&data.mvp, dx::XMMatrixTranspose(mvp));
	data.isLine = TRUE;
	data.color = { 1,0,0,1 };

	DXHelper::BindConstBuffer(context, buffer, &data, 0, ShaderBindFlag::VERTEX);

	UINT stride = sizeof(LineVertex);
	UINT offset = 0;

	context->IASetVertexBuffers(0, 1, &lineVBuffer, &stride, &offset);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	context->Draw(verts, 0);
}
