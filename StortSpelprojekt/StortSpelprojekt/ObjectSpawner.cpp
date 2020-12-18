#include "stdafx.h"
#include "ObjectSpawner.h"
#include "ResourceManager.h"
#include "Engine.h"
#include <random>
#include "MeshCollider.h"
#include "World.h"

ObjectSpawner::ObjectSpawner() : environmentQT(nullptr)
{
	doOnce = true;
}

ObjectSpawner::~ObjectSpawner()
{
	RELEASE(grassBfr);
	RELEASE(grassSrv);
	RELEASE(grassIndexBfr);
	RELEASE(grassIndexSrv);
	RELEASE(grassBCBfr);
	RELEASE(grassBCSRV);

	if (pooler)
		delete pooler;
	if (environmentQT)
		delete environmentQT;
}

void ObjectSpawner::Initialize(Object* root, World* world, ItemManager* items, Renderer* renderer)
{
	ResourceManager* resources = Engine::Instance->GetResources();
	this->pooler = new ObjectPooler(resources);
	this->root = root;
	this->renderer = renderer;
	this->world = world;
	this->items = items;

	// DEFAULT TREE
	baseTreeModel.meshes.push_back(resources->GetResource<Mesh>("instancedTree"));
	baseTreeModel.meshes.push_back(resources->GetResource<Mesh>("instancedleaves"));

	baseTreeModel.materials.push_back(resources->GetResource<Material>("instancedTreeMaterial"));

	Material* mat = resources->GetResource<Material>("instancedleavesMaterial");
	mat->SetShader(resources->GetShaderResource("leafShader"));

	baseTreeModel.materials.push_back(mat);
	baseTreeModel.materials[1]->SetTransparent(true);

	baseTreeModel.minScale = 1.4f;
	baseTreeModel.maxScale = 2.0f;
	baseTreeModel.minRotation = 5.0f;
	baseTreeModel.maxRotation = 15.0f;

	baseTreeModel.discRadius = 3.0f; // with padding
	baseTreeModel.segmentScaleFactor = 0.7f;
	
}

void ObjectSpawner::Spawn(const SaveState& state, const Bounds& worldBounds, std::unordered_map<int, Chunk*>& chunkMap)
{
	QuadTree tree(worldBounds);

	if(environmentQT != nullptr)
		delete environmentQT;

	environmentQT = new QuadTree(worldBounds);

	for (auto i : chunkMap)
	{
		SpawnStatic(i.second);

		AddTreesToChunk(baseTreeModel, i.second, state.segment);
		AddGrassToChunk(i.second);

#if SPAWN_ITEMS
		SpawnItem(i.second);
#endif
	}
}

void ObjectSpawner::Despawn()
{
	items->DespawnAll();
}

void ObjectSpawner::SpawnSpecific(std::vector<dx::XMFLOAT2> positions, dx::XMVECTOR axis, std::vector<float> angles, std::string modelName, std::unordered_map<int, Chunk*>& chunkMap, std::function<void(Object*)> modifier)
{
	for (size_t i = 0; i < positions.size(); i++)
	{
		Chunk* chunk = world->GetChunk(positions[i].x, positions[i].y);

		if (chunk != nullptr)
		{
			Object* obj = Engine::Instance->GetResources()->AssembleObject(modelName, modelName + "Material", false);
			Object::AddToHierarchy(chunk->GetOwner(), obj);

			float height = chunk->SampleHeight(positions[i].x, positions[i].y);
			obj->GetTransform().SetWorldPosition({ positions[i].x, height, positions[i].y });
			obj->GetTransform().SetWorldRotation(dx::XMQuaternionRotationNormal(axis, angles[i]));

			modifier(obj);
		}
	}
}

//void ObjectSpawner::RegisterItem(std::string key, size_t queueCount, std::function<Object* (ResourceManager*)> factory)
//{
//	Item item;
//	item.key = key;
//	pooler->Register(key, 0, factory);
//
//	for (size_t i = 0; i < queueCount; i++)
//		itemRegistry.push_back(item);
//}

void ObjectSpawner::RegisterInstancedItem(std::string key, float yOffsetFactor, size_t queueCount, dx::XMUINT3 rotationAxis)
{
	ResourceManager* resources = Engine::Instance->GetResources();
	RegisterInstancedItem(resources->GetResource<Mesh>(key), resources->GetResource<Material>(key + "Material"), yOffsetFactor, queueCount, rotationAxis);
}

void ObjectSpawner::RegisterInstancedItem(Mesh* mesh, Material* material, float yOffset, size_t queueCount, dx::XMUINT3 rotationAxis)
{
	assert(rotationAxis.x == 0 || rotationAxis.x == 1);
	assert(rotationAxis.y == 0 || rotationAxis.y == 1);
	assert(rotationAxis.z == 0 || rotationAxis.z == 1);
	assert(mesh != nullptr);
	assert(material != nullptr);
	assert(queueCount > 0);

	Bounds bounds;
	bounds.CalculateAABB(mesh);

	Prop prop;
	prop.mesh = mesh;
	prop.material = material;
	prop.yOffset = yOffset;
	prop.queueCount = queueCount;
	prop.bounds = bounds;
	prop.randomRotationAxis = rotationAxis;
	instancedProps.push_back(prop);
}

void ObjectSpawner::DrawDebug()
{
	return;
	for (size_t i = 0; i < itemSpawnPositions.size(); i++)
	{
		dx::XMFLOAT3 worldPoint(itemSpawnPositions[i].x, 5, itemSpawnPositions[i].y);
		DShape::DrawSphere(worldPoint, 0.5f, dx::XMFLOAT3(1, 1, 0));
	}

	for (size_t i = 0; i < propSpawnPositions.size(); i++)
	{
		dx::XMFLOAT3 worldPoint(propSpawnPositions[i].x, 5, propSpawnPositions[i].y);
		DShape::DrawSphere(worldPoint, 0.5f, dx::XMFLOAT3(0, 0, 1));
	}
}

void ObjectSpawner::UpdateGrass(ID3D11DeviceContext* context)
{
	if (grassSrv)
	{
		ShaderBindFlag flag = ShaderBindFlag::VERTEX | ShaderBindFlag::HULL | ShaderBindFlag::DOMAINS;
		DXHelper::BindStructuredBuffer(context, GRASS_STRAWS_SRV_SLOT, flag, &grassSrv);
		DXHelper::BindStructuredBuffer(context, GRASS_INDICES_SRV_SLOT, flag, &grassIndexSrv);
		DXHelper::BindStructuredBuffer(context, GRASS_COORD_SRV_SLOT, ShaderBindFlag::DOMAINS, &grassBCSRV);
	}
}


void ObjectSpawner::AddTreesToChunk(const TreeModel& treeModel, Chunk* chunk, size_t segment) const
{
	float discradius = treeModel.discRadius * (1.0f + (segment * treeModel.segmentScaleFactor * 0.5f));
	std::vector<dx::XMFLOAT2> points = PossionDiscSampler::Sample(discradius, CHUNK_SIZE, CHUNK_SIZE);

	if (points.size() > 0)
	{
		std::vector<dx::XMFLOAT2> validPoints;

		for (size_t i = 0; i < points.size(); i++)
		{
			if (Random::Value() > NO_TREE_CHANCE && ValidSpawnPoint(points[i], chunk, TREE_SPAWN_FACTOR))
				validPoints.push_back(points[i]);
		}

		size_t nrOfInstancedStyTrees = validPoints.size();

		if (nrOfInstancedStyTrees > 0)
		{
			std::vector<dx::XMFLOAT4X4> treesInstanced(nrOfInstancedStyTrees);
			dx::XMFLOAT2 posXZ = Chunk::IndexToWorldXZ(chunk->GetIndex());

			Bounds bbInfo;
			bbInfo.CalculateAABB(baseTreeModel.meshes[0]);
			dx::XMFLOAT3 extends = bbInfo.GetExtends();

			std::vector<dx::XMFLOAT3> colliderPositions;
			std::vector<dx::XMFLOAT3> colliderExtends;
			std::vector<dx::XMFLOAT4> colliderRotations;

			for (size_t i = 0; i < nrOfInstancedStyTrees; i++)
			{
				float y = chunk->SampleHeight(posXZ.x + validPoints[i].x, posXZ.y + validPoints[i].y) - TREE_HEIGHT_ADJUSTMENT_FACTOR;
				environmentQT->Insert(dx::XMFLOAT2(posXZ.x + validPoints[i].x, posXZ.y + validPoints[i].y), TREE_RADIUS);

				dx::XMFLOAT3 position(posXZ.x + validPoints[i].x, y, posXZ.y + validPoints[i].y);

				dx::XMFLOAT3 normal;
				world->SampleNormal(position.x, position.z, normal);

				float angleRotation = Random::Range(baseTreeModel.minRotation, baseTreeModel.maxRotation) * Math::ToRadians;

				dx::XMVECTOR normalAngleRotation = dx::XMQuaternionRotationNormal(dx::XMLoadFloat3(&normal), angleRotation);
				dx::XMVECTOR randomYRotation = dx::XMQuaternionRotationAxis({ 0,1,0 }, Random::RadAngle());

				//dx::XMMATRIX xzRotation = dx::XMMatrixRotationNormal(dx::XMLoadFloat3(&normal), angleRotation);
				//dx::XMMATRIX yRotation = dx::XMMatrixRotationQuaternion(dx::XMQuaternionRotationAxis({ 0,1,0 }, Random::RadAngle()));
				//dx::XMMATRIX rotation = dx::XMMatrixMultiply(yRotation, xzRotation);

				dx::XMVECTOR combinedRot = dx::XMQuaternionMultiply(randomYRotation, normalAngleRotation);
				dx::XMMATRIX rotation = dx::XMMatrixRotationQuaternion(combinedRot);

				float scale = Random::Range(treeModel.minScale, treeModel.maxScale);
				scale *= 1.0f + (segment * treeModel.segmentScaleFactor);
				dx::XMMATRIX translation = dx::XMMatrixScaling(scale, scale, scale) * rotation * dx::XMMatrixTranslation(position.x, position.y, position.z);
				dx::XMStoreFloat4x4(&treesInstanced[i], dx::XMMatrixTranspose(translation));

				dx::XMFLOAT3 scaledExtends = dx::XMFLOAT3(extends.x * scale * 0.85f, extends.y * scale * 0.85f, extends.z * scale * 0.85f);

				position.y += 3.0f;
				colliderPositions.push_back(position);
				colliderExtends.push_back(scaledExtends);

				dx::XMFLOAT4 rot;
				dx::XMStoreFloat4(&rot, combinedRot);
				colliderRotations.push_back(rot);
			}

			Object* treeObject = new Object("tree", ObjectFlag::DEFAULT | ObjectFlag::NO_CULL);

			Object::AddToHierarchy(chunk->GetOwner(), treeObject);

			treeObject->GetTransform().SetPosition({ 0,0,0 });
			treeObject->GetTransform().SetWorldPosition({ 0,0,0 });

			MeshComponent* meshComp = treeObject->AddComponent<MeshComponent>(baseTreeModel.meshes, baseTreeModel.materials);
			meshComp->SetInstanceable(treesInstanced, nrOfInstancedStyTrees, renderer->GetDevice());

			BoxColliderComponent* colliders = treeObject->AddComponent<BoxColliderComponent>(colliderExtends, colliderPositions);
			for (size_t i = 0; i < nrOfInstancedStyTrees; i++)
				colliders->SetRotation(i, colliderRotations[i]);

			treeObject->AddComponent<RigidBodyComponent>(0.f, FilterGroups::DEFAULT, FilterGroups::EVERYTHING, BodyType::STATIC, true);

		}
	}
}

void ObjectSpawner::AddGrassToChunk(Chunk* chunk)
{
	Mesh* chunkMesh = chunk->GetOwner()->GetComponent<MeshComponent>()->GetMeshes()[0];
	Shader* shader = Engine::Instance->GetResources()->GetShaderResource("grassShader");
	assert(chunkMesh != nullptr);
	assert(shader != nullptr);

	Bounds bounds;
	bounds.SetMinMax(dx::XMFLOAT3(0, 0, 0), dx::XMFLOAT3(CHUNK_SIZE, TERRAIN_SCALE + 1.0f, CHUNK_SIZE));

	GrassComponent* grassComponent = chunk->GetOwner()->AddComponent<GrassComponent>(chunkMesh->GetTriangleCount(), renderer->GetDevice(), shader, bounds);
	grassComponent->GetMaterial()->SetTexture(chunk->GetData()->dataTexture, 6, ShaderBindFlag::HULL);
	grassComponent->GetMaterial()->SetTexture(chunk->GetData()->dataTexture, 6, ShaderBindFlag::DOMAINS);
	if (doOnce)
	{
		grassComponent->InitOnce(chunkMesh, renderer->GetDevice(), &grassBfr, &grassSrv, &grassIndexBfr, &grassIndexSrv, &grassBCBfr, &grassBCSRV);

		doOnce = false;
	}
	grassComponent->InitializeGrass(chunkMesh, renderer->GetDevice(), renderer->GetContext(), &grassBfr, &grassSrv, &grassIndexBfr, &grassIndexSrv, &grassBCBfr, &grassBCSRV);
}

bool ObjectSpawner::ValidSpawnPoint(const dx::XMFLOAT2& point, Chunk* chunk, float minInfluence) const
{
	int dx = static_cast<int>(roundf(point.x));
	int dy = static_cast<int>(roundf(point.y));

	bool valid = false;
	if (dx + dy * (CHUNK_SIZE + 1) < (CHUNK_SIZE + 1) * (CHUNK_SIZE + 1))
	{
		float v = chunk->GetData()->influenceMap[dx + dy * (CHUNK_SIZE + 1)];
		valid = chunk->GetType() != ChunkType::PUZZEL && chunk->GetType() != ChunkType::PATH && v > minInfluence;
	}

	return valid;
}

static float spawnChance = 0.0f;
static float spawnIncrement = 0.2f;
static float spawnDecrement = 0.15f;

void ObjectSpawner::SpawnStatic(Chunk* chunk)
{
	const float SPAWN_RADIUS = 10.0f;
	std::vector<dx::XMFLOAT2> points = PossionDiscSampler::Sample(SPAWN_RADIUS, CHUNK_SIZE, CHUNK_SIZE);

	if (chunk->GetType() != ChunkType::TERRAIN)
		return;


	static size_t MAX_TRIES = 12;
	size_t nrToSpawn = UICAST(Random::Range(1, (3 + 1)));	// 1 - 3

	for (size_t nr = 0; nr < nrToSpawn; nr++)
	{
		if (Random::Value() > spawnChance)
		{
			size_t propIndex = UICAST(Random::Range(0, instancedProps.size()));
			Prop& prop = instancedProps[propIndex];
			Bounds b = prop.bounds;

			size_t tries = 0;
			while (tries < MAX_TRIES)
			{
				float x = Random::Value() * FCAST(CHUNK_SIZE);
				float z = Random::Value() * FCAST(CHUNK_SIZE);

				dx::XMFLOAT3 chunkPosition;
				dx::XMStoreFloat3(&chunkPosition, chunk->GetOwner()->GetTransform().GetWorldPosition());
				float dx = x + chunkPosition.x;
				float dz = z + chunkPosition.z;

				dx::XMFLOAT2 point(dx, dz);
				const float radius = prop.bounds.GetRadiusXZ();

				if (ValidSpawnPoint(dx::XMFLOAT2(x, z), chunk, 0.9f) && environmentQT->CountInRange(point, radius) == 0)
				{
					spawnChance -= spawnDecrement;
					environmentQT->Insert(point, radius * 1.5f);	// small padding by 50%

					float y = chunk->SampleHeight(dx, dz) - (prop.bounds.GetSize().y * (1.0f - prop.yOffset));

					Object* props = new Object("props_" + prop.mesh->GetMeshName(), ObjectFlag::DEFAULT);
					Object::AddToHierarchy(chunk->GetOwner(), props);

					dx::XMFLOAT3 sampleNormal = chunk->SampleNormal(dx, dz);

					dx::XMVECTOR randomYRotation = dx::XMQuaternionRotationAxis(dx::XMLoadFloat3(&sampleNormal), Random::RadAngle());

					props->GetTransform().SetWorldPosition({ dx, y, dz });
					props->GetTransform().SetWorldRotation(randomYRotation);

					MeshComponent* mc = props->AddComponent<MeshComponent>(prop.mesh, prop.material);
					mc->SetBatchable(true);

					//dx::XMStoreFloat4x4(&wat, dx::XMMatrixTranspose(dx::XMMatrixRotationQuaternion(randomYRotation) * dx::XMMatrixTranslation(dx, y, dz)));
					//mc->SetInstanceable(instancedData, instancedData.size(), renderer->GetDevice());

					dx::XMFLOAT3 tmpPos;
					dx::XMStoreFloat3(&tmpPos, props->GetTransform().GetLocalPosition());

					MeshCollider* meshCollider = props->AddComponent<MeshCollider>(prop.mesh, dx::XMFLOAT3(0, 0, 0));
					//props->AddComponent<BoxColliderComponent>(dx::XMFLOAT3({ 0.5, 0.5, 0.5 }), dx::XMFLOAT3({ 0, 0, 0 }));

					dx::XMFLOAT4 rot;
					dx::XMStoreFloat4(&rot, randomYRotation);
					//meshCollider->SetRotation(0, rot);

					props->AddComponent<RigidBodyComponent>(0.f, FilterGroups::PROPS, FilterGroups::EVERYTHING, BodyType::STATIC, true);
					break;
				}

				tries++;
			}
		}
		else
		{
			spawnChance += spawnIncrement;
		}
	}

	spawnChance = Math::Clamp01(spawnChance);
}

static size_t m_itemIndex = 0;
void ObjectSpawner::SpawnItem(Chunk* chunk)
{
	if (chunk->GetType() == ChunkType::TERRAIN)
	{
		dx::XMINT2 index = chunk->GetIndex();
		PickupType priorityType = PickupType::Food;
		if (index.x & 2 == 0 && index.y & 2 == 0)
			priorityType = PickupType::Fuel;
		
		const size_t MIN_ITEMS_PER_CHUNK = 2;
		const size_t MAX_ITEMS_PER_CHUNK = 4;

		std::vector<dx::XMFLOAT2> positions = PossionDiscSampler::Sample(2.0f, CHUNK_SIZE, CHUNK_SIZE);

		if (positions.size() > 0)
		{
			ShuffleVector(positions);
			size_t spawnCount = 0;
			size_t ITEMS_PER_CHUNK = UICAST(Random::Range(ICAST(MIN_ITEMS_PER_CHUNK), ICAST(MAX_ITEMS_PER_CHUNK + 1)));

			for (size_t i = 0; i < positions.size() && spawnCount < ITEMS_PER_CHUNK; i++)
			{
				dx::XMFLOAT2 pos = positions[i];
				dx::XMFLOAT3 chunkPosition;
				dx::XMStoreFloat3(&chunkPosition, chunk->GetOwner()->GetTransform().GetWorldPosition());
				pos.x += chunkPosition.x;
				pos.y += chunkPosition.z;

				if (environmentQT->CountInRange(pos, 1.0f) == 0)
				{
					float y = chunk->SampleHeight(pos.x, pos.y);

					dx::XMVECTOR position = dx::XMVectorSet(pos.x, y + 1.0f, pos.y, 0.0f);
					Object* object = nullptr;

					if (i == 0)
						object = items->SpawnRandomOfType(priorityType, position, root);
					else
						object = items->SpawnRandom(position, root);

					spawnCount++;
				}
			}
		}

	}
}

std::vector<dx::XMFLOAT2> ObjectSpawner::CreateSpawnPositions(QuadTree* tree, float spawnRadius, float itemRadius, std::unordered_map<int, Chunk*>& chunkMap) const
{
	dx::XMFLOAT2 min = tree->GetMin();
	dx::XMFLOAT2 max = tree->GetMax();

	float width = max.x - min.x;
	float height = max.y - min.y;

	std::vector<dx::XMFLOAT2> points = PossionDiscSampler::Sample(spawnRadius, width, height);
	std::vector<dx::XMFLOAT2> validPoints;
	size_t fails = 0;
	size_t cols = 0;

	for (size_t i = 0; i < points.size(); i++)
	{
		dx::XMFLOAT2 point = points[i];
		point.x += min.x;
		point.y += min.y;

		if (tree->CountInRange(point, itemRadius) == 0)
		{
			Chunk* chunk = world->GetChunk(point.x, point.y);

			if (chunk != nullptr && chunk->GetType() == ChunkType::TERRAIN)
			{
				//tree.Insert(point);
				validPoints.push_back(point);
			}
			else
			{
				fails++;
			}
		}
		else
		{
			cols++;
		}
	}

	/*std::cout << "VALID: " << validPoints.size() << std::endl;
	std::cout << "FAILS: " << fails << std::endl;
	std::cout << "COLS: " << cols << std::endl;*/

	ShuffleVector(validPoints);
	return validPoints;
}
