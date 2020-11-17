#include "stdafx.h"
#include "WorldGenerator.h"
#include "Engine.h"

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

		path = Path(indexes);
		
		treePoints.Clear();
		dx::XMFLOAT2 wmin = Chunk::IndexToWorldXZ(minIndex);
		dx::XMFLOAT2 wmax = Chunk::IndexToWorldXZ(maxIndex);
		wmax.x += CHUNK_SIZE;
		wmax.y += CHUNK_SIZE;
		treePoints.SetMinMax(wmin, wmax);
		
		for (auto i : chunks)
		{
			ChunkIndexInfo indexInfo = i.second;
			CreateChunk(indexInfo.index, root, description, indexInfo.type);
		}

		Bounds worldBounds(dx::XMFLOAT3(wmin.x, 0, wmin.y), dx::XMFLOAT3(wmax.x, 0.0f, wmax.y));
		spawner->Spawn(state, worldBounds, chunkMap);

		std::vector<dx::XMFLOAT2> positions;
		std::vector<float> angles;
		path.GetLanternInformation(positions, angles);

		spawner->SpawnSpecific(positions, { 0,1,0 }, angles, "Lamp", chunkMap, [](Object* obj)
		{
			obj->GetTransform().SetScale({ 1.2f, 1.2f, 1.2f });



			/*Object* light = new Object("lantern_pointLight");
			light->GetTransform().SetPosition({ 0,2,0 });
			light->AddComponent<PointLightComponent>(dx::XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f), 4.0f);
			Transform::SetParentChild(obj->GetTransform(), light->GetTransform());*/

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
			Transform::ClearFromHierarchy(i.second->GetOwner()->GetTransform());
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

Chunk* WorldGenerator::CreateChunk(const dx::XMINT2& index, Object* root, const WorldDescription& description, ChunkType type)
{
	std::string name = "chunk " + std::to_string(index.x) + ", " + std::to_string(index.y);
	Object* chunkObject = new Object(name, ObjectFlag::RENDER);
	Chunk* chunk = chunkObject->AddComponent<Chunk>(index, type);

	chunkObject->GetTransform().SetPosition(Chunk::IndexToWorld(index, 0.0f));
	Transform::SetParentChild(root->GetTransform(), chunkObject->GetTransform());
	chunk->Create(description, path, renderer->GetDevice());

	Material* material = new Material(chunkShader);
	cb_Material mat;
	mat.ambient = dx::XMFLOAT4(0.5f, 0.5f, 0.5f, 1);
	mat.diffuse = dx::XMFLOAT4(0.5f, 0.5f, 0.5f, 1);
	mat.specular = dx::XMFLOAT4(0.1f, 0.1f, 0.1f, 0.1f);
	material->SetMaterialData(mat);

	//Texture texture(chunkDataSRV);

	Texture* grassTexture = Texture::LoadTexture(renderer->GetDevice(), L"Textures/newGrass.png");
	Texture* roadTexture = Texture::LoadTexture(renderer->GetDevice(), L"Textures/Stone_Floor_003_COLOR.jpg");

	material->SetTexture(chunk->GetData().dataTexture, 0, ShaderBindFlag::PIXEL | ShaderBindFlag::VERTEX);
	material->SetTexture(grassTexture, 1, ShaderBindFlag::PIXEL);
	material->SetTexture(roadTexture, 2, ShaderBindFlag::PIXEL);

	auto dataSampler = DXHelper::CreateSampler(D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_CLAMP, renderer->GetDevice());
	material->SetSampler(dataSampler, 0, ShaderBindFlag::PIXEL | ShaderBindFlag::VERTEX);

	auto textureSampler = DXHelper::CreateSampler(D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_WRAP, renderer->GetDevice());
	material->SetSampler(textureSampler, 1, ShaderBindFlag::PIXEL);

	Bounds bounds (dx::XMFLOAT3(0, 0, 0), dx::XMFLOAT3(CHUNK_SIZE, TERRAIN_SCALE + 1.0f, CHUNK_SIZE));
	chunkObject->AddComponent<MeshComponent>(chunkMesh, material, bounds);

	int i = HASH2D_I(index.x, index.y);
	chunkMap.insert({ i, chunk });
	
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

		// 50% att den går rakt vertikalt
		if (value < 0.5f)
		{
			direction.x = 0;
			direction.y = 1;
		}
		// 50% att den går helt åt sidan
		else if (value >= 0.5f)
		{
			direction.y = 1;
		}
	}
	else
	{
		// 50% om den byter riktning i x
		direction.x = (Random::Value() < 0.5f) ? -1 : 1;

		// 50% om den börjar gå horizontal
		if (Random::Value() < 0.5f)
		{
			direction.y = 0;
		}
	}
}