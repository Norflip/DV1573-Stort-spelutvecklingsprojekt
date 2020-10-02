#pragma once
#include <map>
#include <string>
#include <fstream>
#include "ZWEBLoader.h"

class ResourceManager
{
private:
	std::map<std::string, void*> resources;
public:
	ResourceManager();
	~ResourceManager();

	template <typename T> void AddResource(std::string key, T* resource);
	template <typename T> T* GetResource(std::string key);
	void RemoveResource(std::string key);
	void InitializeResources(ID3D11Device* device);
	void ReadObjects(ID3D11Device* device);
	void ReadLights();
	void ReadShaders();

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
