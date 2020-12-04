
#include "Shader.h"

Shader::Shader() : pixelShader(nullptr), vertexShader(nullptr), inputLayout(nullptr), geometryShader(nullptr), hullShader(nullptr), domainShader(nullptr), computeShader(nullptr)
{
	shaderCompilationFlag = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
	shaderCompilationFlag = shaderCompilationFlag | D3DCOMPILE_DEBUG;
#endif

	SetInputLayoutStructure(6, DEFAULT_INPUT_LAYOUTd);
}

Shader::~Shader()
{
	RELEASE(pixelShader);
	RELEASE(vertexShader);
	RELEASE(inputLayout);
	RELEASE(geometryShader);
	RELEASE(hullShader);
	RELEASE(domainShader);
	RELEASE(computeShader);
}

void Shader::SetPixelShader(std::string path, LPCSTR entry)
{
	this->pixelPath = path;
	this->pixelEntry = entry;
	shaderFlags |= ShaderBindFlag::PIXEL;
}

void Shader::SetVertexShader(std::string path, LPCSTR entry)
{
	this->vertexPath = path;
	this->vertexEntry = entry;
	shaderFlags |= ShaderBindFlag::VERTEX;
}

void Shader::SetHullShader(std::string path, LPCSTR entry)
{
	this->hullPath = path;
	this->hullEntry = entry;
	shaderFlags |= ShaderBindFlag::HULL;
}

void Shader::SetDomainShader(std::string path, LPCSTR entry)
{
	this->domainPath = path;
	this->domainEntry = entry;
	shaderFlags |= ShaderBindFlag::DOMAINS;
}
void Shader::SetComputeShader(std::string path, LPCSTR entry)
{
	this->computePath = path;
	this->computeEntry = entry;
	shaderFlags |= ShaderBindFlag::COMPUTE;
}
void Shader::SetGeometryShader(std::string path, LPCSTR entry)
{
	this->geometryPath = path;
	this->geometryEntry = entry;
	shaderFlags |= ShaderBindFlag::GEOMETRY;
}

void Shader::SetSOGeometryShader(std::string path, LPCSTR entry)
{
	this->geometryPath = path;
	this->geometryEntry = entry;
	shaderFlags |= ShaderBindFlag::SOGEOMETRY;
}

void Shader::SetInputLayoutStructure(size_t arraySize, D3D11_INPUT_ELEMENT_DESC* inputLayoutDesc)
{
	this->inputLayoutDescription = inputLayoutDesc;
	this->ilArrayCount = arraySize;
}


void Shader::Compile(ID3D11Device* device)
{	
	CompileCS(device);
	CompileVS(device);
	CompilePS(device);
	CompileGS(device);
	CompileSOGS(device);
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

	if ((flag & (int)ShaderBindFlag::SOGEOMETRY) != 0)
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
	if ((flag & (int)ShaderBindFlag::COMPUTE) != 0)
	{
		context->CSSetShader(computeShader, 0, 0);
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
		std::wstring wPath = std::wstring(pixelPath.begin(), pixelPath.end());

		const D3D_SHADER_MACRO defines[] =
		{
#if _DEBUG
			"_DEBUG", "1",
#else
			"_DEBUG", "0",
#endif
			NULL, NULL
		};

		HRESULT PSCompileResult = D3DCompileFromFile
		(
			wPath.c_str(),
			defines,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			pixelEntry,
			"ps_5_0",
			shaderCompilationFlag,
			0,
			&PSBlob,
			&errorBlob
		);

		ASSERT_SHADER(PSCompileResult, errorBlob, wPath);

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
		std::wstring wPath = std::wstring(vertexPath.begin(), vertexPath.end());

		const D3D_SHADER_MACRO defines[] =
		{
#if _DEBUG
			"_DEBUG", "1",
#else
			"_DEBUG", "0",
#endif
			NULL, NULL
		};

		// VERTEX SHADER
		HRESULT	VSCompileResult = D3DCompileFromFile
		(
			wPath.c_str(),
			defines,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			vertexEntry,
			"vs_5_0",
			shaderCompilationFlag,
			0,
			&VSBlob,
			&errorBlob
		);

		ASSERT_SHADER(VSCompileResult, errorBlob, wPath);

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
		std::wstring wPath = std::wstring(geometryPath.begin(), geometryPath.end());

		const D3D_SHADER_MACRO defines[] =
		{
#if _DEBUG
			"_DEBUG", "1",
#else
			"_DEBUG", "0",
#endif
			NULL, NULL
		};

		HRESULT GSCompileResult = D3DCompileFromFile
		(
			wPath.c_str(),
			defines,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			geometryEntry,
			"gs_5_0",
			shaderCompilationFlag,
			0,
			&GSBlob,
			&errorBlob
		);

		ASSERT_SHADER(GSCompileResult, errorBlob, wPath);

		HRESULT GSCreateResult = device->CreateGeometryShader(GSBlob->GetBufferPointer(), GSBlob->GetBufferSize(), nullptr, &this->geometryShader);
		assert(SUCCEEDED(GSCreateResult));
	}
}

void Shader::CompileSOGS(ID3D11Device* device)
{
	if (((int)shaderFlags & (int)ShaderBindFlag::SOGEOMETRY) != 0)
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
		std::wstring wPath = std::wstring(geometryPath.begin(), geometryPath.end());

		const D3D_SHADER_MACRO defines[] =
		{
#if _DEBUG
			"_DEBUG", "1",
#else
			"_DEBUG", "0",
#endif
			NULL, NULL
		};

		HRESULT GSCompileResult = D3DCompileFromFile
		(
			wPath.c_str(),
			defines,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			geometryEntry,
			"gs_5_0",
			shaderCompilationFlag,
			0,
			&GSBlob,
			&errorBlob
		);

		ASSERT_SHADER(GSCompileResult, errorBlob, wPath);

		//HRESULT GSCreateResult = device->CreateGeometryShader(GSBlob->GetBufferPointer(), GSBlob->GetBufferSize(), nullptr, &this->geometryShader);
		//assert(SUCCEEDED(GSCreateResult));

		D3D11_SO_DECLARATION_ENTRY soDeclaration[] = {

			{0,"POSITION", 0, 0,3,0},
			{0,"VELOCITY", 0, 0, 3, 0},
			{0,"SIZE", 0, 0, 2, 0},
			{0,"AGE", 0, 0, 1, 0},
			{0,"TYPE", 0, 0, 1, 0}
		};
		
		UINT stride = sizeof(Mesh::Particle);
		HRESULT hr = device->CreateGeometryShaderWithStreamOutput(GSBlob->GetBufferPointer(), GSBlob->GetBufferSize(), 
			soDeclaration, _countof(soDeclaration), &stride, 1, 0, NULL, &geometryShader);
		assert(SUCCEEDED(hr));
	}
}

void Shader::CompileHS(ID3D11Device* device)
{
	if (((int)shaderFlags & (int)ShaderBindFlag::HULL) != 0)
	{

		if (hullShader != nullptr)
		{
			//delete hullShader;
			//hullShader = nullptr;
			hullShader->Release();
		}

		ID3DBlob* errorBlob = nullptr;
		ID3DBlob* HSBlob = nullptr;

		// Convert the string to a wstring locally, without changing the content
		std::wstring wPath = std::wstring(hullPath.begin(), hullPath.end());

		const D3D_SHADER_MACRO defines[] =
		{
#if _DEBUG
			"_DEBUG", "1",
#else
			"_DEBUG", "0",
#endif
			NULL, NULL
		};

		HRESULT HSCompileResult = D3DCompileFromFile
		(
			wPath.c_str(),
			defines,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			hullEntry,
			"hs_5_0",
			shaderCompilationFlag,
			0,
			&HSBlob,
			&errorBlob
		);

		ASSERT_SHADER(HSCompileResult, errorBlob, wPath);

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
			/*delete domainShader;
			domainShader = nullptr;*/
			domainShader->Release();
		}

		ID3DBlob* errorBlob = nullptr;
		ID3DBlob* DSBlob = nullptr;

		// Convert the string to a wstring locally, without changing the content
		std::wstring wPath = std::wstring(domainPath.begin(), domainPath.end());

		const D3D_SHADER_MACRO defines[] =
		{
#if _DEBUG
			"_DEBUG", "1",
#else
			"_DEBUG", "0",
#endif
			NULL, NULL
		};
		HRESULT DSCompileResult = D3DCompileFromFile
		(
			wPath.c_str(),
			defines,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			domainEntry,
			"ds_5_0",
			shaderCompilationFlag,
			0,
			&DSBlob,
			&errorBlob
		);

		ASSERT_SHADER(DSCompileResult, errorBlob, wPath);

		HRESULT DSCreateResult = device->CreateDomainShader(DSBlob->GetBufferPointer(), DSBlob->GetBufferSize(), nullptr, &this->domainShader);
		assert(SUCCEEDED(DSCreateResult));
	}
}

void Shader::CompileCS(ID3D11Device* device)
{
	if (((int)shaderFlags & (int)ShaderBindFlag::COMPUTE) != 0)
	{
		if (computeShader != nullptr)
		{
			computeShader->Release();
		}
		ID3DBlob* errorBlob = nullptr;
		ID3DBlob* CSBlob = nullptr;

		// Convert the string to a wstring locally, without changing the content
		std::wstring wPath = std::wstring(computePath.begin(), computePath.end());

		const D3D_SHADER_MACRO defines[] =
		{
	#if _DEBUG
				"_DEBUG", "1",
	#else
				"_DEBUG", "0",
	#endif
				NULL, NULL
		};
		HRESULT CSCompileResult = D3DCompileFromFile
		(
			wPath.c_str(),
			defines,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			computeEntry,
			"cs_5_0",
			shaderCompilationFlag,
			0,
			&CSBlob,
			&errorBlob
		);

		ASSERT_SHADER(CSCompileResult, errorBlob, wPath);

		HRESULT CSCreateResult = device->CreateComputeShader(CSBlob->GetBufferPointer(), CSBlob->GetBufferSize(), nullptr, &this->computeShader);
		assert(SUCCEEDED(CSCreateResult));
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

	if ((flag & (int)ShaderBindFlag::COMPUTE) != 0)
		context->CSSetShader(nullptr, 0, 0);

	if ((flag & (int)ShaderBindFlag::SOGEOMETRY) != 0)
		context->GSSetShader(nullptr, 0, 0);

}