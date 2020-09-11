#pragma once
#include <string>
#include <d3d11.h>   
#include <dxgi.h>     
#include <d3dcompiler.h>
#include <assert.h>

#include "Log.h"
#include "Window.h"

class Shader
{
	D3D11_INPUT_ELEMENT_DESC DEFAULT_INPUT_LAYOUTd [5]
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,	 D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},		// 0 + 12 b = 12 b		// D3D11_APPEND_ALIGNED_ELEMENT
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0,	D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},		// 12 + 8 b = 20 b		
		{"NORMAL",	 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,	D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},		// 20 + 12 b = 32b
		{"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT,  0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},			// 32 + 12 b = 44b
		
		{"SV_InstanceID",   0, DXGI_FORMAT_R32_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	D3D11_INPUT_ELEMENT_DESC SKELETON_INPUT_LAYOUTd[6] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,	 D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},		// 0 + 12 b = 12 b		// D3D11_APPEND_ALIGNED_ELEMENT
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0,	D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},		// 12 + 8 b = 20 b		
		{"NORMAL",	 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,	D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},		// 20 + 12 b = 32b
		{"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT,  0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},		// 32 + 12 b = 44b
		{"BONEID", 0, DXGI_FORMAT_R32G32B32_FLOAT,  0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"SKINWEIGHT", 0, DXGI_FORMAT_R32G32B32_FLOAT,  0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};



public:
	Shader();
	virtual ~Shader();

	void SetPixelShader(LPCWSTR path, LPCSTR entry = "main");
	void SetVertexShader(LPCWSTR path, LPCSTR entry = "main");
	void SetSkeletonVertexShader(LPCWSTR path, LPCSTR entry = "main");
	void SetInputLayoutStructure(size_t arraySize, D3D11_INPUT_ELEMENT_DESC* inputLayoutDesc);
	void SetSkeletonInputLayoutStructure(size_t arraySize, D3D11_INPUT_ELEMENT_DESC* inputLayoutDesc);

	void Compile(ID3D11Device*);
	void BindToContext(ID3D11DeviceContext*);
	void BindSkeletonToContext(ID3D11DeviceContext*);

	ID3D11PixelShader* GetPixelShader() const { return this->pixelShader; }
	ID3D11VertexShader* GetVertexShader() const { return this->vertexShader; }
	ID3D11InputLayout* GetInputLayout() const { return this->inputLayout; }
	ID3D11VertexShader* GetSkeletonVertexShader() const { return this->skeletonVertexShader; }
	ID3D11InputLayout* GetSkeletonInputLayout() const { return this->skeletonInputLayout; }

private:
	bool hasPixelShader, hasVertexShader;
	LPCWSTR pixelPath, vertexPath, skeletonVertexPath;
	LPCSTR pixelEntry, vertexEntry, skeletonVertexEntry;
	DWORD shaderCompilationFlag;

	D3D11_INPUT_ELEMENT_DESC* inputLayoutDescription;
	size_t arraySize;

	D3D11_INPUT_ELEMENT_DESC* inputLayoutSkeletonDescription;
	size_t arraySizeSkeleton;

	// SHADER POINTERS
	ID3D11PixelShader* pixelShader;
	ID3D11VertexShader* vertexShader;
	ID3D11InputLayout* inputLayout;
	ID3D11InputLayout* skeletonInputLayout;
	ID3D11VertexShader* skeletonVertexShader;

};