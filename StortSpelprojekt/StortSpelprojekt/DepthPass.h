#pragma once
#include "stdafx.h"
class DepthPass
{
public:
	static ID3D11ShaderResourceView** GetDepthSRV() { return &depthSRV; }
	//initialize resources
	static void Init(ID3D11Device*, UINT width, UINT height);
	//bind depth stencil and set rtv to 0
	static void BindDSV(ID3D11DeviceContext*);
	static void BindNull(ID3D11DeviceContext*);
private:
	static ID3D11DepthStencilView* depthDSV;
	static ID3D11ShaderResourceView* depthSRV;
};

