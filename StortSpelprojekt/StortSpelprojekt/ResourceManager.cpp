#include "ResourceManager.h"

ResourceManager::ResourceManager()
{
}

ResourceManager::~ResourceManager()
{

}

void ResourceManager::RemoveResource(std::string key)
{
}

void ResourceManager::InitializeResources(ID3D11Device* device)
{
	ReadShaders(device);
	ReadObjects(device);
}

void ResourceManager::ReadObjects(ID3D11Device* device)
{
	std::ifstream file("data/Resources_objects.txt");
	if (file.is_open())
	{
		std::string delimiter = ": ";

		std::string line;
		std::string::size_type sz;

		std::getline(file, line);

		int pos = line.find(delimiter);
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

			Shader* tempShader = GetResource<Shader>(shader);

			Material* material = new Material; *material = ZWEBLoader::LoadMaterials(filepath, tempShader, device)[0];
			Mesh* mesh = new Mesh; *mesh = ZWEBLoader::LoadMeshes(ZWEBLoadType::NoAnimation, filepath, device)[0];

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

void ResourceManager::ReadLights()
{
}

void ResourceManager::ReadShaders(ID3D11Device* device)
{
	std::ifstream file("data/Resources_shaders.txt");
	if (file.is_open())
	{
		std::string delimiter = ": ";
		std::string line;
		std::string::size_type sz;

		std::getline(file, line);
		int pos = line.find(delimiter);
		this->nrOfShaders = std::stoi(line.substr(pos + 2, 1), &sz);

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
				defaultShader->SetVertexShader(L"Shaders/Default_vs.hlsl");
				defaultShader->SetPixelShader(L"Shaders/Default_ps.hlsl");
				defaultShader->Compile(device);

				AddResource("defaultShader", defaultShader);
			}
			else
			{
				std:getline(file, line);
				pos = line.find(delimiter);
				std::string check = line.substr(0, pos);
				
				// If the file specifies what input layout to use
				if (check == "Input")
				{
					std::string input = line.substr(pos + 2, line.length() - pos - 2);

					std::getline(file, line);
					pos = line.find(delimiter);
					std::string vertex = line.substr(pos + 2, line.length() - pos - 2);

					std::getline(file, line);
					pos = line.find(delimiter);
					std::string pixel = line.substr(pos + 2, line.length() - pos - 2);

					std::wstring wVertex = std::wstring(vertex.begin(), vertex.end());
					std::wstring wPixel = std::wstring(pixel.begin(), pixel.end());

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
					
					tempShader->SetVertexShader(wVertex.c_str());
					tempShader->SetPixelShader(wPixel.c_str());
					tempShader->Compile(device);

					AddResource(name, tempShader);
				}
				else 
				{
					std::string vertex = line.substr(pos + 2, line.length() - pos - 2);

					std::getline(file, line);
					pos = line.find(delimiter);
					std::string pixel = line.substr(pos + 2, line.length() - pos - 2);

					std::wstring wVertex = std::wstring(vertex.begin(), vertex.end());
					std::wstring wPixel = std::wstring(pixel.begin(), pixel.end());

					Shader* tempShader = new Shader;
					tempShader->SetVertexShader(wVertex.c_str());
					tempShader->SetPixelShader(wPixel.c_str());
					tempShader->Compile(device);

					AddResource(name, tempShader);
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

