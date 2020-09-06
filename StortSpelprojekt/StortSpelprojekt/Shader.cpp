#include "Shader.h"

Shader::Shader() : pixelShader(nullptr), vertexShader(nullptr), inputLayout(nullptr),skeletonVertexShader(nullptr),skeletonInputLayout(nullptr)
{
	shaderCompilationFlag = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
	shaderCompilationFlag = shaderCompilationFlag | D3DCOMPILE_DEBUG;
#endif

	SetInputLayoutStructure(4, DEFAULT_INPUT_LAYOUTd);
	SetSkeletonInputLayoutStructure(6, SKELETON_INPUT_LAYOUTd);
}

Shader::~Shader()
{
	/*skeletonInputLayout->Release();
	skeletonVertexShader->Release();
	inputLayoutSkeletonDescription = nullptr;*/

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

void Shader::SetSkeletonVertexShader(LPCWSTR path, LPCSTR entry)
{
	this->skeletonVertexPath = path;
	this->skeletonVertexEntry = entry;
}

void Shader::SetInputLayoutStructure(size_t arraySize, D3D11_INPUT_ELEMENT_DESC* inputLayoutDesc)
{
	this->inputLayoutDescription = inputLayoutDesc;
	this->arraySize = arraySize;
}

void Shader::SetSkeletonInputLayoutStructure(size_t arraySize, D3D11_INPUT_ELEMENT_DESC* inputLayoutDesc)
{
	this->inputLayoutSkeletonDescription = inputLayoutDesc;
	this->arraySizeSkeleton = arraySize;
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

	if (skeletonVertexShader != nullptr)
	{
		delete skeletonVertexShader;
		skeletonVertexShader = nullptr;
	}

	ID3DBlob* errorBlob = nullptr;
	ID3DBlob* PSBlob = nullptr;
	ID3DBlob* VSBlob = nullptr;
	ID3DBlob* SkeletonVSBlob = nullptr;

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


	//SkeletonVertexShader

	
	VSCreateResult = D3DCompileFromFile
	(
		skeletonVertexPath,
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		skeletonVertexEntry,
		"vs_5_0",
		shaderCompilationFlag,
		0,
		&SkeletonVSBlob,
		&errorBlob
	);

	VSCreateResult = device->CreateVertexShader(SkeletonVSBlob->GetBufferPointer(), SkeletonVSBlob->GetBufferSize(), nullptr, &this->skeletonVertexShader);
	assert(SUCCEEDED(VSCreateResult));

	VSCreateResult = device->CreateInputLayout(inputLayoutSkeletonDescription, arraySizeSkeleton, SkeletonVSBlob->GetBufferPointer(), SkeletonVSBlob->GetBufferSize(), &skeletonInputLayout);
	assert(SUCCEEDED(VSCreateResult));
}

void Shader::BindToContext(ID3D11DeviceContext* context)
{
	// sets the vertex shader and layout
	context->IASetInputLayout(inputLayout);
	context->VSSetShader(vertexShader, 0, 0);

	//sets the pixel shader	
	context->PSSetShader(pixelShader, 0, 0);
}

void Shader::BindSkeletonToContext(ID3D11DeviceContext* context)
{
	// sets the vertex shader and layout
	context->IASetInputLayout(skeletonInputLayout);
	context->VSSetShader(skeletonVertexShader, 0, 0);

	//sets the pixel shader	
	context->PSSetShader(pixelShader, 0, 0);
}
