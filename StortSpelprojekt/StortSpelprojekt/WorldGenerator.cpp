#include "WorldGenerator.h"

WorldGenerator::WorldGenerator() : chunkMesh()
{

}

WorldGenerator::~WorldGenerator()
{
	for (size_t i = 0; i < chunks.size(); i++)
		delete chunks[i]->GetOwner();

	chunks.clear();
}

void WorldGenerator::Initialize(ID3D11Device* device)
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

	shader.SetPixelShader(L"Shaders/Terrain_ps.hlsl");
	shader.SetVertexShader(L"Shaders/Terrain_vs.hlsl");
	shader.Compile(device);

	chunkMesh = Mesh(device, vertices, indicies);

	/*if (CHUNK_SIZE == 64U)
	{
		chunkMesh = ShittyOBJLoader::Load("Models/plane64x64.obj", device);
	}
	else
	{
		chunkMesh = ShittyOBJLoader::Load("Models/plane32x32.obj", device);
	}*/
}

void WorldGenerator::Generate(const SaveState& levelState, ID3D11Device* device)
{
	size_t steps = 4;
	int segmentSeed = GetSegmentSeed(levelState);

	Path path = CalculatePath(steps, segmentSeed);
	std::unordered_set<int> visited;
	dx::XMINT2 index = { 0,0 };
	size_t indexCount = 0;

	size_t collisionCount = 0;
	size_t chunkCount = 0;
	
	for (auto i = path.cbegin(); i < path.cend(); i++)
	{
		// load around
		for (int y = 0; y <= LOAD_RADIUS; y++)
		{
			for (int x = 0; x <= LOAD_RADIUS; x++)
			{
				index.x = (*i).x + x;
				index.y = (*i).y + y;
				int hash = HASH2D_I(index.x, index.y);

				if (visited.find(hash) == visited.end())
				{
					ChunkType type = ChunkType::Path;

					if (indexCount == 0) 
						type = ChunkType::Start;
					if (indexCount == path.size() - 1) 
						type = ChunkType::Goal;

					CreateChunk(type, index, path, device);
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

}

void WorldGenerator::Draw(Renderer* renderer, CameraComponent* camera)
{
	for (auto i = chunks.begin(); i < chunks.end(); i++)
	{
		(*i)->GetOwner()->Draw(renderer, camera);
	}
}

std::vector<dx::XMINT2> WorldGenerator::CalculatePath(size_t steps, int seed)
{
	const int STEPS_FORWARD = 2;
	std::vector<dx::XMINT2> path;

	std::default_random_engine rng(seed);

	dx::XMINT2 direction = { 0,1 };
	dx::XMINT2 current = { 0,0 };
	size_t maxSteps = steps;
	std::uniform_real_distribution<float> distribution(0.0f, 1.0f);
	dx::XMINT2 tmp = { 0,0 };

	while (maxSteps > 0)
	{
		for (size_t i = 0; i < STEPS_FORWARD && maxSteps > 0; i++)
		{
			tmp.x = current.x + direction.x;
			tmp.y = current.y + direction.y;
			path.push_back(tmp);

			maxSteps--;
			current = tmp;
		}

		direction = GetDirection(direction, distribution(rng), rng);
	}

	return path;
}

dx::XMINT2 WorldGenerator::GetDirection(dx::XMINT2 direction, float value, const std::default_random_engine& rng)
{
	dx::XMINT2 newDirection = direction;

	// Filip: Kolla genom logiken här
	if (newDirection.y == 0)
	{
		if (value < 0.8f)
			newDirection = { 0, 1 };
	}
	else
	{
		if (value < 0.33f)
			newDirection = { -1, 0 };
		else if (value > 0.66f)
			newDirection = { 1, 0 };
	}

	return newDirection;
}

float WorldGenerator::GetDistanceToPath(const dx::XMFLOAT2& position, const Path& indexes) const
{
	float shortest = FLT_MAX;

	for (size_t i = 0; i < indexes.size() - 1; i++)
	{
		dx::XMFLOAT2 p0 = PathIndexToWorld(indexes[i]);
		dx::XMFLOAT2 p1 = PathIndexToWorld(indexes[i + 1]);

		float tmpDistance = Math::DistanceToLineSqr(position.x, position.y, p0.x, p0.y, p1.x, p1.y);

		if (tmpDistance < shortest)
		{
			//std::cout << "(" << p0.x << ", " << p0.y << ") > (" << p1.x << ", " << p1.y << ") dst: " << tmpDistance << " p: " << position.x << ", " << position.y << std::endl;
			shortest = tmpDistance;
		}
	}

	return sqrtf(shortest);
}

dx::XMFLOAT2 WorldGenerator::PathIndexToWorld(const dx::XMINT2& i) const
{
	float x = static_cast<float>(i.x * CHUNK_SIZE) + (static_cast<float>(CHUNK_SIZE) / 2.0f);
	float y = static_cast<float>(i.y * CHUNK_SIZE) + (static_cast<float>(CHUNK_SIZE) / 2.0f);
	return dx::XMFLOAT2(x, y);
}

Chunk* WorldGenerator::CreateChunk(ChunkType type, dx::XMINT2 index, const Path& path, ID3D11Device* device)
{
	// + 1 för height map
	size_t size = CHUNK_SIZE + 1;

	Noise::Settings settings (0);
	settings.octaves = 4;
	settings.persistance = 0.5f;
	settings.lacunarity = 1.99f;
	settings.scale = 20.f;
	
	dx::XMFLOAT2 pos = Chunk::IndexToXZ(index);

	unsigned char* buffer = new unsigned char[size * size * 4];
	unsigned char* normalBuffer = new unsigned char[size * size * 4];
	float* heightMap = new float[size * size];

	size_t bufferIndex = 0;

	const float MAX_DISTANCE = 10.0f;

	for (size_t y = 0; y < size; y++)
	{
		for (size_t x = 0; x < size; x++)
		{
			dx::XMFLOAT2 pf = dx::XMFLOAT2(pos.x + static_cast<float>(x), CHUNK_SIZE - pos.y + static_cast<float>(y));
			float distance = GetDistanceToPath(pf, path);
			
			if (distance > MAX_DISTANCE)
				distance = MAX_DISTANCE;

			distance = distance / MAX_DISTANCE;
 
			dx::XMFLOAT3 normal = CalculateNormal(pos.x + x, CHUNK_SIZE - pos.y + y, settings);
			normalBuffer[bufferIndex * 4 + 0] = static_cast<unsigned char>(255 * normal.x);
			normalBuffer[bufferIndex * 4 + 1] = static_cast<unsigned char>(255 * normal.y);
			normalBuffer[bufferIndex * 4 + 2] = static_cast<unsigned char>(255 * normal.z);
			normalBuffer[bufferIndex * 4 + 3] = 255;
			
			float packedNormal = Math::Pack3DVector(normal.x, normal.y, normal.z);
			float height = Noise::Sample(pos.x + x, pos.y + y, settings);

			heightMap[bufferIndex] = height;

			buffer[bufferIndex * 4 + 0] = static_cast<unsigned char>(255 * height);
			buffer[bufferIndex * 4 + 1] = static_cast<unsigned char>(255 * distance);
			buffer[bufferIndex * 4 + 2] = static_cast<unsigned char>(255 * distance);
			buffer[bufferIndex * 4 + 3] = static_cast<unsigned char>(255);
			bufferIndex++;
		}
	}
	
	auto srv = DXHelper::CreateTexture(buffer, size, size, 4, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, device);
	auto nsrv = DXHelper::CreateTexture(normalBuffer, size, size, 4, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, device);

	//delete[] heightMap;
	delete[] buffer;
	delete[] normalBuffer;

	Material material(shader);
	material.SetTexture(Texture(srv), 0, ShaderBindFlag::PIXEL);
	material.SetTexture(Texture(srv), 0, ShaderBindFlag::VERTEX);
	material.SetTexture(Texture(nsrv), 1, ShaderBindFlag::VERTEX);

	std::string name = "chunk " + std::to_string(index.x) + ", " + std::to_string(index.y);
	Object* chunkObject = new Object(name, ObjectFlag::DEFAULT);

	chunkObject->GetTransform().SetPosition(Chunk::IndexToWorld(index, -5.0f));
	chunkObject->AddComponent<MeshComponent>(chunkMesh, material);

	Chunk* chunk = chunkObject->AddComponent<Chunk>(index, type);
	chunk->SetHeightMap(heightMap);

	chunks.push_back(chunk);
	chunkMap.insert({ HASH2D_I(index.x, index.y), chunk });

	return chunk;
}


dx::XMFLOAT3 WorldGenerator::CalculateNormal(float x, float y, const Noise::Settings& settings) const
{
	// left, right, down, up
	const float quadsize = 1.0f;
	const float terrainHeight = 10.0f;

	float left = Noise::Sample(x - quadsize, y, settings) * terrainHeight;
	float right = Noise::Sample(x + quadsize, y, settings) * terrainHeight;
	float down = Noise::Sample(x, y - quadsize, settings) * terrainHeight;
	float up = Noise::Sample(x, y + quadsize, settings) * terrainHeight;

	// Z 
	dx::XMVECTOR horizontal = dx::XMVector3Normalize({ 1.0f, 0.0f, right - left });
	dx::XMVECTOR vertical = dx::XMVector3Normalize({ 0.0f, 1.0f, up - down});
	
	// Y
	//dx::XMVECTOR horizontal = dx::XMVector3Normalize({ 1.0f, right - left, 0.0f });
	//dx::XMVECTOR vertical = dx::XMVector3Normalize({ 0.0f, up - down, -1.0f }); */

	dx::XMFLOAT3 ret;
	dx::XMStoreFloat3(&ret, dx::XMVector3Cross(horizontal, vertical));
	return ret;
}
