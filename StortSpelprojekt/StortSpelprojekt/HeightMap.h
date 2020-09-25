#ifndef HEIGHTMAP_H
#define HEIGHTMAP_H

#include "object.h"
//#include <d3d11.h>

//#include <fstream>
//#include <stdio.h>
namespace dx = DirectX;

class HeightMap : public Object
{
private:
	struct HeightMapType
	{
		dx::XMFLOAT3 position;
		dx::XMFLOAT3 normal;
	};

	struct ModelType
	{
		dx::XMFLOAT3 position;
		dx::XMFLOAT2 texCoord;
		dx::XMFLOAT3 normal;
	};

public:
	HeightMap(const std::string &, ObjectFlag flag = ObjectFlag::DEFAULT);
	HeightMap(const HeightMap&);
	~HeightMap();

	bool Initialize(std::string setupFilename, ID3D11Device* device);
	void Shutdown();
	
	Mesh* GetMesh() const;

private:

	bool LoadSetupFile(std::string);
	bool LoadBitmapHeightMap();
	void ShutdownHeightMap();
	void SetTerrainCoordinates();

	void CalculateNormals();
	void BuildTerrainModel();
	void ShutdownTerrainModel();

	void InitializeMesh(ID3D11Device*);

private:
	int vertexCount, indexCount;
	int terrainHeight, terrainWidth;
	float heightScale;
	std::string terrainFilename;
	HeightMapType* heightMap;
	ModelType* terrainModel;
	Mesh* theMesh;

};
#endif