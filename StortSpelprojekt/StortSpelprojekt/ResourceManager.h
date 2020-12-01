#pragma once

class ResourceManager
{
private:

	// Maps for different resources and shaders
	std::unordered_map<std::string, Resource*> resources;
	std::unordered_map<std::string, Shader*> shaderResources;
	HMODULE handle = NULL;

	void ReadObjects(ID3D11Device* device);
	void ReadTextures(ID3D11Device* device);
	void ReadShaders(ID3D11Device* device);
	void ReadAnimations(ID3D11Device* device);

#if LOAD_FROM_DLL

	std::unordered_map<std::string, int> dllResourceMap;
	std::unordered_map<std::string, unsigned char*> dllTextureCache;
#endif



	Resource* GetResource(std::string key);

public:
	ResourceManager();
	virtual ~ResourceManager();

	// Add resource to the resources unordered_map 
	void AddResource(std::string key, Resource* resource);

	// Add a shader resource to the shaderResource unordered_map
	void AddShaderResource(std::string key, Shader* shader);

	// Return a resource given the specific key
	template <typename T> T* GetResource(std::string key);

	// Return a shader resource given the specific key
	Shader* GetShaderResource(std::string key);

	HMODULE& GetHMODULE() { return this->handle; }
	const HMODULE& GetHMODULE() const { return this->handle; }

	// Remove a resource from the maps given the specific key
	void RemoveResource(std::string key);

	// Initialize resources, will read its private functions
	void InitializeResources(ID3D11Device* device);

	// Recompile all the shaders inside the shaderResources
	void CompileShaders(ID3D11Device* device);
	Object* AssembleObject(std::string meshName, std::string materialName, bool batchable = false, ObjectFlag flag = ObjectFlag::DEFAULT);

#if LOAD_FROM_DLL

	std::wstring DLLGetShaderData(std::string path, size_t& size);
	unsigned char* DLLGetTextureData(std::string path, size_t& size);
	//unsigned char* DLLGetSoundData(std::string path, size_t& size);

#endif

};

// Template function to get a resource from the unordered_map
template<typename T>
inline T* ResourceManager::GetResource(std::string key)
{
	Resource* resource = GetResource(key);
	assert(resource != nullptr);
	return static_cast<T*>(resource);
}
