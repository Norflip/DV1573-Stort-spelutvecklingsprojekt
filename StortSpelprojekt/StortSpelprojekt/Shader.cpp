#include "Shader.h"

Shader::Shader() : pixelShader(nullptr), vertexShader(nullptr), inputLayout(nullptr), geometryShader(nullptr)
{
	shaderCompilationFlag = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
	shaderCompilationFlag = shaderCompilationFlag | D3DCOMPILE_DEBUG;
#endif

	SetInputLayoutStructure(5, DEFAULT_INPUT_LAYOUTd);
}

Shader::~Shader()
{
	/*if (skeletonShader)
	{
		skeletonShader->Release(); WHERE SHOULD THIS GO!=!?!?
	}*/
	
}

void Shader::SetPixelShader(LPCWSTR path, LPCSTR entry)
{
	this->pixelPath = path;
	this->pixelEntry = entry;
	shaderFlags |= ShaderBindFlag::PIXEL;
}

void Shader::SetVertexShader(LPCWSTR path, LPCSTR entry)
{
	this->vertexPath = path;
	this->vertexEntry = entry;
	shaderFlags |= ShaderBindFlag::VERTEX;
}

void Shader::SetGeometryShader(LPCWSTR path, LPCSTR entry)
{
	this->geometryPath = path;
	this->geometryEntry = entry;
	shaderFlags |= ShaderBindFlag::GEOMETRY;
}

void Shader::setSkeletonShader(LPCWSTR path, LPCSTR entry)
{
	this->skeletonPath = path;
	this->skeletonEntry = entry;
	shaderFlags |= ShaderBindFlag::SKELETON;
}

void Shader::SetInputLayoutStructure(size_t arraySize, D3D11_INPUT_ELEMENT_DESC* inputLayoutDesc)
{
	this->inputLayoutDescription = inputLayoutDesc;
	this->ilArrayCount = 8;//arraySize;
}

void Shader::Compile(ID3D11Device* device)
{
	CompileVS(device);
	CompilePS(device);
	CompileGS(device);
	CompileSkeletonVS(device);
}

void Shader::BindToContext(ID3D11DeviceContext* context)
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
	if ((flag & (int)ShaderBindFlag::SKELETON) != 0)
	{
		context->IASetInputLayout(inputLayout);
		context->VSSetShader(skeletonShader, 0, 0);
	}
}

void Shader::CompilePS(ID3D11Device* device)
{
	if (((int)shaderFlags & (int)ShaderBindFlag::PIXEL) != 0)
	{
		if (pixelShader != nullptr)
		{
			delete pixelShader;
			pixelShader = nullptr;
		}

		ID3DBlob* errorBlob = nullptr;
		ID3DBlob* PSBlob = nullptr;
		
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
	}

}

void Shader::CompileVS(ID3D11Device* device)
{

	if (((int)shaderFlags & (int)ShaderBindFlag::VERTEX) != 0)
	{

		if (vertexShader != nullptr)
		{
			delete vertexShader;
			vertexShader = nullptr;
		}

		ID3DBlob* errorBlob = nullptr;
		ID3DBlob* VSBlob = nullptr;
		
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
			delete geometryShader;
			geometryShader = nullptr;
		}

		ID3DBlob* errorBlob = nullptr;
		ID3DBlob* GSBlob = nullptr;

		HRESULT GSCompileResult = D3DCompileFromFile
		(
			geometryPath,
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

void Shader::CompileSkeletonVS(ID3D11Device* device)
{

	if (((int)shaderFlags & (int)ShaderBindFlag::SKELETON) != 0)
	{

		if (skeletonShader != nullptr)
		{
			delete skeletonShader;
			skeletonShader = nullptr;
		}

		ID3DBlob* errorBlob = nullptr;
		ID3DBlob* VSSkeletonBlob = nullptr;

		// Skeleton VERTEX SHADER
		HRESULT	VSSkeletonCompileResult = D3DCompileFromFile
		(
			skeletonPath,
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			skeletonEntry,
			"vs_5_0",
			shaderCompilationFlag,
			0,
			&VSSkeletonBlob,
			&errorBlob
		);

		if (FAILED(VSSkeletonCompileResult) && errorBlob)
		{
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			Log::Add(Log::LogLevel::Error, (char*)errorBlob->GetBufferPointer());
			errorBlob->Release();
		}

		HRESULT VSCreateResult = device->CreateVertexShader(VSSkeletonBlob->GetBufferPointer(), VSSkeletonBlob->GetBufferSize(), nullptr, &this->skeletonShader);
		assert(SUCCEEDED(VSCreateResult));

		
	}






}
