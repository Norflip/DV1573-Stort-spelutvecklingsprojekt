#include "Shader.h"

Shader::Shader() : pixelShader(nullptr), vertexShader(nullptr), inputLayout(nullptr)
{
	shaderCompilationFlag = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
	shaderCompilationFlag = shaderCompilationFlag | D3DCOMPILE_DEBUG;
#endif

	SetInputLayoutStructure(5, DEFAULT_INPUT_LAYOUTd);
}

Shader::~Shader()
{
}

void Shader::SetPixelShader(LPCWSTR path, LPCSTR entry)
{
	this->pixelPath = path;
	this->pixelEntry = entry;
}

void Shader::SetVertexShader(LPCWSTR path, LPCSTR entry)
{
	this->vertexPath = path;
	this->vertexEntry = entry;
}

void Shader::SetInputLayoutStructure(size_t arraySize, D3D11_INPUT_ELEMENT_DESC* inputLayoutDesc)
{
	this->inputLayoutDescription = inputLayoutDesc;
	this->arraySize = arraySize;
}

void Shader::Compile(ID3D11Device* device)
{
	if (pixelShader != nullptr)
	{
		delete pixelShader;
		pixelShader = nullptr;
	}

	if (vertexShader != nullptr)
	{
		delete vertexShader;
		vertexShader = nullptr;
	}

	ID3DBlob* errorBlob = nullptr;
	ID3DBlob* PSBlob = nullptr;
	ID3DBlob* VSBlob = nullptr;

	HRESULT PSCompileResult = D3DCompileFromFile
	(
		pixelPath,
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



	// VERTEX SHADER
	HRESULT	VSCompileResult = D3DCompileFromFile
	(
		vertexPath,
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

	HRESULT createInputLayoutResult = device->CreateInputLayout(inputLayoutDescription, arraySize, VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), &inputLayout);
	assert(SUCCEEDED(createInputLayoutResult));
}

void Shader::BindToContext(ID3D11DeviceContext* context)
{
	// sets the vertex shader and layout
	context->IASetInputLayout(inputLayout);
	context->VSSetShader(vertexShader, 0, 0);

	//sets the pixel shader	
	context->PSSetShader(pixelShader, 0, 0);
}
