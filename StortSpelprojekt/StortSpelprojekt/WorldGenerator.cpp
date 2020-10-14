#include "WorldGenerator.h"

WorldGenerator::WorldGenerator() : chunkMesh(), path(0)
{
	grassComponents.clear();
}

WorldGenerator::~WorldGenerator()
{
	chunks.clear();
}

void WorldGenerator::InitalizeGrass(ID3D11Device* device, ID3D11DeviceContext* context)
{
	for (int grass = 0; grass < grassComponents.size(); grass++)
	{
		grassComponents[grass]->InitializeGrass(chunkMesh.vertices, chunkMesh.indices, device, context);
	}
}

void WorldGenerator::InitializeTrees(std::vector<Mesh> models, std::vector<Material> materials, ID3D11Device* device)
{
	PossionDiscSampler sampler(0);

	for (auto chunk : chunks)
	{
		Points points = sampler.GeneratePoints(5.0f, { CHUNK_SIZE, CHUNK_SIZE }, false);
		size_t nrOfInstancedStyTrees = points.size();

		if (nrOfInstancedStyTrees > 0)
		{
			std::vector<Mesh::InstanceData> treesInstanced(nrOfInstancedStyTrees);
			std::vector<Mesh::InstanceData> leavesInstanced(nrOfInstancedStyTrees);
			dx::XMFLOAT2 posXZ = Chunk::IndexToXZ(chunk->GetIndex());

			for (size_t i = 0; i < nrOfInstancedStyTrees; i++)
			{
				dx::XMFLOAT3 chunkPosition;
				dx::XMStoreFloat3(&chunkPosition, chunk->GetOwner()->GetTransform().GetWorldPosition());
				float y = chunkPosition.y;// +chunk->SampleHeight(points[i].x, points[i].y);
				
				dx::XMFLOAT3 position (posXZ.x + points[i].x, y, posXZ.y + points[i].y);
				leavesInstanced[i].instancePosition = position;
				treesInstanced[i].instancePosition = position;

				dx::XMMATRIX translation = dx::XMMatrixScaling(0.5f, 0.5f, 0.5f) * dx::XMMatrixTranslation(position.x, position.y, position.z);
				dx::XMStoreFloat4x4(&leavesInstanced[i].instanceWorld, translation);
				dx::XMStoreFloat4x4(&treesInstanced[i].instanceWorld, translation);
			}

			models[0].CreateInstanceBuffer(device, treesInstanced);
			models[0].SetInstanceNr(nrOfInstancedStyTrees);

			models[1].CreateInstanceBuffer(device, leavesInstanced);
			models[1].SetInstanceNr(nrOfInstancedStyTrees);

			materials[1].SetTransparent(true);

			Object* styTreeBase = new Object("treeBase");
			styTreeBase->AddComponent<InstancedMeshComponent>(models[0], materials[0]);

			Object* styLeavesBase = new Object("leaves", ObjectFlag::DEFAULT | ObjectFlag::NO_CULL);
			styLeavesBase->AddComponent<InstancedMeshComponent>(models[1], materials[1]);

			Transform::SetParentChild(chunk->GetOwner()->GetTransform(), styTreeBase->GetTransform());
			Transform::SetParentChild(chunk->GetOwner()->GetTransform(), styLeavesBase->GetTransform());

			// just the first one for testing 
			//break;
		}
	}
}

void WorldGenerator::Initialize(ID3D11Device* device, Shader* shader, Shader* grassShader)
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
				indicies.push_back((indexCount + CHUNK_SIZE + 1));
				indicies.push_back((indexCount + CHUNK_SIZE) + 2);
				indicies.push_back(indexCount + 1);

				indicies.push_back(indexCount + CHUNK_SIZE + 1);
				indicies.push_back(indexCount + 1);
				indicies.push_back(indexCount);
			}

			indexCount++;
		}
	}


	/****************EMILKOD****************/
  
	this->shader = shader;
	chunkMesh = Mesh(device, vertices, indicies);

	this->grassShader = grassShader;
  
	Mesh::Vertex v;
	grassV.push_back(v);
	for (int triangle = 0; triangle < chunkMesh.indices.size() / 3; triangle++)
	{

		grassI.push_back(triangle);

	}

	/**********************************/

	/*if (CHUNK_SIZE == 64U)
	{
		chunkMesh = ShittyOBJLoader::Load("Models/plane64x64.obj", device);
	}
	else
	{
		chunkMesh = ShittyOBJLoader::Load("Models/plane32x32.obj", device);
	}*/

}

void WorldGenerator::Generate(const SaveState& levelState, ID3D11Device* device, Object* root)
{
	int segmentSeed = levelState.seed ^ std::hash<int>()(levelState.segment);
	path.Reseed(segmentSeed);
	path.Walk(5);

	Noise::Settings settings(segmentSeed);
	settings.octaves = 5;
	settings.persistance = 0.5f;
	settings.lacunarity = 1.4f;
	settings.scale = 1.f;

	std::unordered_set<int> visited;
	dx::XMINT2 index = { 0,0 };

	size_t indexCount = 0;
	size_t collisionCount = 0;
	size_t chunkCount = 0;

	std::vector<dx::XMINT2>& indexes = path.GetIndexes();

	Physics& physics = Physics::Instance();
	physics.MutexLock();

	for (auto i = indexes.cbegin(); i < indexes.cend(); i++)
	{
		// load around
		for (int y = -LOAD_RADIUS; y <= LOAD_RADIUS; y++)
		{
			for (int x = -LOAD_RADIUS; x <= LOAD_RADIUS; x++)
			{
				index.x = (*i).x + x;
				index.y = (*i).y + y;
				int hash = HASH2D_I(index.x, index.y);

				if (visited.find(hash) == visited.end())
				{
					ChunkType type = ChunkType::Path;

					if (indexCount == 0)
						type = ChunkType::Start;
					if (indexCount == indexes.size() - 1)
						type = ChunkType::Goal;

					CreateChunk(type, index, settings, device, root);
					
					visited.insert(hash);
					chunkCount++;
				}
				else
				{
					collisionCount++;
				}
			}
		}

		indexCount++;
	}

	physics.MutexUnlock();
}

void WorldGenerator::DrawShapes ()
{
	const float offset = CHUNK_SIZE / 2.0f;
	std::vector<dx::XMFLOAT2>& points = path.GetPoints();

	for (auto i = 0; i < points.size(); i++)
	{
		dx::XMFLOAT2 point0 = points[i];
		dx::XMFLOAT3 worldPoint (point0.x + offset, 5, point0.y + offset);
		DShape::DrawSphere(worldPoint, 0.5f, { 0,0,0 });

		if (i < points.size() - 1)
		{
			dx::XMFLOAT2 point1 = points[i + 1];
			DShape::DrawLine({ point1.x + offset, 5, point1.y + offset }, worldPoint, { 1,0,0 });
		}
	}
}

Chunk* WorldGenerator::CreateChunk(ChunkType type, dx::XMINT2 index, const Noise::Settings& settings, ID3D11Device* device, Object* root)
{
	// + 1 f�r height map
	const size_t size = CHUNK_SIZE + 1;

	dx::XMFLOAT2 chunkPosXZ = Chunk::IndexToXZ(index);

	unsigned char* buffer = new unsigned char[size * size * 4];
	float* heightMap = new float[size * size];
	std::vector<float> heightMap2(size * size);
	//heightMap2.resize(size * size);

	const float MAX_DISTANCE = 10.0f;

	for (size_t y = 0; y < size; y++)
	{
		for (size_t x = 0; x < size; x++)
		{
			dx::XMFLOAT2 tilePos = dx::XMFLOAT2(chunkPosXZ.x + static_cast<float>(x), chunkPosXZ.y + static_cast<float>(y));

			float distance = path.ClosestDistance(tilePos);
			distance = std::min(distance, MAX_DISTANCE) / MAX_DISTANCE;

			int bufferIndex = x + size * y;

			float height = Noise::Sample(chunkPosXZ.x + x, chunkPosXZ.y + y, settings);
			heightMap[x + size * y] = height;
			heightMap2.push_back(height);

			//std::cout << height << std::endl;

			buffer[bufferIndex * 4 + 0] = static_cast<unsigned char>(255 * height * distance);
			buffer[bufferIndex * 4 + 1] = static_cast<unsigned char>(255 * distance);
			buffer[bufferIndex * 4 + 2] = static_cast<unsigned char>(255 * height);
			buffer[bufferIndex * 4 + 3] = static_cast<unsigned char>(255);

			//int d = x + size * y;

			//if (d != bufferIndex)
			//{
			//	std::cout << "index: " << d << " | " << bufferIndex << std::endl;
			//}

			///bufferIndex++;
		}
	}

	auto chunkDataSRV = DXHelper::CreateTexture(buffer, size, size, 4, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, device);
	delete[] buffer;


	Material material(shader);
	material.SetTexture(Texture(chunkDataSRV), 0, ShaderBindFlag::PIXEL | ShaderBindFlag::VERTEX);
	
	auto sampler = DXHelper::CreateSampler(D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_CLAMP, device);
	material.SetSampler(sampler, 0, ShaderBindFlag::PIXEL | ShaderBindFlag::VERTEX);


	std::string name = "chunk " + std::to_string(index.x) + ", " + std::to_string(index.y);
	
	Object* chunkObject = new Object(name, ObjectFlag::DEFAULT);
	Transform::SetParentChild(root->GetTransform(), chunkObject->GetTransform());

	dx::XMVECTOR chunkPosition = Chunk::IndexToWorld(index, 0.0f);
	chunkObject->GetTransform().SetPosition(chunkPosition);
	
	chunkObject->AddComponent<MeshComponent>(chunkMesh, material);

	Chunk* chunk = chunkObject->AddComponent<Chunk>(index, type);
	

	GrassComponent* grassComponent = chunkObject->AddComponent<GrassComponent>(device, grassV, grassI, grassShader);

	grassComponent->GetMaterial().SetTexture(Texture(chunkDataSRV), 6, ShaderBindFlag::HULL);

	grassComponent->GetMaterial().SetTexture(Texture(chunkDataSRV), 6, ShaderBindFlag::DOMAINS);

	//grassComponent->SetType(type);

	grassComponents.push_back(grassComponent);

	chunk->SetupCollisionObject(heightMap);

	//chunkObject->AddComponent<ChunkCollider>(chunk);
	//RigidBodyComp* rigidBody = chunkObject->AddComponent<RigidBodyComp>(STATIC_BODY, PhysicsGroup::TERRAIN);

	//remember to remove when loading new map
	//Physics::Instance().RegisterRigidBody(rigidBody);

	chunks.push_back(chunk);
	chunkMap.insert({ HASH2D_I(index.x, index.y), chunk });

	return chunk;
}


dx::XMFLOAT3 WorldGenerator::CalculateNormal(float x, float y, const Noise::Settings& settings) const
{
	// left, right, down, up
	const float quadsize = 1.0f;

	float left = Noise::Sample(x - quadsize, y, settings) * TERRAIN_SCALE;
	float right = Noise::Sample(x + quadsize, y, settings) * TERRAIN_SCALE;
	float down = Noise::Sample(x, y - quadsize, settings) * TERRAIN_SCALE;
	float up = Noise::Sample(x, y + quadsize, settings) * TERRAIN_SCALE;

	// Z 
	dx::XMVECTOR horizontal = dx::XMVector3Normalize({ 1.0f, 0.0f, right - left });
	dx::XMVECTOR vertical = dx::XMVector3Normalize({ 0.0f, 1.0f, up - down });

	// Y
	//dx::XMVECTOR horizontal = dx::XMVector3Normalize({ 1.0f, right - left, 0.0f });
	//dx::XMVECTOR vertical = dx::XMVector3Normalize({ 0.0f, up - down, -1.0f }); */

	dx::XMFLOAT3 ret;
	dx::XMStoreFloat3(&ret, dx::XMVector3Cross(horizontal, vertical));
	return ret;
}



