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

	DXHelper::CreateConstBuffer(device, &cbuffer, &data, sizeof(T));
}

template<typename T>
inline void ConstantBuffer<T>::UpdateBuffer(ID3D11DeviceContext* context)
{
	assert(initialized);
	DXHelper::BindConstBuffer(context, cbuffer, &data, slot, bindFlag);
}
