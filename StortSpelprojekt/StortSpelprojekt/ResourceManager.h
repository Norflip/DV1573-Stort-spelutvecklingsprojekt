#pragma once
#include <unordered_map>
#include <string>
#include <fstream>
#include "ZWEBLoader.h"

class ResourceManager
{
private:

	// Maps for different resources and shaders
	std::unordered_map<std::string, void*> resources;
	std::unordered_map<std::string, Shader*> shaderResources;

public:
	ResourceManager();
	virtual ~ResourceManager();

	template <typename T> void AddResource(std::string key, T* resource);
	void AddShaderResource(std::string key, Shader* shader);

	template <typename T> T* GetResource(std::string key);
	Shader* GetShaderResource(std::string key);
	void RemoveResource(std::string key);

	void InitializeResources(ID3D11Device* device);

	void ReadObjects(ID3D11Device* device);
	void ReadLights();
	void ReadShaders(ID3D11Device* device);

	void CompileShaders(ID3D11Device* device);
	Object* AssembleObject(std::string meshName, std::string materialName);

};

// Måste template funktioner ligga i .h filen?

template<typename T>
inline void ResourceManager::AddResource(std::string key, T* resource)
{
	auto iterator = resources.find(key);
	if (iterator == resources.end())
		resources.insert({ key, (void*)resource });
}

template<typename T>
inline T* ResourceManager::GetResource(std::string key)
{
	T* resource = nullptr;
	auto iterator = resources.find(key);
	
	if (iterator != resources.end())
	{
		void* item = (*iterator).second;
		resource = (T*)item;
	}

	return resource;
}
