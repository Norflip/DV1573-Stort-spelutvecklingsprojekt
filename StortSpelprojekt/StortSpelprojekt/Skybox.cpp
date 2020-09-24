#include "Skybox.h"
#include "Skybox.h"

Skybox::Skybox(Object* object)
{
	std::vector<Mesh> skybox = ZWEBLoader::LoadMeshes(ZWEBLoadType::NoAnimation, "Models/brickSphere.ZWEB", renderer->GetDevice());
	Material newmat;

	object->AddComponent<MeshComponent>(skybox[0], newmat);
}

Skybox::~Skybox()
{
}

void Skybox::SetDiffuse(Object* object, dx::XMFLOAT4 diffuse)
{
			
}
