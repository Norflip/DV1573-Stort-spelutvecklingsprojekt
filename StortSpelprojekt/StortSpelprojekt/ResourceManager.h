#pragma once

class ResourceManager
{
private:

	// Maps for different resources and shaders
	std::unordered_map<std::string, Resource*> resources;
	std::unordered_map<std::string, Shader*> shaderResources;

public:
	ResourceManager();
	virtual ~ResourceManager();

	void AddResource(std::string key, Resource* resource);
	void AddShaderResource(std::string key, Shader* shader);

	Resource* GetResource(std::string key);
	template <typename T> T* GetResource(std::string key);
	Shader* GetShaderResource(std::string key);
	void RemoveResource(std::string key);

	void InitializeResources(ID3D11Device* device);

	void ReadObjects(ID3D11Device* device);
	void ReadTextures(ID3D11Device* device);
	void ReadShaders(ID3D11Device* device);

	void CompileShaders(ID3D11Device* device);
	Object* AssembleObject(std::string meshName, std::string materialName);
};

// Måste template funktioner ligga i .h filen?

template<typename T>
inline T* ResourceManager::GetResource(std::string key)
{
	Resource* resource = GetResource(key);
	return static_cast<T*>(resource);
}
