#include "stdafx.h"
#include <algorithm>
#include "WorldGenerator.h"
#include "Engine.h"
#include "LightComponent.h"

WorldGenerator::WorldGenerator() : constructed(false), treePoints(dx::XMFLOAT2(0, 0), dx::XMFLOAT2(0, 0))
{
}

WorldGenerator::~WorldGenerator()
{
}

void WorldGenerator::Initialize(Object* root, World* world, Renderer* renderer)
{
	this->root = root;
	this->renderer = renderer;
	this->initialized = true;

	this->spawner = new ObjectSpawner();
	this->spawner->Initialize(root, world, renderer);

	ResourceManager* resources = Engine::Instance->GetResources();
	this->chunkShader = resources->GetShaderResource("terrainShader");
}

void WorldGenerator::Construct(const SaveState& state, const WorldDescription& description)
{
	assert(initialized);

	if (!constructed)
	{
		chunkMesh = GetChunkMesh(renderer->GetDevice());

		std::vector<dx::XMINT2> indexes;
		std::unordered_map<int, ChunkIndexInfo> chunks;

		dx::XMINT2 current(0, 0);
		dx::XMINT2 direction(0, 1);
		dx::XMINT2 minIndex, maxIndex;

		int steps = description.maxSteps;
		while (steps > 0)
		{
			for (size_t i = 0; i < UICAST(description.directionalSteps); i++)
			{
				current.x += direction.x;
				current.y += direction.y;

				indexes.push_back(dx::XMINT2(current));
				steps--;
			}
			UpdateDirection(direction);
		}

		AddChunksFromPath(indexes, chunks);
		AddPadding(CHUNK_PADDING, indexes, chunks, minIndex, maxIndex);

		//RegisterEnviromentProp("test", 0, 10, 4, [](Chunk* chunk, dx::XMVECTOR rootPosition)  
		//	{
		//		Object* root = new Object("puzzel_root");
		//		Object* puzzelModel = Engine::Instance->GetResources()->AssembleObject("Propane", "PropaneMaterial");
		//		Object::AddToHierarchy(root, puzzelModel);
		//		puzzelModel->GetTransform().SetLocalPosition({ CHUNK_SIZE / 2.0f, 5.0f, CHUNK_SIZE / 2.0f });
		//		puzzelModel->GetTransform().SetScale({ 10, 10, 10 });

		//		//dx::XMFLOAT3 pos;
		//		//dx::XMStoreFloat3(&pos, rootPosition);
		//		//std::cout << "PAAZZL: " << pos.x << ", " << pos.y << ", " << pos.z << "\n";

		//		return root;
		//	});

		// Ugly ass spawn for tree/frog puzzle
		RegisterEnviromentProp("PuzzleTree", 0, 10, 10, [](Chunk* chunk, dx::XMVECTOR rootPosition)
		{
			Object* root = new Object("puzzel_root");
			Object* tree =  Engine::Instance->GetResources()->AssembleObject("Tree", "TreeMaterial");

			Object* leaves = Engine::Instance->GetResources()->AssembleObject("leaves", "leavesMaterial");
			leaves->GetComponent<MeshComponent>()->GetMaterials()[0]->SetTransparent(true);
			Object* puzzle = Engine::Instance->GetResources()->AssembleObject("TreePuzzle", "TreePuzzleMaterial");
			Object* crazyFrog = Engine::Instance->GetResources()->AssembleObject("PuzzleFrogStatue", "PuzzleFrogStatueMaterial");
			Object* crazyFly = Engine::Instance->GetResources()->AssembleObject("PuzzleFlyStatue", "PuzzleFlyStatueMaterial");
			Object* frogHead = new Object("frogHead");

			tree->GetTransform().SetScale({ 2, 2, 2 });

			// bounding box for tree calculations
			Bounds bbInfo;
			bbInfo.CalculateAABB(tree->GetComponent<MeshComponent>()->GetMeshes());
			dx::XMFLOAT3 extends = bbInfo.GetExtends();
			extends.x *= 1.3;
			extends.y *= 2.0;
			extends.z *= 1.3;

			// For tree
			BoxColliderComponent* colliders = tree->AddComponent<BoxColliderComponent>(extends, dx::XMFLOAT3(0, 0, 0));
			RigidBodyComponent* trb = tree->AddComponent<RigidBodyComponent>(0.f, FilterGroups::DEFAULT, FilterGroups::EVERYTHING, BodyType::STATIC, true);

			// For puzzle
			puzzle->AddComponent<BoxColliderComponent>(dx::XMFLOAT3(2.018f, 0.1f, 0.969f), dx::XMFLOAT3(1.239f, 0.175f, 0.422f));
			puzzle->AddComponent<BoxColliderComponent>(dx::XMFLOAT3(0.764f, 0.1f, 2.175f), dx::XMFLOAT3(0.209f, 1.333f, 1.456f));
			puzzle->AddComponent<BoxColliderComponent>(dx::XMFLOAT3(1.956f, 0.1f, 0.745f), dx::XMFLOAT3(-1.253f, 2.259f, 0.51f));
			puzzle->AddComponent<BoxColliderComponent>(dx::XMFLOAT3(0.778f, 0.1f, 1.861f), dx::XMFLOAT3(-0.13f, 3.304f, -1.045f));
			puzzle->AddComponent<BoxColliderComponent>(dx::XMFLOAT3(2.083f, 0.1f, 0.68f), dx::XMFLOAT3(1.092f, 4.476f, -0.118f));
			puzzle->AddComponent<BoxColliderComponent>(dx::XMFLOAT3(0.6536f, 0.1f, 1.238f), dx::XMFLOAT3(0.259f, 5.515f, 1.238f));
			puzzle->AddComponent<BoxColliderComponent>(dx::XMFLOAT3(1.195f, 0.1f, 2.041f), dx::XMFLOAT3(-1.337f, 6.57f, 0.183f));
			RigidBodyComponent* prb = puzzle->AddComponent<RigidBodyComponent>(0.f, FilterGroups::PUZZLE, FilterGroups::EVERYTHING, BodyType::STATIC, true);

			// For fly
			crazyFly->AddComponent<BoxColliderComponent>(dx::XMFLOAT3(0.5f, 0.5f, 0.5f), dx::XMFLOAT3(0, 0.3f, 0));
			RigidBodyComponent* flyrb = crazyFly->AddComponent<RigidBodyComponent>(10.0f, FilterGroups::HOLDABLE, FilterGroups::EVERYTHING, BodyType::DYNAMIC, true);

			// For frog
			//crazyFrog->AddComponent<SphereColliderComponent>(1.685, dx::XMFLOAT3(0, -0.0f, 0));
			//crazyFrog->AddComponent<BoxColliderComponent>(dx::XMFLOAT3(2.f, 2.f, 2.2f), dx::XMFLOAT3(0, -0.3f, 0));
			crazyFrog->AddComponent<CapsuleColliderComponent>(2.0f, 1.6f, dx::XMFLOAT3(0, -2.0f, 0));
			RigidBodyComponent* frogrb = crazyFrog->AddComponent<RigidBodyComponent>(0.0f, FilterGroups::PROPS, FilterGroups::EVERYTHING, BodyType::STATIC, true);

			// For froghead
			frogHead->AddComponent<BoxColliderComponent>(dx::XMFLOAT3({ 1.158f, 0.435f, 1.871f }), dx::XMFLOAT3({ 0, 2.181f, -0.021f }));
			RigidBodyComponent* headrb = frogHead->AddComponent<RigidBodyComponent>(0.0f, FilterGroups::PUZZLE, FilterGroups::EVERYTHING, BodyType::STATIC, true);

			// Position for puzzle
			dx::XMVECTOR position(dx::XMVectorAdd(rootPosition, dx::XMVECTOR({ CHUNK_SIZE / 2.0f, 3.5f, CHUNK_SIZE / 2.0f })));

			Object::AddToHierarchy(chunk->GetOwner(), tree);
			Object::AddToHierarchy(tree, leaves);
			Object::AddToHierarchy(chunk->GetOwner(), puzzle);
			Object::AddToHierarchy(chunk->GetOwner(), crazyFrog);
			Object::AddToHierarchy(chunk->GetOwner(), crazyFly);
			Object::AddToHierarchy(chunk->GetOwner(), frogHead);

			dx::XMVECTOR frogpos = dx::XMVectorAdd(position, dx::XMVECTOR({ 4, 1, 4, 0 }));

			// Set the positions for RB
			trb->SetPosition(position);
			prb->SetPosition(position);
			frogrb->SetPosition(frogpos);
			flyrb->SetPosition(dx::XMVectorAdd(frogpos, dx::XMVECTOR({ 4, 0, 4, 0 })));
			headrb->SetPosition(dx::XMVectorAdd(frogpos, dx::XMVECTOR({ 0, 0.0f, 0, 0 })));

			// Testing printing shit to find puzzle
			dx::XMFLOAT3 pos;
			dx::XMStoreFloat3(&pos, position);
			std::cout << "PAAZZL: " << pos.x << ", " << pos.y << ", " << pos.z << "\n";

			return root;
		});

		path = Path(indexes);
		SetPathPointsToChunkType(path, chunks);

		treePoints.Clear();
		dx::XMFLOAT2 wmin = Chunk::IndexToWorldXZ(minIndex);
		dx::XMFLOAT2 wmax = Chunk::IndexToWorldXZ(maxIndex);
		wmax.x += CHUNK_SIZE;
		wmax.y += CHUNK_SIZE;
		treePoints.SetMinMax(wmin, wmax);

		AddEnvironmentProps(state.segment, description, minIndex, maxIndex, chunks);

		for (auto i : chunks)
		{
			CreateChunk(i.second, root, description);
		}

		Bounds worldBounds(dx::XMFLOAT3(wmin.x, 0, wmin.y), dx::XMFLOAT3(wmax.x, 0.0f, wmax.y));
		spawner->Spawn(state, worldBounds, chunkMap);

		std::vector<dx::XMFLOAT2> positions;
		std::vector<float> angles;
		path.GetLanternInformation(positions, angles);

		spawner->SpawnSpecific(positions, { 0,1,0 }, angles, "Lamp", chunkMap, [](Object* obj)
			{
				obj->GetTransform().SetScale({ 1.2f, 1.2f, 1.2f });

				Object* light = new Object("lantern_pointLight");
				light->GetTransform().SetPosition({ 1.0f, 3.0f, 0.f });
				LightComponent* lc = light->AddComponent<LightComponent>(LightType::POINT_LIGHT, dx::XMFLOAT4(0.5f, 0.5f, 0.0f, 1.0f), 5.0f);
				lc->SetEnabled(true);
				lc->SetIntensity(0.7f);
				Object::AddToHierarchy(obj, light);

			});

		constructed = true;
	}
}

void WorldGenerator::Deconstruct()
{
	if (constructed)
	{
		for (auto i : chunkMap)
		{
			i.second->PhysicRelease();
			Object::RemoveFromHierarchy(i.second->GetOwner());
			delete i.second;
		}

		spawner->Despawn();
		constructed = false;
		chunkMap.clear();
	}
}

void WorldGenerator::DrawDebug()
{
	path.DrawDebug();
	spawner->DrawDebug();
}

size_t WorldGenerator::RegisterEnviromentProp(std::string key, size_t minSegment, size_t maxSegment, size_t queueCount, PropFactory factory)
{
	size_t index = enviromentProps.size();

	WorldGenerator::EnviromentProp prop;
	prop.key = key;
	prop.factory = factory;
	prop.width = prop.height = 1;
	prop.minSegment = minSegment;
	prop.maxSegment = maxSegment;
	prop.onRoad = false;
	prop.usedCount = 0;

	for (size_t i = 0; i < queueCount; i++)
		enviromentProps.push_back(prop);

	return index;
}

void WorldGenerator::SetPathPointsToChunkType(const Path& path, std::unordered_map<int, ChunkIndexInfo>& chunkInformation)
{
	std::vector<LineSegment> segments = path.GetExtraSegments();
	const float walkLength = CHUNK_SIZE / 2.0f;

	for (size_t i = 0; i < segments.size(); i++)
	{
		float totalLength = segments[i].Length();
		float length = totalLength;

		while (length > 0.0f)
		{
			float t = length / totalLength;
			PathPoint point = segments[i].Lerp(t);

			int hash = Chunk::WorldToIndexHash(point.x, point.z);
			if (chunkInformation.find(hash) != chunkInformation.end())
				chunkInformation[hash].type = ChunkType::PATH;

			length -= walkLength;
		}

	}

}

void WorldGenerator::AddEnvironmentProps(const size_t& segmentIndex, const WorldDescription& description, dx::XMINT2 minIndex, dx::XMINT2 maxIndex, std::unordered_map<int, ChunkIndexInfo>& chunkInformation)
{
	if (enviromentProps.empty())
		return;

	// hitta alla props som passar in p� segment  INdex
	size_t nrOfProps = Random::Range(description.minEnviromentProps, description.maxEnviromentProps);	// flytta till description? 
	size_t FOUND = 0;

	std::vector<EnviromentProp> validProps;
	for (size_t i = 0; i < enviromentProps.size(); i++)
	{
		if (segmentIndex >= enviromentProps[i].minSegment && segmentIndex < enviromentProps[i].maxSegment)
			validProps.push_back(enviromentProps[i]);
	}

	if (!validProps.empty())
	{
		// sortera dom baserat p� usage  <- detta m�ste sparas sen
		std::sort(validProps.begin(), validProps.end(), SortProps);

		size_t indexesFound = 0;
		size_t maxTries = 1000;
		size_t propIndex = 0;

		while (indexesFound < nrOfProps && maxTries > 0)
		{
			int dx = Random::Range(minIndex.x, maxIndex.x);
			int dy = Random::Range(minIndex.y, maxIndex.y);
			int hash = HASH2D_I(dx, dy);

			if (chunkInformation.find(hash) != chunkInformation.end() && chunkInformation[hash].type == ChunkType::TERRAIN)
			{
				chunkInformation[hash].type = ChunkType::PUZZEL;
				chunkInformation[hash].propKey = validProps[propIndex].key;
				propIndex = (propIndex + 1) % validProps.size();
				indexesFound++;
				FOUND++;
			}

			maxTries--;
		}
	}

	const int a = 0;
}

bool WorldGenerator::SortProps(const EnviromentProp& propA, const EnviromentProp& propB)
{
	return propA.usedCount < propB.usedCount;
}

Chunk* WorldGenerator::CreateChunk(ChunkIndexInfo& indexInfo, Object* root, const WorldDescription& description)
{
	std::string name = "chunk " + std::to_string(indexInfo.index.x) + ", " + std::to_string(indexInfo.index.y);
	Object* chunkObject = new Object(name, ObjectFlag::RENDER);
	Chunk* chunk = chunkObject->AddComponent<Chunk>(indexInfo.index, indexInfo.type);

	dx::XMVECTOR chunkPosition = Chunk::IndexToWorld(indexInfo.index, 0.0f);
	chunkObject->GetTransform().SetPosition(chunkPosition);
	Object::AddToHierarchy(root, chunkObject);
	chunk->Create(description, path, renderer->GetDevice());

	Material* material = new Material(chunkShader);
	cb_Material mat;
	mat.ambient = dx::XMFLOAT4(0.5f, 0.5f, 0.5f, 1);
	mat.diffuse = dx::XMFLOAT4(0.5f, 0.5f, 0.5f, 1);
	mat.specular = dx::XMFLOAT4(0.1f, 0.1f, 0.1f, 0.1f);
	material->SetMaterialData(mat);

	if (indexInfo.type == ChunkType::PUZZEL)
	{
		EnviromentProp prop;
		
		for (size_t i = 0; i < enviromentProps.size(); i++)
		{
			if (enviromentProps[i].key == indexInfo.propKey)
			{
				prop = enviromentProps[i];
				break;
			}
		}

		Object* object = prop.factory(chunk, chunkPosition);
		Object::AddToHierarchy(chunkObject, object);
		prop.usedCount++;
	}

	ResourceManager* resources = Engine::Instance->GetResources();
	Texture* grassTexture = resources->GetResource<Texture>("Grass");
	Texture* roadTexture = resources->GetResource<Texture>("Road");

	material->SetTexture(chunk->GetData().dataTexture, 0, ShaderBindFlag::PIXEL | ShaderBindFlag::VERTEX);
	material->SetTexture(grassTexture, 1, ShaderBindFlag::PIXEL);
	material->SetTexture(roadTexture, 2, ShaderBindFlag::PIXEL);

	auto dataSampler = DXHelper::CreateSampler(D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_CLAMP, renderer->GetDevice());
	auto textureSampler = DXHelper::CreateSampler(D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_WRAP, renderer->GetDevice());
	material->SetSampler(dataSampler, 0, ShaderBindFlag::PIXEL | ShaderBindFlag::VERTEX);
	material->SetSampler(textureSampler, 1, ShaderBindFlag::PIXEL);

	Bounds bounds(dx::XMFLOAT3(0, 0, 0), dx::XMFLOAT3(CHUNK_SIZE, TERRAIN_SCALE + 1.0f, CHUNK_SIZE));
	chunkObject->AddComponent<MeshComponent>(chunkMesh, material, bounds);

	chunkMap.insert({ HASH2D_I(indexInfo.index.x, indexInfo.index.y), chunk });
	return chunk;
}

Mesh* WorldGenerator::GetChunkMesh(ID3D11Device* device)
{
	Mesh* mesh = nullptr;

	if (!hasChunkMesh)
	{
		// CREATE CHUNK MESH
		std::vector<Mesh::Vertex> vertices;
		std::vector<unsigned int> indicies;
		size_t indexCount = 0;
		Mesh::Vertex vertex;

		const size_t width = CHUNK_SIZE + 1;
		const size_t height = CHUNK_SIZE + 1;
		const size_t channels = 4;

		size_t bufferIndex = 0;

		for (size_t z = 0; z < CHUNK_SIZE + 1; z++)
		{
			for (size_t x = 0; x < CHUNK_SIZE + 1; x++)
			{
				float fx = static_cast<float>(x);
				float fz = static_cast<float>(z);

				//Vertex locations on x and y axis loaded here.
				vertex.position = { fx, 0, fz };
				vertex.uv = DirectX::XMFLOAT2(fx / static_cast<float>(CHUNK_SIZE), fz / static_cast<float>(CHUNK_SIZE));
				vertex.normal = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);
				vertex.tangent = DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f);

				vertices.push_back(vertex);

				if (z < CHUNK_SIZE && x < CHUNK_SIZE)
				{
					indicies.push_back((indexCount + CHUNK_SIZE + 1));	//TL
					indicies.push_back((indexCount + CHUNK_SIZE) + 2);	//TR
					indicies.push_back(indexCount + 1);					//BR
					indicies.push_back(indexCount + CHUNK_SIZE + 1);	//TL
					indicies.push_back(indexCount + 1);					//BR
					indicies.push_back(indexCount);						//BL
				}

				indexCount++;
			}
		}

		hasChunkMesh = true;
		mesh = new Mesh(vertices, indicies);
		mesh->Initialize(device);
	}
	else
	{
		mesh = chunkMesh;
	}

	return mesh;
}

void WorldGenerator::AddChunksFromPath(std::vector<dx::XMINT2>& path, std::unordered_map<int, ChunkIndexInfo>& chunks)
{
	size_t size = path.size();
	for (size_t i = 0; i < size; i++)
	{
		int index = HASH2D_I(path[i].x, path[i].y);
		ChunkIndexInfo info;
		info.index = path[i];
		info.type = ChunkType::PATH;

		if (i == 0)
			info.type = ChunkType::START;
		else if (i == size - 1)
			info.type = ChunkType::END;

		chunks.insert({ index, info });
	}
}

void WorldGenerator::AddPadding(int radius, std::vector<dx::XMINT2>& points, std::unordered_map<int, ChunkIndexInfo>& chunks, dx::XMINT2& minIndex, dx::XMINT2& maxIndex)
{
	int currentX, currentY;
	size_t size = points.size();

	for (size_t i = 0; i < size; i++)
	{
		if (i == 0)
			minIndex = maxIndex = points[i];

		for (int y = -radius; y <= radius; y++)
		{
			for (int x = -radius; x <= radius; x++)
			{
				if (x == 0 && y == 0)
					continue;

				currentX = points[i].x + x;
				currentY = points[i].y + y;

				minIndex.x = std::min(minIndex.x, currentX);
				minIndex.y = std::min(minIndex.y, currentY);
				maxIndex.x = std::max(maxIndex.x, currentX);
				maxIndex.y = std::max(maxIndex.y, currentY);

				int index = HASH2D_I(currentX, currentY);

				auto find = chunks.find(index);
				if (find == chunks.end())
				{
					ChunkIndexInfo info;
					info.index = dx::XMINT2(currentX, currentY);
					info.type = ChunkType::TERRAIN;

					chunks.insert({ index, info });
				}
			}
		}
	}
}

void WorldGenerator::UpdateDirection(dx::XMINT2& direction)
{
	if (direction.y == 0)
	{
		float value = Random::Value();

		// 50% att den g�r rakt vertikalt
		if (value < 0.5f)
		{
			direction.x = 0;
			direction.y = 1;
		}
		// 50% att den g�r helt �t sidan
		else if (value >= 0.5f)
		{
			direction.y = 1;
		}
	}
	else
	{
		// 50% om den byter riktning i x
		direction.x = (Random::Value() < 0.5f) ? -1 : 1;

		// 50% om den b�rjar g� horizontal
		if (Random::Value() < 0.5f)
		{
			direction.y = 0;
		}
	}
}