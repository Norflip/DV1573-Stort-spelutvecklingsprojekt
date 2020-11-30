#pragma once
#include "stdafx.h"
class DepthPass
{
public:
	ID3D11ShaderResourceView** GetRandomSRV() { return &randomSRV; }
	ID3D11ShaderResourceView** GetDepthSRV() { return &depthSRV; }
	//initialize resources
	void Init(ID3D11Device*, UINT width, UINT height);
	//bind depth stencil and set rtv to 0
	void BindDSV(ID3D11DeviceContext*);
	void BindNull(ID3D11DeviceContext*);
private:
	ID3D11DepthStencilView* depthDSV;
	ID3D11ShaderResourceView* depthSRV;
	ID3D11ShaderResourceView* randomSRV;
};

