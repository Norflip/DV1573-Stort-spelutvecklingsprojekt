#pragma once

class ResourceManager
{
private:

	// Maps for different resources and shaders
	std::unordered_map<std::string, Resource*> resources;
	std::unordered_map<std::string, Shader*> shaderResources;

	void ReadObjects(ID3D11Device* device);
	void ReadTextures(ID3D11Device* device);
	void ReadShaders(ID3D11Device* device);
	void ReadAnimations(ID3D11Device* device);

	Resource* GetResource(std::string key);

	Texture* emptyTexture;
	
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

	// Remove a resource from the maps given the specific key
	void RemoveResource(std::string key);

	// Initialize resources, will read its private functions
	void InitializeResources(ID3D11Device* device);

	Texture* GetEmptyTexture() { return this->emptyTexture; }

	// Recompile all the shaders inside the shaderResources
	void CompileShaders(ID3D11Device* device);
	Object* AssembleObject(std::string meshName, std::string materialName, bool batchable = false, ObjectFlag flag = ObjectFlag::DEFAULT);
};

// Template function to get a resource from the unordered_map
template<typename T>
inline T* ResourceManager::GetResource(std::string key)
{
	Resource* resource = GetResource(key);
	assert(resource != nullptr);
	return static_cast<T*>(resource);
}
