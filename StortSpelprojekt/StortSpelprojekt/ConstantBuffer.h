#pragma once
#include "DXHandler.h"

template <typename T>
class ConstantBuffer
{
public:
	ConstantBuffer();
	virtual ~ConstantBuffer();

	void Initialize(size_t slot, ID3D11Device* device);
	void Bind(ID3D11DeviceContext* context);
	void Release();

	T* GetData() { return &this->data; }

private:
	ID3D11Buffer* buffer;
	size_t slot;
	T data;
};

template<typename T>
inline ConstantBuffer<T>::ConstantBuffer() {}

template<typename T>
inline ConstantBuffer<T>::~ConstantBuffer() {}

template<typename T>
inline void ConstantBuffer<T>::Initialize(size_t slot, ID3D11Device* device)
{
	this->data = T();
	this->slot = slot;

	static_assert(sizeof(data) % 16 == 0, "struct has the wrong size");
	ZeroMemory(&data, sizeof(T));
	ZeroMemory(&buffer, sizeof(ID3D11Buffer));

	D3D11_BUFFER_DESC bufferDescription;
	ZeroMemory(&bufferDescription, sizeof(D3D11_BUFFER_DESC));
	bufferDescription.Usage = D3D11_USAGE_DEFAULT;
	bufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	D3D11_SUBRESOURCE_DATA subresourceData;
	ZeroMemory(&subresourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	bufferDescription.ByteWidth = sizeof(data);
	subresourceData.pSysMem = &data;
	
	HRESULT createdBufferResult = device->CreateBuffer(&bufferDescription, &subresourceData, &buffer);
	assert(SUCCEEDED(createdBufferResult));
}

template<typename T>
inline void ConstantBuffer<T>::Bind(ID3D11DeviceContext* context)
{
	context->UpdateSubresource(buffer, 0, 0, &data, 0, 0);
	context->VSSetConstantBuffers(slot, 1, &buffer);
}

template<typename T>
inline void ConstantBuffer<T>::Release()
{
	this->buffer->Release();
	this->buffer = nullptr;
}
