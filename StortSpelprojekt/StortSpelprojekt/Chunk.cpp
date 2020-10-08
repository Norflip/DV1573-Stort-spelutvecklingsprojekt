#include "Chunk.h"
#include <iostream>

Chunk::Chunk(dx::XMINT2 index, ChunkType type) : index(index), type(type), heightMap(nullptr)
{

}

Chunk::~Chunk()
{
	if (heightMap)
		delete[] heightMap;

	//delete heightMap;
	//heightMap = 0;
}

float Chunk::SampleHeight(float x, float z)
{
	int col =  (int)floorf(x);
	int row =  (int)floorf(z);
	float height = 0.0f;

	if (row >= 0 && col >= 0 && row < CHUNK_SIZE && col < CHUNK_SIZE)
	{
		float bl = heightMap[col + CHUNK_SIZE * row];
		float br = heightMap[(col + 1) + CHUNK_SIZE * row];
		float tr = heightMap[(col + 1) + CHUNK_SIZE * (row + 1)];
		float tl = heightMap[col + CHUNK_SIZE * (row + 1)];

		float u = x - (float)col;
		float v = z - (float)row;
		height = Math::Lerp(Math::Lerp(bl, br, u), Math::Lerp(tl, tr, u), 1.0f - v);
	}
	
	std::cout << "col: " << col << ", row: " << row << ", height: " << height << std::endl;
	return height * TERRAIN_SCALE;
}

dx::XMVECTOR Chunk::IndexToWorld(const dx::XMINT2& index, float y)
{
	float x = static_cast<float>(index.x * (int)CHUNK_SIZE);// +((float)CHUNK_SIZE / 2.0f);
	float z = static_cast<float>(index.y * (int)CHUNK_SIZE) - CHUNK_SIZE;// + ((float)CHUNK_SIZE / 2.0f);
	dx::XMVECTOR pos = { x,y,z };
	return pos;
}

dx::XMFLOAT2 Chunk::IndexToXZ(const dx::XMINT2& index)
{
	float x = (float)index.x * (float)CHUNK_SIZE;
	float y = (float)index.y * (float)CHUNK_SIZE - CHUNK_SIZE;

	return dx::XMFLOAT2(x, y);
}


//
//dx::XMINT2 Chunk::WorldToIndex(const dx::XMVECTOR& position)
//{
//	dx::XMFLOAT3 pos;
//	dx::XMStoreFloat3(&pos, position);
//
//	int x = static_cast<int>(floor(pos.x / CHUNK_SIZE));
//	int z = static_cast<int>(floor(pos.z / CHUNK_SIZE));
//	return dx::XMINT2(x, z);
//}
//
//dx::XMINT2 Chunk::WorldToIndex(const dx::XMFLOAT2& position)
//{
//	int x = static_cast<int>(floor(position.x / CHUNK_SIZE));
//	int z = static_cast<int>(floor(position.y / CHUNK_SIZE));
//	return dx::XMINT2(x, z);
//}
