#include "stdafx.h"
#include "SegmentGenerator.h"
#include "Engine.h"

SegmentGenerator::SegmentGenerator() : constructed(false), treePoints(dx::XMFLOAT2(0, 0), dx::XMFLOAT2(0, 0))
{
}

SegmentGenerator::~SegmentGenerator()
{
}

void SegmentGenerator::Initialize(Object* root, ResourceManager* resources, ObjectSpawner* spawner, ID3D11Device* device, ID3D11DeviceContext* context)
{
	this->root = root;
	this->device = device;
	this->context = context;
	this->spawner = spawner;
	this->initialized = true;

	//Material* mat = resourceManager->GetResource<Material>("TestMaterial");
	//materialData = mat->GetMaterialData();

	// Något fel med resourceManagerns textures, får fixa det efter sprintmötet
	//grassTexture = resourceManager->GetResource<Texture>("Grass");// .LoadTexture(device, L"Textures/newGrass.png");
	//roadTexture = resourceManager->GetResource<Texture>("Road");// .LoadTexture(device, L"Textures/Stone_Floor_003_COLOR.jpg");

	//0 base 1 branch 2 leaves
	stylizedTreeModel.push_back(resources->GetResource<Mesh>("Tree"));
	stylizedTreeModel.push_back(resources->GetResource<Mesh>("leaves"));
	//0 tree 1 leaves
	stylizedTreeMaterial.push_back(resources->GetResource<Material>("TreeMaterial"));
	stylizedTreeMaterial.push_back(resources->GetResource<Material>("leavesMaterial"));

	this->grassShader = resources->GetShaderResource("grassShader");
	this->chunkShader = resources->GetShaderResource("terrainShader");
}

void SegmentGenerator::Construct(const SaveState& state, const SegmentDescription& description)
{
	assert(initialized);

	if (!constructed)
	{
		chunkMesh = CreateChunkMesh(device);

		grid.Generate(description.maxSteps, CHUNK_PADDING, description.directionalSteps);
		path = Path(grid.GetIndexes());
		
		treePoints.Clear();
		dx::XMFLOAT2 min = Chunk::IndexToWorldXZ(grid.GetMinIndex());
		dx::XMFLOAT2 max = Chunk::IndexToWorldXZ(grid.GetMaxIndex());
		max.x += CHUNK_SIZE;
		max.y += CHUNK_SIZE;
		treePoints.SetMinMax(min, max);

		for (auto i : grid.GetChunks())
		{
			ChunkGrid::ChunkIndexInfo indexInfo = i.second;
			CreateChunk(indexInfo.index, root, description, indexInfo.type);
		}

		spawner->Spawn(state, treePoints, chunkMap, device);

		std::vector<dx::XMFLOAT2> positions;
		std::vector<float> angles;
		path.GetLanternInformation(positions, angles);

		spawner->SpawnSpecific(positions, { 0,1,0 }, angles, "Lamp", chunkMap, [](Object* obj)
		{
			obj->GetTransform().SetScale({ 1.2f, 1.2f, 1.2f });



		//	Object* light = new Object("lantern_pointLight");
		//	light->GetTransform().SetPosition({ 0,2,0 });
		//	light->AddComponent<PointLightComponent>(dx::XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f), 20.f);
		//	Transform::SetParentChild(obj->GetTransform(), light->GetTransform());

		});

		constructed = true;
	}
}

void SegmentGenerator::Deconstruct()
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
		grid.Clear();
		chunkMap.clear();

	}
}

void SegmentGenerator::DrawDebug()
{
	path.DrawDebug();
}

void SegmentGenerator::CreateChunkMap(const dx::XMINT2& index, const SegmentDescription& description, Chunk::Data& data)
{
	const size_t size = CHUNK_SIZE + 1;
	dx::XMFLOAT2 chunkPosXZ = Chunk::IndexToWorldXZ(index);

	unsigned char* buffer = new unsigned char[size * size * 4];
	data.heightMap = new float[size * size];
	data.influenceMap = new float[size * size];

	for (size_t y = 0; y < size; y++)
	{
		for (size_t x = 0; x < size; x++)
		{
			dx::XMFLOAT2 tilePos = dx::XMFLOAT2(chunkPosXZ.x + static_cast<float>(x), chunkPosXZ.y + static_cast<float>(y));

			float influence = path.SampleInfluence(tilePos);
			//distance = std::min(distance, MAX_DISTANCE) / MAX_DISTANCE;

			float sampledHeight = Noise::Sample(chunkPosXZ.x + x, chunkPosXZ.y + y, description.noiseSettings);
			float worldHeight = sampledHeight * influence * TERRAIN_SCALE;
			worldHeight = std::max(worldHeight, MIN_TERRAIN_HEIGHT * 4.0f);
			float height = worldHeight / TERRAIN_SCALE;

			data.heightMap[x + size * y] = worldHeight;
			data.influenceMap[x + size * y] = influence;

			int bufferIndex = x + size * y;

			buffer[bufferIndex * 4 + 0] = static_cast<unsigned char>(255 * height);
			buffer[bufferIndex * 4 + 1] = static_cast<unsigned char>(255 * sampledHeight);
			buffer[bufferIndex * 4 + 2] = static_cast<unsigned char>(255 * influence);
			buffer[bufferIndex * 4 + 3] = static_cast<unsigned char>(255);
		}
	}

	data.dataTexture = Texture::CreateFromBuffer(buffer, CHUNK_SIZE + 1, CHUNK_SIZE + 1, 4, DXGI_FORMAT_R8G8B8A8_UNORM, device);
}


Chunk* SegmentGenerator::CreateChunk(const dx::XMINT2& index, Object* root, const SegmentDescription& description, ChunkType type)
{
	Chunk::Data data;
	CreateChunkMap(index, description, data);

	std::string name = "chunk " + std::to_string(index.x) + ", " + std::to_string(index.y);
	Object* chunkObject = new Object(name, ObjectFlag::RENDER | ObjectFlag::ENABLED);
	Chunk* chunk = chunkObject->AddComponent<Chunk>(index, type, data);

	chunkObject->GetTransform().SetPosition(Chunk::IndexToWorld(index, 0.0f));
	Transform::SetParentChild(root->GetTransform(), chunkObject->GetTransform());

	chunk->Create();

	Material* material = new Material(chunkShader);
	cb_Material mat;
	mat.ambient = dx::XMFLOAT4(0.5f, 0.5f, 0.5f, 1);
	mat.diffuse = dx::XMFLOAT4(0.5f, 0.5f, 0.5f, 1);
	mat.specular = dx::XMFLOAT4(0.1f, 0.1f, 0.1f, 0.1f);

	material->SetMaterialData(mat);

	//Texture texture(chunkDataSRV);

	Texture* grassTexture = Texture::LoadTexture(device, L"Textures/newGrass.png");
	Texture* roadTexture = Texture::LoadTexture(device, L"Textures/Stone_Floor_003_COLOR.jpg");

	material->SetTexture(data.dataTexture, 0, ShaderBindFlag::PIXEL | ShaderBindFlag::VERTEX);
	material->SetTexture(grassTexture, 1, ShaderBindFlag::PIXEL);
	material->SetTexture(roadTexture, 2, ShaderBindFlag::PIXEL);

	auto dataSampler = DXHelper::CreateSampler(D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_CLAMP, device);
	material->SetSampler(dataSampler, 0, ShaderBindFlag::PIXEL | ShaderBindFlag::VERTEX);

	auto textureSampler = DXHelper::CreateSampler(D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_WRAP, device);
	material->SetSampler(textureSampler, 1, ShaderBindFlag::PIXEL);

	MeshComponent* meshComp = chunkObject->AddComponent<MeshComponent>(chunkMesh, material);

	Bounds& bounds = meshComp->GetBounds();
	dx::XMFLOAT3 max = bounds.GetMax();
	bounds.SetMinMax(bounds.GetMin(), dx::XMFLOAT3(max.x, TERRAIN_SCALE, max.z));

	AddTreesToChunk(chunk, data);
	AddGrassToChunk(chunk, data.dataTexture);

	int i = HASH2D_I(index.x, index.y);
	chunkMap.insert({ i, chunk });
	
	return chunk;
}

Mesh* SegmentGenerator::CreateChunkMesh(ID3D11Device* device)
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
		mesh = new Mesh(vertices, indicies);
		mesh->Initialize(device);
	}
	else
	{
		mesh = chunkMesh;
	}

	return mesh;
}

void SegmentGenerator::AddTreesToChunk(Chunk* chunk, const Chunk::Data& data)
{
	PossionDiscSampler sampler;

	Random::SetSeed(Random::GenerateSeed());
	Points points = sampler.GeneratePoints(5.0f, { CHUNK_SIZE, CHUNK_SIZE }, false, 5);

	if (points.size() > 0)
	{
		std::vector<dx::XMFLOAT2> validPoints;

		for (size_t i = 0; i < points.size(); i++)
		{
			if (ValidateTreePoint(points[i], data))
				validPoints.push_back(points[i]);
		}

		size_t nrOfInstancedStyTrees = validPoints.size();
		if (nrOfInstancedStyTrees > 0)
		{
			std::vector<Mesh::InstanceData> treesInstanced(nrOfInstancedStyTrees);
			dx::XMFLOAT2 posXZ = Chunk::IndexToWorldXZ(chunk->GetIndex());

			Bounds bbInfo;
			bbInfo.CalculateAABB(stylizedTreeModel[0]);
			dx::XMFLOAT3 extends = bbInfo.GetExtends();

			std::vector<dx::XMFLOAT3> colliderPositions;
			std::vector<dx::XMFLOAT3> colliderExtends;

			for (size_t i = 0; i < nrOfInstancedStyTrees; i++)
			{
				float y = chunk->SampleHeight(posXZ.x + validPoints[i].x, posXZ.y + validPoints[i].y) * TREE_HEIGHT_ADJUSTMENT_FACTOR;
				treePoints.Insert(dx::XMFLOAT2(posXZ.x + validPoints[i].x, posXZ.y + validPoints[i].y));

				dx::XMFLOAT3 position(posXZ.x + validPoints[i].x, y, posXZ.y + validPoints[i].y);
				treesInstanced[i].instancePosition = position;

				float scale = Random::Range(1.4f, 2.0f);
				dx::XMMATRIX rotation = dx::XMMatrixRotationQuaternion(dx::XMQuaternionRotationAxis({ 0,1,0 }, Random::RadAngle()));
				dx::XMMATRIX translation = dx::XMMatrixScaling(scale, scale, scale) * rotation * dx::XMMatrixTranslation(position.x, position.y, position.z);
				dx::XMStoreFloat4x4(&treesInstanced[i].instanceWorld, dx::XMMatrixTranspose(translation));

				colliderPositions.push_back(position);
				colliderExtends.push_back(extends);
			}

			Object* tree = new Object("tree", ObjectFlag::DEFAULT);
			Transform::SetParentChild(chunk->GetOwner()->GetTransform(), tree->GetTransform());
			tree->GetTransform().SetPosition({ 0,0,0 });
			tree->GetTransform().SetWorldPosition({ 0,0,0 });


			MeshComponent* meshComp = tree->AddComponent<MeshComponent>(stylizedTreeModel, stylizedTreeMaterial);

			meshComp->SetInstanceable(0, treesInstanced, nrOfInstancedStyTrees, device);

			stylizedTreeMaterial[1]->SetTransparent(true);

			tree->AddComponent<BoxColliderComponent>(colliderExtends, colliderPositions);
			tree->AddComponent<RigidBodyComponent>(0.f, FilterGroups::DEFAULT, FilterGroups::EVERYTHING, BodyType::STATIC, true);
		}
	}
}

void SegmentGenerator::AddGrassToChunk(Chunk* chunk, Texture* texture)
{
	size_t chunkTriangleCount = chunkMesh->GetTriangleCount();
	GrassComponent* grassComponent = chunk->GetOwner()->AddComponent<GrassComponent>(chunkTriangleCount, device, grassShader);
	grassComponent->GetMaterial()->SetTexture(texture, 6, ShaderBindFlag::HULL);
	grassComponent->GetMaterial()->SetTexture(texture, 6, ShaderBindFlag::DOMAINS);

	grassComponent->InitializeGrass(chunkMesh, device, context);
}


bool SegmentGenerator::ValidateTreePoint(const dx::XMFLOAT2& point, const Chunk::Data& data)
{
	int dx = static_cast<int>(roundf(point.x));
	int dy = static_cast<int>(roundf(point.y));

	float v = data.influenceMap[dx + dy * (CHUNK_SIZE + 1)];
	return v > TREE_SPAWN_FACTOR;
}
