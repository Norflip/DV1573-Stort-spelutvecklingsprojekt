#pragma once
#include <d3d11_1.h>
#include <dxgi.h> 
#include <d3dcompiler.h>
#include <assert.h>
#include <unordered_map>
#include <DirectXMath.h>
#pragma comment(lib, "gdi32")
#pragma comment(lib, "d3d11") 
#pragma comment( lib, "dxgi" )   
#pragma comment(lib, "d3dcompiler.lib")

#include "Window.h"

enum class ShaderBindFlag
{
	NONE = 0,
	VERTEX = 1 << 0,
	PIXEL = 1 << 1,
	GEOMETRY = 1 << 2,
	HULL = 1 << 3,
	DOMAINS = 1 << 4
};

DEFINE_ENUM_FLAG_OPERATORS(ShaderBindFlag);

struct RenderTexture
{
	size_t width, height;
	ID3D11RenderTargetView* rtv;
	ID3D11ShaderResourceView* srv;
	ID3D11DepthStencilView* dsv;
	ID3D11ShaderResourceView* depthSRV;
	D3D11_VIEWPORT viewport;

	RenderTexture() : rtv(nullptr), srv(nullptr), dsv(nullptr), width(-1), height(-1) {}
	void Release () 
	{
		if (rtv) rtv->Release();
		if (srv) srv->Release();
		if (dsv) dsv->Release();
	}
};







namespace DXHelper
{
	static std::unordered_map<int, ID3D11SamplerState*> m_samplerCache;

	

	
	void CreateSwapchain(const Window& window, ID3D11Device** device, ID3D11DeviceContext** context, IDXGISwapChain** swapchain);
	void CreateConstBuffer(ID3D11Device* device, ID3D11Buffer** buffer, void* initdata, unsigned int byteSize);
	void BindConstBuffer(ID3D11DeviceContext* context, ID3D11Buffer* buffer, void* data, size_t slot, ShaderBindFlag flag);

	D3D11_MAPPED_SUBRESOURCE BindInstanceBuffer(ID3D11DeviceContext* context, ID3D11Buffer* buffer);

	void UnBindInstanceBuffer(ID3D11DeviceContext* context, ID3D11Buffer* buffer);

	RenderTexture CreateBackbuffer(size_t width, size_t height, ID3D11Device* device, IDXGISwapChain* swapchain);
	RenderTexture CreateRenderTexture(size_t width, size_t height, ID3D11Device* device, ID3D11DeviceContext* context, ID3D11DepthStencilState** dss);
	void CreateBlendState(ID3D11Device* device, ID3D11BlendState** blendOn, ID3D11BlendState** blendOff);
	void CreateRSState(ID3D11Device* device, ID3D11RasterizerState** cullBack, ID3D11RasterizerState** cullNone, ID3D11RasterizerState** CCWO); //can add more in the future.

	//void CreateBackbuffer(size_t width, size_t height, ID3D11Device* device, IDXGISwapChain* swapchain, ID3D11RenderTargetView** backbuffer, ID3D11DepthStencilView** depthStencilView);

	void CreateVertexBuffer(ID3D11Device* device, size_t verticeCount, size_t vertexSize, void* vertices, ID3D11Buffer** vertexBuffer);
	void CreateIndexBuffer(ID3D11Device* device, size_t indexCount, unsigned int* indicies, ID3D11Buffer** indexBuffer);
	void CreateInstanceBuffer(ID3D11Device* device, size_t instanceCount, size_t instanceDataSize, void* instanceData, ID3D11Buffer** instanceBuffer);


	ID3D11ShaderResourceView* CreateTexture(unsigned char* buffer, size_t width, size_t height, size_t channels, DXGI_FORMAT format, ID3D11Device* device);
	ID3D11SamplerState* CreateSampler(D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE mode, ID3D11Device* device);

	//Structured buffer
	//stride is sizeof struct, width is all it contains.
	template<typename T>
	void CreateStructuredBuffer(ID3D11Device* device, ID3D11Buffer** buffer, std::vector<T>& data, unsigned int byteStride, unsigned int arraySize,
		ID3D11ShaderResourceView** srv);
	template<typename T>
	void BindStructuredBuffer(ID3D11DeviceContext* context, ID3D11Buffer* buffer, std::vector<T>& data, size_t slot, ShaderBindFlag flag, ID3D11ShaderResourceView** srv);

	
	

	template<typename T>
	void CreateStructuredBuffer(ID3D11Device* device, ID3D11Buffer** buffer, std::vector<T>& data, unsigned int byteStride, unsigned int arraySize, ID3D11ShaderResourceView** srv)
	{


		D3D11_BUFFER_DESC sBufferDesc = {};
		D3D11_SUBRESOURCE_DATA sBufferSub = {};

		sBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		sBufferDesc.ByteWidth = byteStride * arraySize; //sizeofStruct*nrOfElements
		sBufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		sBufferDesc.CPUAccessFlags = 0;// 0; //D3D11_CPU_ACCESS_WRITE
		sBufferDesc.StructureByteStride = byteStride; //sizeofStruct
		sBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;


		sBufferSub.pSysMem = data.data();



		HRESULT hr = device->CreateBuffer(&sBufferDesc, &sBufferSub, buffer);
		assert(SUCCEEDED(hr));

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
		srvDesc.BufferEx.FirstElement = 0;
		srvDesc.BufferEx.Flags = 0;
		srvDesc.BufferEx.NumElements = arraySize;
		hr = device->CreateShaderResourceView(*buffer, &srvDesc, srv);

		assert(SUCCEEDED(hr));


	}

	template<typename T>
	void BindStructuredBuffer(ID3D11DeviceContext* context, ID3D11Buffer* buffer, std::vector<T>& data, size_t slot, ShaderBindFlag flag, ID3D11ShaderResourceView** srv)
	{
		assert(buffer != 0);

		context->UpdateSubresource(buffer, 0, 0, data.data(), 0, 0);


		int bflag = static_cast<int>(flag);

		if ((bflag & (int)ShaderBindFlag::PIXEL) != 0)
			context->PSSetShaderResources(slot, 1, srv);

		if ((bflag & (int)ShaderBindFlag::VERTEX) != 0)
			context->VSSetShaderResources(slot, 1, srv);

		if ((bflag & (int)ShaderBindFlag::HULL) != 0)
			context->HSSetShaderResources(slot, 1, srv);

		if ((bflag & (int)ShaderBindFlag::DOMAINS) != 0)
			context->DSSetShaderResources(slot, 1, srv);

		if ((bflag & (int)ShaderBindFlag::GEOMETRY) != 0)
			context->GSSetShaderResources(slot, 1, srv);

		
	}
	ID3D11RasterizerState* CreateRasterizerState(D3D11_CULL_MODE cullMode, D3D11_FILL_MODE fillMode, ID3D11Device* device);

}