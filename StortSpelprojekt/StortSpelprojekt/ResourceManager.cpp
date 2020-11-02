#include "stdafx.h"
#include "ResourceManager.h"

ResourceManager::ResourceManager()
{
}

ResourceManager::~ResourceManager()
{
	/*for (auto i : resources)
	{
		delete i.second;
	}*/

	for (auto i : shaderResources)
	{
		delete i.second;
	}

	resources.clear();
	shaderResources.clear();
}

void ResourceManager::AddShaderResource(std::string key, Shader* shader)
{
	auto iterator = shaderResources.find(key);
	if (iterator == shaderResources.end())
		shaderResources.insert({ key, shader });
}

Shader*& ResourceManager::GetShaderResource(std::string key)
{
	Shader* crap = nullptr;
	auto iterator = shaderResources.find(key);

	if (iterator != shaderResources.end())
	{
		return (*iterator).second;
	}
	else
	{
		// Wouldnt let me return nullptr directly
		// But this nullptr varible is somehow fine
		return crap;
	}
}

void ResourceManager::RemoveResource(std::string key)
{
	auto iterator = resources.find(key);
	if (iterator != resources.end())
	{
		resources.erase(iterator);
	}
	else // If the key isnt found in the resource map, check the shaderResources
	{
		auto iterator2 = shaderResources.find(key);
		if (iterator2 != shaderResources.end())
		{
			shaderResources.erase(key);
		}
	}
}

void ResourceManager::InitializeResources(ID3D11Device* device)
{
	ReadTextures(device);
	ReadShaders(device);
	ReadObjects(device);
}

void ResourceManager::ReadObjects(ID3D11Device* device)
{
	// Start reading the file with the objects
	std::ifstream file("data/Resources_objects.txt");
	if (file.is_open())
	{
		// Delimiter is used to seperate contents inside a string
		std::string delimiter = ": ";

		std::string line;
		std::string::size_type sz;

		std::getline(file, line);

		int pos = line.find(delimiter);
		
		// Number of objects in this file
		int nrOfObjects = std::stoi(line.substr(pos + 2, 1), &sz);

		std::getline(file, line);

		for (int i = 0; i < nrOfObjects; i++)
		{
			getline(file, line);
			
			int pos = line.find(delimiter);
			std::string name = line.substr(0, pos);
			std::string filepath = line.substr(pos + 2, line.length() - pos - 2);

			getline(file, line);
			pos = line.find(delimiter);

			std::string shader = line.substr(pos + 2, line.length() - pos - 2);

			// Right now, it supports only 1 mesh and material for each path
			// Will check if we need to load a whole vector with several meshes at the same time
			//std::vector<Material*> materials = ZWEBLoader::LoadMaterials(filepath, GetShaderResource(shader), device);
			//std::vector<Mesh*> meshes = ZWEBLoader::LoadMeshes(ZWEBLoadType::NoAnimation, filepath, device);

			auto sampler = DXHelper::CreateSampler(D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_WRAP, device);

			/*if (meshes.size() > 1)
			{
				for (int i = 0; i < meshes.size(); i++)
				{
					materials[i]->SetSampler(sampler, 0, ShaderBindFlag::PIXEL);

					AddResource(meshes[i]->GetMeshName(), meshes[i]);
					AddResource(meshes[i]->GetMaterialName(), materials[i]);
				}
			}
			else
			{
				materials[0]->SetSampler(sampler, 0, ShaderBindFlag::PIXEL);

				AddResource(name, meshes[0]);
				AddResource(name + "Material", materials[0]);
			}*/
			

			Material* material = new Material;
			*material = ZWEBLoader::LoadMaterials(filepath, GetShaderResource(shader), device)[0];

			
			material->SetSampler(sampler, 0, ShaderBindFlag::PIXEL);

			Mesh* mesh = new Mesh;
			*mesh = ZWEBLoader::LoadMeshes(ZWEBLoadType::NoAnimation, filepath, device)[0];

			AddResource(name, mesh);
			AddResource(name + "Material", material);

			std::getline(file, line);
		}

		file.close();
	}
	else
	{
		std::cout << "Couldnt open file: " << std::endl;
	}
}

void ResourceManager::ReadTextures(ID3D11Device* device)
{
	std::ifstream file("data/Resources_textures.txt");
	if (file.is_open())
	{
		// Delimiter is used to seperate content inside a string
		std::string delimiter = ": ";
		std::string line;
		std::string::size_type sz;

		std::getline(file, line);
		int pos = line.find(delimiter);

		// Amount of textures in the file
		int nrOfTextures = std::stoi(line.substr(pos + 2, 1), &sz);

		std::getline(file, line);

		for (int i = 0; i < nrOfTextures; i++)
		{
			std::getline(file, line);
			pos = line.find(delimiter);
			std::string name = line.substr(0, pos);
			std::string filepath = line.substr(pos + 2, line.length() - pos - 2);

			Texture* texture = new Texture;
			std::wstring pathWSTR(filepath.begin(), filepath.end());

			bool success = texture->LoadTexture(device, pathWSTR.c_str());
			assert(success);

			std::getline(file, line);
			AddResource(name, texture);
		}
	}
}

void ResourceManager::ReadShaders(ID3D11Device* device)
{
	// Start reading the file with the shaders
	std::ifstream file("data/Resources_shaders.txt");
	if (file.is_open())
	{
		// Delimiter is used to seperate content inside a string
		std::string delimiter = ": ";
		std::string line;
		std::string::size_type sz;

		std::getline(file, line);
		int pos = line.find(delimiter);

		// Amount of shaders in the file
		int nrOfShaders = std::stoi(line.substr(pos + 2, 1), &sz);

		std::getline(file, line);

		for (int i = 0; i < nrOfShaders; i++)
		{
			std::getline(file, line);
			pos = line.find(delimiter);
			std::string name = line.substr(pos + 2, line.length() - pos - 2);

			// Default shader
			if (name == "default")
			{
				Shader* defaultShader = new Shader;

				defaultShader->SetVertexShader("Shaders/Default_vs.hlsl");
				defaultShader->SetPixelShader("Shaders/Default_ps.hlsl");
				defaultShader->Compile(device);

				AddShaderResource("defaultShader", defaultShader);
			}
			else
			{
				std::getline(file, line);
				pos = line.find(delimiter);
				std::string check = line.substr(0, pos);
				
				std::string pixelPath;
				std::string vertexPath;
				std::string hullPath;
				std::string domainPath;
				std::string geometryPath;

				// If the file specifies what input layout to use
				if (check == "Input")
				{
					std::string input = line.substr(pos + 2, line.length() - pos - 2);

					std::getline(file, line);
					pos = line.find(delimiter);
					vertexPath = line.substr(pos + 2, line.length() - pos - 2);

					std::getline(file, line);
					pos = line.find(delimiter);

					std::string type = line.substr(0, pos);

					if (type == "HS")
					{
						hullPath = line.substr(pos + 2, line.length() - pos - 2);

						getline(file, line);
						pos = line.find(delimiter);
						domainPath = line.substr(pos + 2, line.length() - pos - 2);

						getline(file, line);
						pos = line.find(delimiter);
						geometryPath = line.substr(pos + 2, line.length() - pos - 2);

						getline(file, line);
						pos = line.find(delimiter);
						pixelPath = line.substr(pos + 2, line.length() - pos - 2);
					}
					else if(type == "PS")
					{
						pixelPath = line.substr(pos + 2, line.length() - pos - 2);
					}

					Shader* tempShader = new Shader;

					// Make sure we use the specified input layout
					if (input == "skeleton")
					{
						tempShader->SetInputLayoutStructure(8, tempShader->SKELETON_INPUT_LAYOUTd);
					}
					else if (input == "instance")
					{
						tempShader->SetInputLayoutStructure(10, tempShader->INSTANCE_INPUT_LAYOUTd);
					}
					
					// Compile the shader
					tempShader->SetVertexShader(vertexPath);
					if(hullPath != "")
						tempShader->SetHullShader(hullPath);
					if (domainPath != "")
						tempShader->SetDomainShader(domainPath);
					if (geometryPath != "")
						tempShader->SetGeometryShader(geometryPath);

					tempShader->SetPixelShader(pixelPath);
					tempShader->Compile(device);

					AddShaderResource(name, tempShader);
				}
				else // If there is no specified input layout, the shader will use default
				{
					std::string vertexPath = line.substr(pos + 2, line.length() - pos - 2);

					std::getline(file, line);
					pos = line.find(delimiter);
					std::string type = line.substr(0, pos);

					if (type == "HS")
					{
						hullPath = line.substr(pos + 2, line.length() - pos - 2);

						getline(file, line);
						pos = line.find(delimiter);
						domainPath = line.substr(pos + 2, line.length() - pos - 2);

						getline(file, line);
						pos = line.find(delimiter);
						geometryPath = line.substr(pos + 2, line.length() - pos - 2);

						getline(file, line);
						pos = line.find(delimiter);
						pixelPath = line.substr(pos + 2, line.length() - pos - 2);
					}
					else if (type == "PS")
					{
						pixelPath = line.substr(pos + 2, line.length() - pos - 2);
					}
					std::string pixelPath = line.substr(pos + 2, line.length() - pos - 2);

					Shader* tempShader = new Shader;

					// Compile the shader
					tempShader->SetVertexShader(vertexPath);
					if (hullPath != "")
						tempShader->SetHullShader(hullPath);
					if (domainPath != "")
						tempShader->SetDomainShader(domainPath);
					if (geometryPath != "")
						tempShader->SetGeometryShader(geometryPath);

					tempShader->SetPixelShader(pixelPath);
					tempShader->Compile(device);

					AddShaderResource(name, tempShader);
				}
			}
			std::getline(file, line);
		}
		file.close();
	}
	else
	{
		std::cout << "Couldnt open file" << std::endl;
	}
}

void ResourceManager::CompileShaders(ID3D11Device* device)
{
	// Recompile every shader in the shaderResource map
	for (auto& it : shaderResources)
	{
		it.second->Compile(device);
	}
}

Object* ResourceManager::AssembleObject(std::string meshName, std::string materialName)
{
	Object* object = new Object(meshName);

	object->AddComponent<MeshComponent>(*GetResource<Mesh>(meshName), *GetResource<Material>(materialName));

	return object;
}


