#include "HeightMap.h"

HeightMap::HeightMap(const std::string& name, ObjectFlag flag): Object(name,flag)
{
	this->heightMap = nullptr;
	this->terrainModel = nullptr;
	this->theMesh = nullptr;
}
HeightMap::HeightMap(const HeightMap& other) :
	Object(other.GetName()) 
{
	//this->heightMap = nullptr;
	//this->terrainModel = nullptr;
}
HeightMap::~HeightMap()
{
	this->Shutdown();
}
bool HeightMap::Initialize(std::string setupFilename, ID3D11Device* device)
{
	// Get the terrain filename, dimensions, and so forth from the setup file.
	bool result = this->LoadSetupFile(setupFilename);
	if(result)
	{
		// Initialize the terrain height map with the data from the bitmap file.
		result = this->LoadBitmapHeightMap();
		if (result)
		{
			// Setup the X and Z coordinates for the height map as well as scale the terrain height by the height scale value.
			this->SetTerrainCoordinates();
			this->CalculateNormals();
			this->BuildTerrainModel();
			this->InitializeMesh(device);
			this->ShutdownHeightMap();
		}
	}
	return result;
}
void HeightMap::Shutdown()
{
	this->ShutdownTerrainModel(); //Release the terrain model.
	this->ShutdownHeightMap(); // Release the height map.
	delete this->theMesh;
	this->theMesh = nullptr;
}
Mesh* HeightMap::GetMesh() const
{
	return this->theMesh;
}
bool HeightMap::LoadSetupFile(std::string filename)
{
	bool result = false;
	std::ifstream fin;
	char input; 
	char separator = ':'; //identifier 
	fin.open(filename);
	if (fin.is_open())
	{
		fin.get(input);			//name
		while (input != separator)
			fin.get(input);
		fin >> this->terrainFilename;

		fin.get(input);			//height
		while (input != separator)
			fin.get(input);
		fin >> this->terrainHeight;

		fin.get(input);			//width
		while (input != separator)
			fin.get(input);
		fin >> this->terrainWidth;

		fin.get(input);			//height scaling
		while (input != separator)
			fin.get(input);
		fin >> this->heightScale;
		result = true;
		fin.close();
	}
	return result;;
}
bool HeightMap::LoadBitmapHeightMap()
{
	int imageSize, index;
	FILE* filePtr;
	unsigned char height;
	bool result = false;
	// Open the bitmap map file in binary.
	int error = fopen_s(&filePtr, this->terrainFilename.c_str(), "rb");
	if (error == 0)
	{
		BITMAPFILEHEADER bitmapFileHeader;
		BITMAPINFOHEADER bitmapInfoHeader;
		// Start by creating the array structure to hold the height map data.
		this->heightMap = new HeightMapType[this->terrainWidth * this->terrainHeight];
		if (!this->heightMap) //fail alloc??
		{
			return false;
		}

		unsigned long long count;
		// Read in the bitmap file header.
		count = fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);
		if (count != 1)
		{
			return false;
		}

		// Read in the bitmap info header.
		count = fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);
		if (count != 1)
		{
			return false;
		}

		// Make sure the height map dimensions are the same as the terrain dimensions for easy 1 to 1 mapping.
		if ((bitmapInfoHeader.biHeight != this->terrainHeight) || (bitmapInfoHeader.biWidth != this->terrainWidth))
		{
			return false;
		}

		// Calculate the size of the bitmap image data.  
		// Since we use non-divide by 2 dimensions (eg. 257x257) we need to add an extra byte to each line.
		imageSize = this->terrainHeight * ((this->terrainWidth * 3) + 1);

		// Allocate memory for the bitmap image data.
		unsigned char* bitmapImage;
		bitmapImage = new unsigned char[imageSize];
		if (!bitmapImage)
		{
			return false;
		}

		// Move to the beginning of the bitmap data.
		fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

		// Read in the bitmap image data.
		count = fread(bitmapImage, 1, imageSize, filePtr);
		if (count != imageSize)
		{
			return false;
		}

		// Close the file.
		error = fclose(filePtr);

		// Initialize the position in the image data buffer.
		int k = 0;

		// Read the image data into the height map array.
		for (int j = 0; j < this->terrainHeight; j++)
		{
			for (int i = 0; i < this->terrainWidth; i++)
			{
				// Bitmaps are upside down so load bottom to top into the height map array.
				index = (this->terrainWidth * (this->terrainHeight - 1 - j)) + i;

				// Get the grey scale pixel value from the bitmap image data at this location.
				height = bitmapImage[k];

				// Store the pixel value as the height at this point in the height map array.
				this->heightMap[index].position.y = (float)height;

				// Increment the bitmap image data index.
				k += 3;
			}

			// Compensate for the extra byte at end of each line in non-divide by 2 bitmaps (eg. 257x257).
			k++;
		}
		delete[] bitmapImage;
		bitmapImage = nullptr;
		result = true;
	}
	return result;
}
void HeightMap::ShutdownHeightMap()
{
	// Release the height map array.
	if (this->heightMap)
	{
		delete[] this->heightMap;
		this->heightMap = nullptr;
	}
}
void HeightMap::SetTerrainCoordinates()
{
	int index = 0;
	// Loop through all the elements in the height map array and adjust their coordinates correctly.
	for (int j = 0; j < this->terrainHeight; j++)
	{
		for (int i = 0; i < this->terrainWidth; i++)
		{
			index = (this->terrainWidth * j) + i;
			// Set the X and Z coordinates.
			this->heightMap[index].position.x = (float)i;
			this->heightMap[index].position.z = -(float)j;

			// Move the terrain depth into the positive range.  For example from (0, -256) to (256, 0).
			this->heightMap[index].position.z += (float)(this->terrainHeight - 1);

			// Scale the height.
			this->heightMap[index].position.y /= this->heightScale;
		}
	}
}
void HeightMap::CalculateNormals()
{
	int index1, index2, index3, index;
	float vertex1[3], vertex2[3], vertex3[3], vector1[3], vector2[3], sum[3], length;
	dx::XMFLOAT3* normals = new dx::XMFLOAT3[(this->terrainHeight - 1) * (this->terrainWidth - 1)];

	// Go through all the faces in the mesh and calculate their normals.
	for (int j = 0; j < (this->terrainHeight - 1); j++)
	{
		for (int i = 0; i < (this->terrainWidth - 1); i++)
		{
			index1 = ((j + 1) * this->terrainWidth) + i;		// Bottom left vertex.
			index2 = ((j + 1) * this->terrainWidth) + (i + 1);  // Bottom right vertex.
			index3 = (j * this->terrainWidth) + i;				// Upper left vertex.

			// Get three vertices from the face.
			vertex1[0] = this->heightMap[index1].position.x;
			vertex1[1] = this->heightMap[index1].position.y;
			vertex1[2] = this->heightMap[index1].position.z;

			vertex2[0] = this->heightMap[index2].position.x;
			vertex2[1] = this->heightMap[index2].position.y;
			vertex2[2] = this->heightMap[index2].position.z;

			vertex3[0] = this->heightMap[index3].position.x;
			vertex3[1] = this->heightMap[index3].position.y;
			vertex3[2] = this->heightMap[index3].position.z;

			// Calculate the two vectors for this face.
			vector1[0] = vertex1[0] - vertex3[0];
			vector1[1] = vertex1[1] - vertex3[1];
			vector1[2] = vertex1[2] - vertex3[2];
			vector2[0] = vertex3[0] - vertex2[0];
			vector2[1] = vertex3[1] - vertex2[1];
			vector2[2] = vertex3[2] - vertex2[2];
			index = (j * (this->terrainWidth - 1)) + i;

			// Calculate the cross product of those two vectors to get the un-normalized value for this face normal.
			normals[index].x = (vector1[1] * vector2[2]) - (vector1[2] * vector2[1]);
			normals[index].y = (vector1[2] * vector2[0]) - (vector1[0] * vector2[2]);
			normals[index].z = (vector1[0] * vector2[1]) - (vector1[1] * vector2[0]);

			// Calculate the length.
			length = (float)sqrt((normals[index].x * normals[index].x) + (normals[index].y * normals[index].y) +
				(normals[index].z * normals[index].z));

			// Normalize the final value for this face using the length.
			normals[index].x = (normals[index].x / length);
			normals[index].y = (normals[index].y / length);
			normals[index].z = (normals[index].z / length);
		}
	}
	// Now go through all the vertices and take a sum of the face normals that touch this vertex.
	for (int j = 0; j < this->terrainHeight; j++)
	{
		for (int i = 0; i < this->terrainWidth; i++)
		{
			// Initialize the sum.
			sum[0] = 0.0f;
			sum[1] = 0.0f;
			sum[2] = 0.0f;

			// Bottom left face.
			if (((i - 1) >= 0) && ((j - 1) >= 0))
			{
				index = ((j - 1) * (this->terrainWidth - 1)) + (i - 1);
				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
			}

			// Bottom right face.
			if ((i < (this->terrainWidth - 1)) && ((j - 1) >= 0))
			{
				index = ((j - 1) * (this->terrainWidth - 1)) + i;
				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
			}

			// Upper left face.
			if (((i - 1) >= 0) && (j < (this->terrainHeight - 1)))
			{
				index = (j * (this->terrainWidth - 1)) + (i - 1);
				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
			}

			// Upper right face.
			if ((i < (this->terrainWidth - 1)) && (j < (this->terrainHeight - 1)))
			{
				index = (j * (this->terrainWidth - 1)) + i;
				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
			}
			// Calculate the length of this normal.
			length = (float)sqrt((sum[0] * sum[0]) + (sum[1] * sum[1]) + (sum[2] * sum[2]));

			// Get an index to the vertex location in the height map array.
			index = (j * this->terrainWidth) + i;

			// Normalize the final shared normal for this vertex and store it in the height map array.
			this->heightMap[index].normal.x = (sum[0] / length);
			this->heightMap[index].normal.y = (sum[1] / length);
			this->heightMap[index].normal.z = (sum[2] / length);
		}
	}
	delete[] normals;// Release the temporary normals.
	normals = nullptr;
}
void HeightMap::BuildTerrainModel()
{
	int index = 0, index1, index2, index3, index4;
	this->vertexCount = (this->terrainHeight - 1) * (this->terrainWidth - 1) * 6; 
	this->terrainModel = new ModelType[this->vertexCount];
	// Load the 3D terrain model with the height map terrain data.
	// We will be creating 2 triangles for each of the four points in a quad.
	for (int j = 0; j < (this->terrainHeight - 1); j++)
	{
		for (int i = 0; i < (this->terrainWidth - 1); i++)
		{
			// Get the indexes to the four points of the quad.
			index1 = (this->terrainWidth * j) + i;          // Upper left.
			index2 = (this->terrainWidth * j) + (i + 1);      // Upper right.
			index3 = (this->terrainWidth * (j + 1)) + i;      // Bottom left.
			index4 = (this->terrainWidth * (j + 1)) + (i + 1);  // Bottom right.

			// Now create two triangles for that quad.
			// Triangle 1 - Upper left.
			this->terrainModel[index].position.x = this->heightMap[index1].position.x;
			this->terrainModel[index].position.y = this->heightMap[index1].position.y;
			this->terrainModel[index].position.z = this->heightMap[index1].position.z;
			this->terrainModel[index].texCoord.x = 0.0f;
			this->terrainModel[index].texCoord.y = 0.0f;
			this->terrainModel[index].normal.x = this->heightMap[index1].normal.x;
			this->terrainModel[index].normal.y = this->heightMap[index1].normal.y;
			this->terrainModel[index].normal.z = this->heightMap[index1].normal.z;
			index++;

			// Triangle 1 - Upper right.
			this->terrainModel[index].position.x = this->heightMap[index2].position.x;
			this->terrainModel[index].position.y = this->heightMap[index2].position.y;
			this->terrainModel[index].position.z = this->heightMap[index2].position.z;
			this->terrainModel[index].texCoord.x = 1.0f;
			this->terrainModel[index].texCoord.y = 0.0f;
			this->terrainModel[index].normal.x = this->heightMap[index2].normal.x;
			this->terrainModel[index].normal.y = this->heightMap[index2].normal.y;
			this->terrainModel[index].normal.z = this->heightMap[index2].normal.z;
			index++;

			// Triangle 1 - Bottom left.
			this->terrainModel[index].position.x = this->heightMap[index3].position.x;
			this->terrainModel[index].position.y = this->heightMap[index3].position.y;
			this->terrainModel[index].position.z = this->heightMap[index3].position.z;
			this->terrainModel[index].texCoord.x = 0.0f;
			this->terrainModel[index].texCoord.y = 1.0f;
			this->terrainModel[index].normal.x = this->heightMap[index3].normal.x;
			this->terrainModel[index].normal.y = this->heightMap[index3].normal.y;
			this->terrainModel[index].normal.z = this->heightMap[index3].normal.z;
			index++;

			// Triangle 2 - Bottom left.
			this->terrainModel[index].position.x = this->heightMap[index3].position.x;
			this->terrainModel[index].position.y = this->heightMap[index3].position.y;
			this->terrainModel[index].position.z = this->heightMap[index3].position.z;
			this->terrainModel[index].texCoord.x = 0.0f;
			this->terrainModel[index].texCoord.y = 1.0f;
			this->terrainModel[index].normal.x = this->heightMap[index3].normal.x;
			this->terrainModel[index].normal.y = this->heightMap[index3].normal.y;
			this->terrainModel[index].normal.z = this->heightMap[index3].normal.z;
			index++;

			// Triangle 2 - Upper right.
			this->terrainModel[index].position.x = this->heightMap[index2].position.x;
			this->terrainModel[index].position.y = this->heightMap[index2].position.y;
			this->terrainModel[index].position.z = this->heightMap[index2].position.z;
			this->terrainModel[index].texCoord.x = 1.0f;
			this->terrainModel[index].texCoord.y = 0.0f;
			this->terrainModel[index].normal.x = this->heightMap[index2].normal.x;
			this->terrainModel[index].normal.y = this->heightMap[index2].normal.y;
			this->terrainModel[index].normal.z = this->heightMap[index2].normal.z;
			index++;

			// Triangle 2 - Bottom right.
			this->terrainModel[index].position.x = this->heightMap[index4].position.x;
			this->terrainModel[index].position.y = this->heightMap[index4].position.y;
			this->terrainModel[index].position.z = this->heightMap[index4].position.z;
			this->terrainModel[index].texCoord.x = 1.0f;
			this->terrainModel[index].texCoord.y = 1.0f;
			this->terrainModel[index].normal.x = this->heightMap[index4].normal.x;
			this->terrainModel[index].normal.y = this->heightMap[index4].normal.y;
			this->terrainModel[index].normal.z = this->heightMap[index4].normal.z;
			index++;
		}
	}
}
void HeightMap::ShutdownTerrainModel()
{
	// Release the terrain model data.
	if (this->terrainModel)
	{
		delete[] this->terrainModel;
		this->terrainModel = nullptr;
	}
}
void HeightMap::InitializeMesh(ID3D11Device* device)
{
	// Set the height and width of the terrain grid. //get values from size of map?
	int terrainHeight = 256;
	int terrainWidth = 256;
	std::vector<Mesh::Vertex> vertices;
	std::vector<unsigned int> indices;
	//this->vertexCount = (terrainWidth - 1) * (terrainHeight - 1) * 6;
	this->indexCount = this->vertexCount;

	Mesh::Vertex tempV; //TempVertex;
	int tempI; //tempIndex
	for (int i = 0; i < this->vertexCount; i++) //Load 3D terrain model data.
	{
		tempV.position = dx::XMFLOAT3(this->terrainModel[i].position.x, this->terrainModel[i].position.y, this->terrainModel[i].position.z);
		tempV.uv = dx::XMFLOAT2(this->terrainModel[i].texCoord.x, this->terrainModel[i].texCoord.y);
		tempV.normal = dx::XMFLOAT3(this->terrainModel[i].normal.x, this->terrainModel[i].normal.y, this->terrainModel[i].normal.z);
		tempI = i;
		vertices.push_back(tempV);
		indices.push_back(tempI);
	}

	this->theMesh = new Mesh(device,vertices, indices);
	vertices.clear();
	indices.clear();

	ShutdownTerrainModel(); // Release the terrain model now that the rendering buffers have been loaded.
	//return theMesh;
}