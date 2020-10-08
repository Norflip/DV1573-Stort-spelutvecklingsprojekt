#include "Shader.h"

Shader::Shader() : pixelShader(nullptr), vertexShader(nullptr), inputLayout(nullptr), geometryShader(nullptr), hullShader(nullptr), domainShader(nullptr)
{
	shaderCompilationFlag = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
	shaderCompilationFlag = shaderCompilationFlag | D3DCOMPILE_DEBUG;
#endif

	SetInputLayoutStructure(6, DEFAULT_INPUT_LAYOUTd);
}

Shader::~Shader()
{
	//if (pixelShader)
	//{
	//	pixelShader->Release();
	//}
	//if (vertexShader)
	//{
	//	vertexShader->Release();
	//}
	//if (inputLayout)
	//{
	//	inputLayout->Release();
	//}
	//if (geometryShader)
	//{
	//	geometryShader->Release();
	//}
	//if (hullShader)
	//{
	//	hullShader->Release();
	//}
	//if (domainShader)
	//{
	//	domainShader->Release();
	//}
}

void Shader::SetPixelShader(std::string path, LPCSTR entry)
{
	this->pixel = path;
	this->pixelEntry = entry;
	shaderFlags |= ShaderBindFlag::PIXEL;
}

void Shader::SetVertexShader(std::string path, LPCSTR entry)
{
	this->vertex = path;
	this->vertexEntry = entry;
	shaderFlags |= ShaderBindFlag::VERTEX;
}

void Shader::SetHullShader(LPCWSTR path, LPCSTR entry)
{
	this->hullPath = path;
	this->hullEntry = entry;
	shaderFlags |= ShaderBindFlag::HULL;
}

void Shader::SetDomainShader(LPCWSTR path, LPCSTR entry)
{
	this->domainPath = path;
	this->domainEntry = entry;
	shaderFlags |= ShaderBindFlag::DOMAINS;
}

void Shader::SetGeometryShader(LPCWSTR path, LPCSTR entry)
{
	this->geometryPath = path;
	this->geometryEntry = entry;
	shaderFlags |= ShaderBindFlag::GEOMETRY;
}

void Shader::SetInputLayoutStructure(size_t arraySize, D3D11_INPUT_ELEMENT_DESC* inputLayoutDesc)
{
	this->inputLayoutDescription = inputLayoutDesc;
	this->ilArrayCount = arraySize;
}

void Shader::Compile(ID3D11Device* device)
{
	CompileVS(device);
	CompilePS(device);
	CompileGS(device);
	CompileHS(device);
	CompileDS(device);
}

void Shader::BindToContext(ID3D11DeviceContext* context) const
{
	int flag = static_cast<int>(shaderFlags);

	if ((flag & (int)ShaderBindFlag::VERTEX) != 0)
	{
		// sets the vertex shader and layout
		context->IASetInputLayout(inputLayout);
		context->VSSetShader(vertexShader, 0, 0);
	}

	if ((flag & (int)ShaderBindFlag::PIXEL) != 0)
	{
		//sets the pixel shader	
		context->PSSetShader(pixelShader, 0, 0);
	}

	if ((flag & (int)ShaderBindFlag::GEOMETRY) != 0)
	{
		context->GSSetShader(geometryShader, 0, 0);
	}

	if ((flag & (int)ShaderBindFlag::HULL) != 0)
	{
		context->HSSetShader(hullShader, 0, 0);
	}
	if ((flag & (int)ShaderBindFlag::DOMAINS) != 0)
	{
		context->DSSetShader(domainShader, 0, 0);
	}
}

void Shader::CompilePS(ID3D11Device* device)
{
	if (((int)shaderFlags & (int)ShaderBindFlag::PIXEL) != 0)
	{
		if (pixelShader != nullptr)
		{
			pixelShader->Release();
			//delete pixelShader;
			//pixelShader = nullptr;
		}

		ID3DBlob* errorBlob = nullptr;
		ID3DBlob* PSBlob = nullptr;
		
		// Convert the string to a wstring locally, without changing the content
		std::wstring wPixel = std::wstring(pixel.begin(), pixel.end());

		HRESULT PSCompileResult = D3DCompileFromFile
		(
			wPixel.c_str(),
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			pixelEntry,
			"ps_5_0",
			shaderCompilationFlag,
			0,
			&PSBlob,
			&errorBlob
		);

		if (FAILED(PSCompileResult) && errorBlob)
		{
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			Log::Add(Log::LogLevel::Error, (char*)errorBlob->GetBufferPointer());
			errorBlob->Release();
		}

		HRESULT PSCreateResult = device->CreatePixelShader(PSBlob->GetBufferPointer(), PSBlob->GetBufferSize(), nullptr, &this->pixelShader);
		assert(SUCCEEDED(PSCreateResult));
	}

}

void Shader::CompileVS(ID3D11Device* device)
{

	if (((int)shaderFlags & (int)ShaderBindFlag::VERTEX) != 0)
	{

		if (vertexShader != nullptr)
		{
			vertexShader->Release();
			//delete vertexShader;
			//vertexShader = nullptr;
		}

		ID3DBlob* errorBlob = nullptr;
		ID3DBlob* VSBlob = nullptr;
		
		// Convert the string to a wstring locally, without changing the content
		std::wstring wVertex = std::wstring(vertex.begin(), vertex.end());

		// VERTEX SHADER
		HRESULT	VSCompileResult = D3DCompileFromFile
		(
			wVertex.c_str(),
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			vertexEntry,
			"vs_5_0",
			shaderCompilationFlag,
			0,
			&VSBlob,
			&errorBlob
		);

		if (FAILED(VSCompileResult) && errorBlob)
		{
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			Log::Add(Log::LogLevel::Error, (char*)errorBlob->GetBufferPointer());
			errorBlob->Release();
		}

		HRESULT VSCreateResult = device->CreateVertexShader(VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), nullptr, &this->vertexShader);
		assert(SUCCEEDED(VSCreateResult));

		HRESULT createInputLayoutResult = device->CreateInputLayout(inputLayoutDescription, ilArrayCount, VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), &inputLayout);
		assert(SUCCEEDED(createInputLayoutResult));
	}
}

void Shader::CompileGS(ID3D11Device* device)
{
	if (((int)shaderFlags & (int)ShaderBindFlag::GEOMETRY) != 0)
	{

		if (geometryShader != nullptr)
		{
			geometryShader->Release();
			//delete geometryShader;
			//geometryShader = nullptr;
		}

		ID3DBlob* errorBlob = nullptr;
		ID3DBlob* GSBlob = nullptr;

		// Convert the string to a wstring locally, without changing the content
		std::wstring wGeometry = std::wstring(geometry.begin(), geometry.end());

		HRESULT GSCompileResult = D3DCompileFromFile
		(
			wGeometry.c_str(),
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			geometryEntry,
			"gs_5_0",
			shaderCompilationFlag,
			0,
			&GSBlob,
			&errorBlob
		);

		if (FAILED(GSCompileResult) && errorBlob)
		{
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			Log::Add(Log::LogLevel::Error, (char*)errorBlob->GetBufferPointer());
			errorBlob->Release();
		}

		HRESULT GSCreateResult = device->CreateGeometryShader(GSBlob->GetBufferPointer(), GSBlob->GetBufferSize(), nullptr, &this->geometryShader);
		assert(SUCCEEDED(GSCreateResult));
	}
}

void Shader::CompileHS(ID3D11Device* device)
{
	if (((int)shaderFlags & (int)ShaderBindFlag::HULL) != 0)
	{

		if (hullShader != nullptr)
		{
			delete hullShader;
			hullShader = nullptr;
		}

		ID3DBlob* errorBlob = nullptr;
		ID3DBlob* HSBlob = nullptr;

		HRESULT HSCompileResult = D3DCompileFromFile
		(
			hullPath,
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			hullEntry,
			"hs_5_0",
			shaderCompilationFlag,
			0,
			&HSBlob,
			&errorBlob
		);

		if (FAILED(HSCompileResult) && errorBlob)
		{
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			Log::Add(Log::LogLevel::Error, (char*)errorBlob->GetBufferPointer());
			errorBlob->Release();
		}

		HRESULT HSCreateResult = device->CreateHullShader(HSBlob->GetBufferPointer(), HSBlob->GetBufferSize(), nullptr, &this->hullShader);
		assert(SUCCEEDED(HSCreateResult));
	}
}

void Shader::CompileDS(ID3D11Device* device)
{

	if (((int)shaderFlags & (int)ShaderBindFlag::DOMAINS) != 0)
	{

		if (domainShader != nullptr)
		{
			delete domainShader;
			domainShader = nullptr;
		}

		ID3DBlob* errorBlob = nullptr;
		ID3DBlob* DSBlob = nullptr;

		HRESULT DSCompileResult = D3DCompileFromFile
		(
			domainPath,
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			domainEntry,
			"ds_5_0",
			shaderCompilationFlag,
			0,
			&DSBlob,
			&errorBlob
		);

		if (FAILED(DSCompileResult) && errorBlob)
		{
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			Log::Add(Log::LogLevel::Error, (char*)errorBlob->GetBufferPointer());
			errorBlob->Release();
		}

		HRESULT DSCreateResult = device->CreateDomainShader(DSBlob->GetBufferPointer(), DSBlob->GetBufferSize(), nullptr, &this->domainShader);
		assert(SUCCEEDED(DSCreateResult));
	}
}

void Shader::Unbind(ID3D11DeviceContext* context) const
{
	

	int flag = static_cast<int>(shaderFlags);

	

	if ((flag & (int)ShaderBindFlag::HULL) != 0)
		context->HSSetShader(nullptr, 0, 0);

	if ((flag & (int)ShaderBindFlag::DOMAINS) != 0)
		context->DSSetShader(nullptr, 0, 0);

	if ((flag & (int)ShaderBindFlag::GEOMETRY) != 0)
		context->GSSetShader(nullptr, 0, 0);


}