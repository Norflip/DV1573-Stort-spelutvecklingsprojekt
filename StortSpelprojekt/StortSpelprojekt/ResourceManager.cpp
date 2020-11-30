#include "stdafx.h"
#include "ResourceManager.h"

ResourceManager::ResourceManager()
{
}

ResourceManager::~ResourceManager()
{
	for (auto i : resources)
	{
		delete i.second;
	}

	for (auto i : shaderResources)
	{
		delete i.second;
	}

	resources.clear();
	shaderResources.clear();
}

void ResourceManager::AddResource(std::string key, Resource* resource)
{
	auto iterator = resources.find(key);
	if (iterator == resources.end())
		resources.insert({ key, resource });
}

void ResourceManager::AddShaderResource(std::string key, Shader* shader)
{
	auto iterator = shaderResources.find(key);
	if (iterator == shaderResources.end())
		shaderResources.insert({ key, shader });
}

Resource* ResourceManager::GetResource(std::string key)
{
	Resource* resource = nullptr;
	auto iterator = resources.find(key);

	if (iterator != resources.end())
		resource = (*iterator).second;
	
	return resource;
}

Shader* ResourceManager::GetShaderResource(std::string key)
{
	auto iterator = shaderResources.find(key);

	if (iterator != shaderResources.end())
	{
		return (*iterator).second;
	}
	else
	{
		// Wouldnt let me return nullptr directly
		// But this nullptr varible is somehow fine
		return nullptr;
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
		int nrOfObjects = std::stoi(line.substr(pos + 2, line.length() - pos - 2), &sz);

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
			
			auto sampler = DXHelper::CreateSampler(D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_WRAP, device);

			if (shader == "skeletonShader" || shader == "houseShader")
			{
				std::vector<Material*> materials = ZWEBLoader::LoadMaterials(filepath, GetShaderResource(shader), device);
				std::vector<Mesh*> meshes = ZWEBLoader::LoadMeshes(ZWEBLoadType::SkeletonAnimation, filepath, device);

				std::getline(file, line);
				pos = line.find(delimiter);

				int nrOfAnimations = std::stoi(line.substr(pos + 2, line.length() - pos - 2), &sz);

				SkeletonMeshComponent* skeletonMesh = new SkeletonMeshComponent(meshes[0], materials[0]);

				for (int j = 0; j < nrOfAnimations; j++)
				{
					std::getline(file, line);
					pos = line.find(delimiter);

					int type = std::stoi(line.substr(0, pos));
					std::string path = line.substr(pos + 2, line.length() - pos - 2);

					if (name == "House")
					{
						SkeletonAni animation = ZWEBLoader::LoadSkeletonOnly(path, meshes[0]->GetBoneIDS(), true);
						skeletonMesh->SetAnimationTrack(animation, (SkeletonStateMachine)type);
					}
					else
					{
						SkeletonAni animation = ZWEBLoader::LoadSkeletonOnly(path, meshes[0]->GetBoneIDS(), false);
						skeletonMesh->SetAnimationTrack(animation, (SkeletonStateMachine)type);
					}
				}

				skeletonMesh->BlendAnimations();
				AddResource(name+"Skeleton", skeletonMesh);
			}
			// Ugly presumption that we load a Tree at some point
			else if (name == "Tree")
			{
				std::vector<Material*> materials = ZWEBLoader::LoadMaterials(filepath, GetShaderResource(shader), device);
				std::vector<Mesh*> meshes = ZWEBLoader::LoadMeshes(ZWEBLoadType::NoAnimation, filepath, device);

				materials[0]->SetSampler(sampler, 0, ShaderBindFlag::PIXEL);
				materials[1]->SetSampler(sampler, 0, ShaderBindFlag::PIXEL);

				materials[1]->SetShader(GetShaderResource("alphaInstanceShader"));

				AddResource(name, meshes[0]);
				AddResource(meshes[1]->GetMeshName(), meshes[1]);

				AddResource(name + "Material", materials[0]);
				AddResource(meshes[1]->GetMeshName() + "Material", materials[1]);
			}
			else
			{
				std::vector<Material*> materials = ZWEBLoader::LoadMaterials(filepath, GetShaderResource(shader), device);
				std::vector<Mesh*> meshes = ZWEBLoader::LoadMeshes(ZWEBLoadType::NoAnimation, filepath, device);

				materials[0]->SetSampler(sampler, 0, ShaderBindFlag::PIXEL);

				AddResource(name, meshes[0]);
				AddResource(name + "Material", materials[0]);
			}

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

		// Add each texture in the file
		for (int i = 0; i < nrOfTextures; i++)
		{
			std::getline(file, line);
			pos = line.find(delimiter);
			std::string name = line.substr(0, pos);
			std::string filepath = line.substr(pos + 2, line.length() - pos - 2);

			std::wstring pathWSTR(filepath.begin(), filepath.end());
			Texture* texture = Texture::LoadTexture(device, pathWSTR.c_str());
			assert(texture);

			// Read empty line
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
		int nrOfShaders = std::stoi(line.substr(pos + 2, line.length() - pos - 2), &sz);

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
						tempShader->SetInputLayoutStructure(9, tempShader->INSTANCE_INPUT_LAYOUTd);
					}
					else if (input == "color")
					{
						tempShader->SetInputLayoutStructure(3, tempShader->DEFAULT_INPUT_LAYOUTCOLOR);
					}
					else 
					{
						tempShader->SetInputLayoutStructure(6, tempShader->DEFAULT_INPUT_LAYOUTd);
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

void ResourceManager::ReadAnimations(ID3D11Device* device)
{
	std::ifstream file("data/Resources_animations.txt");
	if (file.is_open())
	{
		// Delimiter is used to seperate content inside a string
		std::string delimiter = ": ";
		std::string line;
		std::string::size_type sz;

		std::getline(file, line);
		int pos = line.find(delimiter);

		// Amount of shaders in the file
		int nrOfObjects = std::stoi(line.substr(pos + 2, line.length() - pos - 2), &sz);

		std::getline(file, line);

		for (int i = 0; i < nrOfObjects; i++)
		{
			std::getline(file, line);
			pos = line.find(delimiter);

			int nrOfAnimations = std::stoi(line.substr(pos + 2, line.length() - pos - 2), &sz);

			for (int j = 0; j < nrOfAnimations; j++)
			{
				std::getline(file, line);

				pos = line.find(delimiter);
				
				int type = std::stoi(line.substr(0, pos));
				std::string filepath = line.substr(pos + 2, line.length() - pos - 2);

				
			}
		}
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

Object* ResourceManager::AssembleObject(std::string meshName, std::string materialName, bool batchable, ObjectFlag flag)
{
	Object* object = new Object(meshName, flag);

	MeshComponent* meshComponent = object->AddComponent<MeshComponent>(GetResource<Mesh>(meshName), GetResource<Material>(materialName));
	meshComponent->SetBatchable(batchable);

	return object;
}


