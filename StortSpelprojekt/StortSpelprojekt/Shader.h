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
	D3D11_INPUT_ELEMENT_DESC DEFAULT_INPUT_LAYOUTd[4]
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,	 D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},		// 0 + 12 b = 12 b		// D3D11_APPEND_ALIGNED_ELEMENT
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0,	D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},		// 12 + 8 b = 20 b		
		{"NORMAL",	 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,	D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},		// 20 + 12 b = 32b
		{"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT,  0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},			// 32 + 12 b = 44b
	};

public:
	Shader();
	virtual ~Shader();

	void SetPixelShader(LPCWSTR path, LPCSTR entry = "main");
	void SetVertexShader(LPCWSTR path, LPCSTR entry = "main");
	void SetInputLayoutStructure(size_t arraySize, D3D11_INPUT_ELEMENT_DESC* inputLayoutDesc);

	void Compile(ID3D11Device*);
	void BindToContext(ID3D11DeviceContext*);

	ID3D11PixelShader* GetPixelShader() const { return this->pixelShader; }
	ID3D11VertexShader* GetVertexShader() const { return this->vertexShader; }
	ID3D11InputLayout* GetInputLayout() const { return this->inputLayout; }

private:
	bool hasPixelShader, hasVertexShader;
	LPCWSTR pixelPath, vertexPath;
	LPCSTR pixelEntry, vertexEntry;
	DWORD shaderCompilationFlag;

	D3D11_INPUT_ELEMENT_DESC* inputLayoutDescription;
	size_t arraySize;

	// SHADER POINTERS
	ID3D11PixelShader* pixelShader;
	ID3D11VertexShader* vertexShader;
	ID3D11InputLayout* inputLayout;
};