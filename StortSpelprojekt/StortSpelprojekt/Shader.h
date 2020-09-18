#pragma once
#include <string>
#include <d3d11.h>   
#include <dxgi.h>     
#include <d3dcompiler.h>
#include <assert.h>

#include "Log.h"
#include "Window.h"
#include "DXHelper.h"

class Shader
{
	D3D11_INPUT_ELEMENT_DESC DEFAULT_INPUT_LAYOUTd [8]
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,	 D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},		// 0 + 12 b = 12 b		// D3D11_APPEND_ALIGNED_ELEMENT
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0,	D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},		// 12 + 8 b = 20 b		
		{"NORMAL",	 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,	D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},		// 20 + 12 b = 32b
		{"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT,  0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},		// 32 + 12 b = 44b
		{"BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT,  0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},		// 44 + 12 b = 56b
		{"BONEID", 0, DXGI_FORMAT_R32G32B32_UINT,  0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},		// 56 + 12 b = 68b
		{"SKINWEIGHT", 0, DXGI_FORMAT_R32G32B32_FLOAT,  0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},	// 68 + 12 b = 80b
		{"SV_InstanceID",   0, DXGI_FORMAT_R32_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }		
	};
	
public:
	Shader();
	virtual ~Shader();

	void SetPixelShader(LPCWSTR path, LPCSTR entry = "main"); 
	void SetVertexShader(LPCWSTR path, LPCSTR entry = "main");
	void SetGeometryShader(LPCWSTR path, LPCSTR entry = "main");
	

	void SetInputLayoutStructure(size_t arraySize, D3D11_INPUT_ELEMENT_DESC* inputLayoutDesc);

	void Compile(ID3D11Device*);
	void BindToContext(ID3D11DeviceContext*);

	void CompilePS(ID3D11Device*);
	void CompileVS(ID3D11Device*);
	void CompileGS(ID3D11Device*);
	void CompileSkeletonVS(ID3D11Device*);

private:
	LPCWSTR pixelPath, vertexPath, geometryPath, skeletonPath;
	LPCSTR pixelEntry, vertexEntry, geometryEntry, skeletonEntry;

	DWORD shaderCompilationFlag;
	ShaderBindFlag shaderFlags;

	D3D11_INPUT_ELEMENT_DESC* inputLayoutDescription;
	size_t ilArrayCount;

	// SHADER POINTERS
	ID3D11VertexShader* vertexShader;
	ID3D11InputLayout* inputLayout;
	ID3D11PixelShader* pixelShader;
	ID3D11GeometryShader* geometryShader;
	ID3D11VertexShader* skeletonShader;
};