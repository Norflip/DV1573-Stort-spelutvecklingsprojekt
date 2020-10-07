#include "Shader.h"

Shader::Shader() : pixelShader(nullptr), vertexShader(nullptr), inputLayout(nullptr), geometryShader(nullptr)
{
	shaderCompilationFlag = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
	shaderCompilationFlag = shaderCompilationFlag | D3DCOMPILE_DEBUG;
#endif

	SetInputLayoutStructure(6, DEFAULT_INPUT_LAYOUTd);
}

Shader::~Shader()
{
	/*if (vertexShader)
		vertexShader->Release(); 
	
	if (pixelShader)
		pixelShader->Release();

	if (geometryShader)
		geometryShader->Release();*/
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

void Shader::SetGeometryShader(std::string path, LPCSTR entry)
{
	//this->geometryPath = path;
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

