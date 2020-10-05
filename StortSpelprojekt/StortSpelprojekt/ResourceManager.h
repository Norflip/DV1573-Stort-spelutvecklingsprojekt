#pragma once
#include <unordered_map>
#include <string>
#include <fstream>
#include "ZWEBLoader.h"

class ResourceManager
{
private:
	std::unordered_map<std::string, void*> resources;
	int nrOfShaders;
public:
	ResourceManager();
	~ResourceManager();

	template <typename T> void AddResource(std::string key, T* resource);
	template <typename T> T* GetResource(std::string key);
	void RemoveResource(std::string key);
	void InitializeResources(ID3D11Device* device);
	void ReadObjects(ID3D11Device* device);
	void ReadLights();
	void ReadShaders(ID3D11Device* device);

	template <typename T> void CompileShaders(ID3D11Device* device);
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

//template<typename T>
//inline void ResourceManager::CompileShaders(ID3D11Device* device)
//{
//	int counter = 0;
//
//	/*for (std::pair<std::string, void*> element : resources)
//	{
//
//	}*/
//	/*std::for_each(resource.begin(), resources.end(), [](std::pair<std::string, T> element))
//	{
//		Shader* temp = (T*)element.second();
//
//		counter++;
//
//		if (counter == nrOfShaders - 1)
//		{
//			break;
//		}
//	}*/
//
//}