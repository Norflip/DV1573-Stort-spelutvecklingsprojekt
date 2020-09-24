#pragma once
#include <vector>
#include "Object.h"
#include "ShittyOBJLoader.h"
#include "Input.h"
#include "CameraComponent.h"
#include "MoveComponent.h"
#include "ControllerComponent.h"
#include "MeshComponent.h"
#include "ZWEBLoader.h"
#include "SkeletonMeshComponent.h"


class Scene
{
public:
	Scene();
	virtual ~Scene();

	void Initialize(Renderer* renderer);
	
	void OnActivate() {}
	void OnDeactivate() {}
	
	void Update(const float& deltaTime);
	void FixedUpdate(const float& fixedDeltaTime);
	void Render();

	/* new - render scene to texture */
	void RenderSceneToTexture();

	void PrintSceneHierarchy() const;
	void PrintSceneHierarchy(Object* object, size_t level) const;

	
private:	
	std::vector<Object*> objects;
	CameraComponent* camera;
	//MoveComponent* move;
	ControllerComponent* move;
	Renderer* renderer;

	Input& input;

	/* Test stuff */
	Texture* screenquadTex;
	Object* quad;
	Material screenquadmat;	


	//frustumplanes extraction
	std::vector<dx::XMFLOAT4> extractedPlanes;
	dx::XMFLOAT3 tempPos;

};