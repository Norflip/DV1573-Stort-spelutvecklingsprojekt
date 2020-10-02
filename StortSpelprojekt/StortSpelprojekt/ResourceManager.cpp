#include "ResourceManager.h"

ResourceManager::ResourceManager()
{
}

ResourceManager::~ResourceManager()
{
	/*for (auto i : resources)
	{
		delete i.second;
	}

	resources.clear();*/
}

void ResourceManager::RemoveResource(std::string key)
{
}

void ResourceManager::InitializeResources(ID3D11Device* device)
{
	ReadObjects(device);
}

void ResourceManager::ReadObjects(ID3D11Device* device)
{
	std::ifstream file("data/Resources_objects.txt");
	if (file.is_open())
	{
		// Temp shader here, make sure we read the shader .txt file first and use the shaders there
		Shader tempShader;
		tempShader.SetPixelShader(L"Shaders/Default_ps.hlsl");
		tempShader.SetVertexShader(L"Shaders/Default_vs.hlsl");
		tempShader.Compile(device);

		int counter = 1;
		std::string delimiter = ": ";

		std::string line;
		std::string::size_type sz;

		std::getline(file, line);

		int pos = line.find(delimiter);
		int nrOfObjects = std::stoi(line.substr(pos + 2, 1), &sz);
		std::cout << "Nr of objects: " << nrOfObjects << std::endl;

		for (int i = 0; i < nrOfObjects; i++)
		{
			getline(file, line);
			
			int pos = line.find(delimiter);
			std::string name = line.substr(0, pos);
			std::string filepath = line.substr(pos + 2, line.length() - pos - 2);

			std::cout << i << ": " << name.c_str() << std::endl;
			std::cout << i << ": " << filepath.c_str() << std::endl;

			Material material = ZWEBLoader::LoadMaterials(filepath, tempShader, device)[0];
			Mesh mesh = ZWEBLoader::LoadMeshes(ZWEBLoadType::NoAnimation, filepath, device)[0];

			Object* testMesh = new Object(name);
			testMesh->AddComponent<MeshComponent>(mesh, material);

			AddResource(name, testMesh);
		}

	}
	else
	{
		std::cout << "Couldnt open file: " << std::endl;
	}
}

void ResourceManager::ReadLights()
{
}

void ResourceManager::ReadShaders()
{
}
