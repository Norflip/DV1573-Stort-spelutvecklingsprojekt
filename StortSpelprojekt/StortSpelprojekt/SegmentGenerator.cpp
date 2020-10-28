#include "SegmentGenerator.h"

SegmentGenerator::SegmentGenerator() : constructed(false), treePoints(dx::XMFLOAT2(0,0), dx::XMFLOAT2(0, 0))
{
}

SegmentGenerator::~SegmentGenerator()
{
}

void SegmentGenerator::Initialize(Object* root, ResourceManager* resourceManager, ObjectSpawner* spawner, ID3D11Device* device, ID3D11DeviceContext* context)
{
	this->root = root;
	this->device = device;
	this->context = context;
	this->spawner = spawner;
	this->initialized = true;

	Material* mat = resourceManager->GetResource<Material>("Test");
	materialData = mat->GetMaterialData();

	this->grassShader = resourceManager->GetShaderResource("grassShader");
	this->chunkShader = resourceManager->GetShaderResource("terrainShader");

	InitializeTrees(resourceManager);
	Physics& physics = Physics::Instance();
	physics.MutexLock();

	
	physics.MutexUnlock();
}

void SegmentGenerator::Construct(const SaveState& state, const SegmentDescription& description)
{
	assert(initialized);

	if (!constructed)
	{
		CreateChunkMesh(chunkMesh, device);
		
		grid.Generate(description.maxSteps, CHUNK_PADDING, description.directionalSteps);
		auto& chunkData = grid.GetChunks();

		treePoints.Clear();
		dx::XMFLOAT2 min = Chunk::IndexToWorldXZ(grid.GetMinIndex());
		dx::XMFLOAT2 max = Chunk::IndexToWorldXZ(grid.GetMaxIndex());
		max.x += CHUNK_SIZE;
		max.y += CHUNK_SIZE;
		treePoints.SetMinMax(min, max);

		for (auto i : chunkData)
		{
			std::pair<dx::XMINT2, ChunkType> pair = i.second;
			CreateChunk(pair.first, root, description, pair.second);
		}

		spawner->Spawn(state, treePoints, chunkMap, chunks);
		constructed = true;
	}
}

void SegmentGenerator::Deconstruct()
{
	if (constructed)
	{
		for (int i = chunks.size() - 1; i >= 0; i--)
		{
			chunks[i]->PhysicRelease();
			Transform::ClearFromHierarchy(chunks[i]->GetOwner()->GetTransform());
			delete chunks[i];
		}

		spawner->Despawn();
		constructed = false;
		chunks.clear();
		grid.Clear();
	}
}

void SegmentGenerator::GetChunksInRadius(const dx::XMINT2& index, int radius, std::vector<Chunk*>& chunks) const
{
	if (constructed)
	{
		for (int y = -radius; y <= radius; y++)
		{
			for (int x = -radius; x <= radius; x++)
			{
				int i = HASH2D_I(index.x + x, index.y + y);
				auto find = chunkMap.find(i);
				if (find != chunkMap.end())
					chunks.push_back((*find).second);
			}
		}
	}
}

Chunk* SegmentGenerator::GetChunk(float x, float z) const
{
	dx::XMINT2 index = Chunk::WorldToIndex(x, z);
	int hash = HASH2D_I(index.x, index.y);
	Chunk* chunk = nullptr;

	auto find = chunkMap.find(hash);
	
	if (find != chunkMap.end())
		chunk = find->second;

	return chunk;
}

void SegmentGenerator::DrawDebug()
{
	treePoints.Draw(dx::XMFLOAT3(0, 0, 0));
	spawner->DrawDebug();

	// PATH
	const float offset = CHUNK_SIZE / 2.0f;
	std::vector<dx::XMFLOAT2> points = grid.GetPath().GetPoints();
	
	const int skip = 10;

	for (auto i = 0; i < points.size(); i += skip)
	{
		dx::XMFLOAT2 point0 = points[i];
		dx::XMFLOAT3 worldPoint(point0.x + offset, 5, point0.y + offset);
		DShape::DrawSphere(worldPoint, 0.5f, { 1,0,1 });

		if (i < points.size() - 1 && (i + skip) < points.size() - 1)
		{
			dx::XMFLOAT2 point1 = points[i + skip];
			DShape::DrawLine({ point1.x + offset, 5, point1.y + offset }, worldPoint, { 1,0,0 });
		}
	}
}

std::vector<SegmentGenerator::ChunkPointInformation> SegmentGenerator::CreateChunkMap(const dx::XMINT2& index, const SegmentDescription& description, float*& heightMap, unsigned char*& buffer)
{
	const size_t size = CHUNK_SIZE + 1;
	dx::XMFLOAT2 chunkPosXZ = Chunk::IndexToWorldXZ(index);

	buffer = new unsigned char[size * size * 4];
	heightMap = new float[size * size];

	const float MAX_DISTANCE = 10.0f;
	Path path = grid.GetPath();
	std::vector<ChunkPointInformation> informations;

	for (size_t y = 0; y < size; y++)
	{
		for (size_t x = 0; x < size; x++)
		{
			dx::XMFLOAT2 tilePos = dx::XMFLOAT2(chunkPosXZ.x + static_cast<float>(x), chunkPosXZ.y + static_cast<float>(y));

			float distance = path.ClosestDistance(tilePos);
			distance = std::min(distance, MAX_DISTANCE) / MAX_DISTANCE;

			float sampledHeight = Noise::Sample(chunkPosXZ.x + x, chunkPosXZ.y + y, description.noiseSettings);
			float worldHeight = sampledHeight * distance * TERRAIN_SCALE;
			
			worldHeight = std::max(worldHeight, MIN_TERRAIN_HEIGHT * 4.0f);
			
			
			float height = worldHeight / TERRAIN_SCALE;

			heightMap[x + size * y] = worldHeight;

			ChunkPointInformation chunkPoint;
			chunkPoint.distance = distance;
			chunkPoint.height = worldHeight;
			informations.push_back(chunkPoint);

			int bufferIndex = x + size * y;

			buffer[bufferIndex * 4 + 0] = static_cast<unsigned char>(255 * height);
			buffer[bufferIndex * 4 + 1] = static_cast<unsigned char>(255 * sampledHeight);
			buffer[bufferIndex * 4 + 2] = static_cast<unsigned char>(255 * distance);
			buffer[bufferIndex * 4 + 3] = static_cast<unsigned char>(255);
		}
	}

	return informations;
}

Chunk* SegmentGenerator::CreateChunk(const dx::XMINT2& index, Object* root, const SegmentDescription& description, ChunkType type)
{
	float* heightMap;
	unsigned char* buffer;
	auto chunkInformation = CreateChunkMap(index, description, heightMap, buffer);

	std::string name = "chunk " + std::to_string(index.x) + ", " + std::to_string(index.y);
	dx::XMVECTOR chunkPosition = Chunk::IndexToWorld(index, 0.0f);

	Object* chunkObject = new Object(name, ObjectFlag::RENDER);
	Chunk* chunk = chunkObject->AddComponent<Chunk>(index, type);

	chunkObject->GetTransform().SetPosition(chunkPosition);
	Transform::SetParentChild(root->GetTransform(), chunkObject->GetTransform());

	chunk->SetupCollisionObject(heightMap);

	auto chunkDataSRV = DXHelper::CreateTexture(buffer, CHUNK_SIZE + 1, CHUNK_SIZE + 1, 4, DXGI_FORMAT_R8G8B8A8_UNORM, device);

	Material material(chunkShader);
	//cb_Material mat = material.GetMaterialData();
	//mat.ambient = dx::XMFLOAT4(0.1f, 0.1f, 0.1f, 1);
	//mat.diffuse = dx::XMFLOAT4(0.1f, 0.1f, 0.1f, 1);
	//mat.specular = dx::XMFLOAT4(0.1f, 0.1f, 0.1f, 0.1f);

	material.SetMaterialData(materialData);

	Texture texture(chunkDataSRV);

	Texture grassTexture;
	//grassTexture.LoadTexture(device, L"Textures/Grass_001_COLOR.jpg");
	//grassTexture.LoadTexture(device, L"Textures/ground.png");
	grassTexture.LoadTexture(device, L"Textures/groundGrass.png");

	Texture roadTexture;
	roadTexture.LoadTexture(device, L"Textures/Stone_Floor_003_COLOR.jpg");

	material.SetTexture(texture, 0, ShaderBindFlag::PIXEL | ShaderBindFlag::VERTEX);
	material.SetTexture(grassTexture, 1, ShaderBindFlag::PIXEL);
	material.SetTexture(roadTexture, 2, ShaderBindFlag::PIXEL);

	delete[] buffer;

	auto dataSampler = DXHelper::CreateSampler(D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_CLAMP, device);
	material.SetSampler(dataSampler, 0, ShaderBindFlag::PIXEL | ShaderBindFlag::VERTEX);

	auto textureSampler = DXHelper::CreateSampler(D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_WRAP, device);
	material.SetSampler(textureSampler, 1, ShaderBindFlag::PIXEL);


	chunkObject->AddComponent<MeshComponent>(chunkMesh, material);

	AddTreesToChunk(chunk, chunkInformation);
	AddGrassToChunk(chunk, texture);

	int i = HASH2D_I(index.x, index.y);
	chunkMap.insert({ i, chunk });
	chunks.push_back(chunk);

	return chunk;
}

void SegmentGenerator::CreateChunkMesh(Mesh& mesh, ID3D11Device* device)
{
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
#if CW_CHUNK_MESH
					indicies.push_back((indexCount + CHUNK_SIZE + 1));	//TL
					indicies.push_back((indexCount + CHUNK_SIZE) + 2);	//TR
					indicies.push_back(indexCount + 1);					//BR
					indicies.push_back(indexCount + CHUNK_SIZE + 1);	//TL
					indicies.push_back(indexCount + 1);					//BR
					indicies.push_back(indexCount);						//BL
#else
					indicies.push_back((indexCount + CHUNK_SIZE) + 2);	//TR
					indicies.push_back((indexCount + CHUNK_SIZE + 1));	//TL
					indicies.push_back(indexCount + 1);					//BR
					indicies.push_back(indexCount + 1);					//BR
					indicies.push_back(indexCount + CHUNK_SIZE + 1);	//TL
					indicies.push_back(indexCount);						//BL
#endif
				}

				indexCount++;
			}
		}

		hasChunkMesh = true;
		mesh = Mesh(device, vertices, indicies);
	}
	else
	{
		mesh = chunkMesh;
	}
}

void SegmentGenerator::InitializeTrees(ResourceManager* resources)
{
	auto sampler = DXHelper::CreateSampler(D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_WRAP, device);

	Shader* instanceShader = resources->GetShaderResource("instanceShader");
	Shader* alphaInstanceShader = resources->GetShaderResource("alphaInstanceShader");

	//0 base 1 branch 2 leaves
	stylizedTreeModel = ZWEBLoader::LoadMeshes(ZWEBLoadType::NoAnimation, "Models/tree.ZWEB", device);
	//0 tree 1 leaves
	stylizedTreeMaterial = ZWEBLoader::LoadMaterials("Models/tree.ZWEB", instanceShader, device);

	stylizedTreeMaterial[0].SetSampler(sampler, 0, ShaderBindFlag::PIXEL);
	stylizedTreeMaterial[1].SetSampler(sampler, 0, ShaderBindFlag::PIXEL);

	stylizedTreeMaterial[0].SetShader(instanceShader);
	stylizedTreeMaterial[1].SetShader(alphaInstanceShader);
}

void SegmentGenerator::AddTreesToChunk(Chunk* chunk, std::vector<ChunkPointInformation>& chunkInformation)
{
	PossionDiscSampler sampler;
	Points points = sampler.GeneratePoints(5.0f, { CHUNK_SIZE, CHUNK_SIZE }, 5);

	//std::cout << "POINTS: " << points.size() << std::endl;

	if (points.size() > 0)
	{
		std::vector<dx::XMFLOAT2> validPoints;

		for (size_t i = 0; i < points.size(); i++)
		{
			if (ValidateTreePoint(points[i], chunkInformation))
				validPoints.push_back(points[i]);
		}

		size_t nrOfInstancedStyTrees = validPoints.size();
		if (nrOfInstancedStyTrees > 0)
		{
			std::vector<Mesh::InstanceData> treesInstanced(nrOfInstancedStyTrees);
			dx::XMFLOAT2 posXZ = Chunk::IndexToWorldXZ(chunk->GetIndex());

			for (size_t i = 0; i < nrOfInstancedStyTrees; i++)
			{
				float y = chunk->SampleHeight(posXZ.x + validPoints[i].x, posXZ.y + validPoints[i].y) * 0.9f + FRACKIG_TREE_ADJUSTMENT;
				treePoints.Insert(dx::XMFLOAT2(posXZ.x + validPoints[i].x, posXZ.y + validPoints[i].y));

				dx::XMFLOAT3 position(posXZ.x + validPoints[i].x, y, posXZ.y + validPoints[i].y);
				treesInstanced[i].instancePosition = position;

				dx::XMMATRIX translation = dx::XMMatrixScaling(2.0f, 2.0f, 2.0f) * dx::XMMatrixTranslation(position.x, position.y, position.z);
				dx::XMStoreFloat4x4(&treesInstanced[i].instanceWorld, translation);
			}

			stylizedTreeModel[0].CreateInstanceBuffer(device, treesInstanced);
			stylizedTreeModel[0].SetInstanceNr(nrOfInstancedStyTrees);
			stylizedTreeModel[1].CreateInstanceBuffer(device, treesInstanced);
			stylizedTreeModel[1].SetInstanceNr(nrOfInstancedStyTrees);
			stylizedTreeMaterial[1].SetTransparent(true);

			Object* tree = new Object("tree");
			tree->AddComponent<InstancedMeshComponent>(stylizedTreeModel, stylizedTreeMaterial);

			Transform::SetParentChild(chunk->GetOwner()->GetTransform(), tree->GetTransform());
		}
	}
}

void SegmentGenerator::AddGrassToChunk(Chunk* chunk, Texture& texture)
{
	size_t chunkTriangleCount = chunkMesh.indices.size() / 3;
	GrassComponent* grassComponent = chunk->GetOwner()->AddComponent<GrassComponent>(chunkTriangleCount, device, grassShader);
	grassComponent->GetMaterial().SetTexture(texture, 6, ShaderBindFlag::HULL);
	grassComponent->GetMaterial().SetTexture(texture, 6, ShaderBindFlag::DOMAINS);

	grassComponent->InitializeGrass(chunkMesh, device, context);

}

bool SegmentGenerator::ValidateTreePoint(const dx::XMFLOAT2& point, std::vector<ChunkPointInformation>& chunkInformation)
{
	int dx = static_cast<int>(roundf(point.x));
	int dy = static_cast<int>(roundf(point.y));

	float v = chunkInformation[dx + dy * (CHUNK_SIZE + 1)].distance;
	return v > TREE_SPAWN_FACTOR;
}
