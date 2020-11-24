#pragma once
#include <assert.h>
#include "DXHelper.h"

template<typename T>
class ConstantBuffer
{
public:
	ConstantBuffer();
	virtual ~ConstantBuffer();

	void Initialize(size_t slot, ShaderBindFlag bindFlag, ID3D11Device* device);
	T& GetData() { return this->data; }
	void SetData(T data) { this->data = data; }
	void UpdateBuffer(ID3D11DeviceContext* context);

	ShaderBindFlag GetFlag() const { return this->bindFlag; }
	void SetBindFlag(ShaderBindFlag bindFlag) { this->bindFlag = bindFlag; }

private:
	T data;
	ID3D11Buffer* cbuffer;
	ShaderBindFlag bindFlag;
	size_t slot;
	bool initialized;
};

template<typename T>
inline ConstantBuffer<T>::ConstantBuffer()
{
}

template<typename T>
inline ConstantBuffer<T>::~ConstantBuffer()
{
	this->cbuffer->Release();
}

template<typename T>
inline void ConstantBuffer<T>::Initialize(size_t slot, ShaderBindFlag bindFlag, ID3D11Device* device)
{
	assert((sizeof(T) % 16) == 0);
	this->data = T();

	initialized = true;
	this->slot = slot;
	this->bindFlag = bindFlag;

	D3D11_BUFFER_DESC bufferDescription;
	ZeroMemory(&bufferDescription, sizeof(D3D11_BUFFER_DESC));
	bufferDescription.Usage = D3D11_USAGE_DEFAULT;
	bufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	D3D11_SUBRESOURCE_DATA subresourceData;
	ZeroMemory(&subresourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	bufferDescription.ByteWidth = sizeof(T);
	subresourceData.pSysMem = &data;

	HRESULT createdBufferResult = device->CreateBuffer(&bufferDescription, &subresourceData, &cbuffer);
	assert(SUCCEEDED(createdBufferResult));
}

template<typename T>
inline void ConstantBuffer<T>::UpdateBuffer(ID3D11DeviceContext* context)
{
	assert(initialized);

	context->UpdateSubresource(cbuffer, 0, 0, &data, 0, 0);
	int bflag = static_cast<int>(bindFlag);

	if ((bflag & (int)ShaderBindFlag::PIXEL) != 0)
		context->PSSetConstantBuffers(slot, 1, &cbuffer);

	if ((bflag & (int)ShaderBindFlag::VERTEX) != 0)
		context->VSSetConstantBuffers(slot, 1, &cbuffer);

	if ((bflag & (int)ShaderBindFlag::GEOMETRY) != 0)
		context->GSSetConstantBuffers(slot, 1, &cbuffer);

	if ((bflag & (int)ShaderBindFlag::HULL) != 0)
		context->HSSetConstantBuffers(slot, 1, &cbuffer);

	if ((bflag & (int)ShaderBindFlag::DOMAINS) != 0)
		context->DSSetConstantBuffers(slot, 1, &cbuffer);

	if ((bflag & (int)ShaderBindFlag::COMPUTE) != 0)
		context->CSSetConstantBuffers(slot, 1, &cbuffer);
}
