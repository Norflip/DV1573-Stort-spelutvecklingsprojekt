#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include <DirectXMath.h>
#include "Mesh.h"

namespace ShittyOBJLoader
{
	inline std::vector<std::string> m_splitString(std::string str, char delimeter)
	{
		std::vector<std::string>  strs;
		size_t pos = str.find(delimeter);
		size_t initialPos = 0;
		strs.clear();

		while (pos != std::string::npos) 
		{
			strs.push_back(str.substr(initialPos, pos - initialPos));
			initialPos = pos + 1;
			pos = str.find(delimeter, initialPos);
		}

		size_t min = (pos < str.size()) ? pos : str.size();
		strs.push_back(str.substr(initialPos, min - initialPos + 1));
		return strs;
	}

	
	inline DirectX::XMFLOAT3 m_calculateTangent(Mesh::Vertex vertex1, Mesh::Vertex vertex2, Mesh::Vertex vertex3)
	{
		float vector1[3], vector2[3];
		float tuVector[2], tvVector[2];

		vector1[0] = vertex2.position.x - vertex1.position.x;
		vector1[1] = vertex2.position.y - vertex1.position.y;
		vector1[2] = vertex2.position.z - vertex1.position.z;

		vector2[0] = vertex3.position.x - vertex1.position.x;
		vector2[1] = vertex3.position.y - vertex1.position.y;
		vector2[2] = vertex3.position.z - vertex1.position.z;

		tuVector[0] = vertex2.uv.x - vertex1.uv.x;
		tvVector[0] = vertex2.uv.y - vertex1.uv.y;

		tuVector[1] = vertex3.uv.x - vertex1.uv.x;
		tvVector[1] = vertex3.uv.y - vertex1.uv.y;

		float den = 1.0f / (tuVector[0] * tvVector[1] - tuVector[1] * tvVector[0]);
		DirectX::XMFLOAT3 tangent;

		tangent.x = (tvVector[1] * vector1[0] - tvVector[0] * vector2[0]) * den;
		tangent.y = (tvVector[1] * vector1[1] - tvVector[0] * vector2[1]) * den;
		tangent.z = (tvVector[1] * vector1[2] - tvVector[0] * vector2[2]) * den;

		float length = sqrt((tangent.x * tangent.x) + (tangent.y * tangent.y) + (tangent.z * tangent.z));
		tangent.x = tangent.x / length;
		tangent.y = tangent.y / length;
		tangent.z = tangent.z / length;

		return tangent;
	}

	inline Mesh Load (std::string path, ID3D11Device* device)
	{
		std::vector<Mesh::Vertex> vertices;
		std::vector<unsigned int> indices;

		std::fstream fs;
		fs.open(path);

		if (fs.is_open())
		{

			std::vector<DirectX::XMFLOAT2> uvs;
			std::vector<DirectX::XMFLOAT3> normals;

			std::string line;
			std::vector<std::string> strs;
			std::string key;

			while (getline(fs, line))
			{
				if (line[0] != '#' && line[0] != 'o' && line[0] != 's')
				{
					float x, y, z;
					strs = m_splitString(line, ' ');
					key = strs[0];

					if (key == "vt")	// UV
					{
						uvs.push_back(DirectX::XMFLOAT2(
							std::stof(strs[1]),
							std::stof(strs[2])
						));
					}
					else if (key == "vn")	// NORMAL
					{
						normals.push_back(DirectX::XMFLOAT3(
							std::stof(strs[1]),
							std::stof(strs[2]),
							std::stof(strs[3])
						));
					}
					else if (key == "v")	// VERTEX
					{
						x = std::stof(strs[1]);
						y = std::stof(strs[2]);
						z = std::stof(strs[3]);

						Mesh::Vertex v;
						v.position = { x, y, z };

						vertices.push_back(v);
					}
					else if (key == "f")	//FACE
					{
						std::vector<std::string> subd;

						for (size_t i = 0; i < 3; i++)
						{
							subd = m_splitString(strs[i + 1], '/');
							int j = std::stoi(subd[0]) - 1;
							indices.push_back(j);

							DirectX::XMVECTOR d = DirectX::XMVectorAdd(
								DirectX::XMLoadFloat3(&vertices[j].normal),
								DirectX::XMLoadFloat3(&normals[std::stoi(subd[2]) - static_cast<long long>(1)])
							);
							//d = dx::XMVector3Normalize(d);

							DirectX::XMStoreFloat3(&vertices[j].normal, d);
							vertices[j].uv = uvs[std::stoi(subd[1]) - static_cast<long long>(1)];
						}
					}
				}
			}

			// normalize added normals
			for (size_t j = 0; j < vertices.size(); j++)
			{
				DirectX::XMVECTOR n = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&vertices[j].normal));
				DirectX::XMStoreFloat3(&vertices[j].normal, n);
			}

			DirectX::XMFLOAT3 tangent;
			size_t triangleCount = indices.size() / 3;

			for (size_t i = 0; i < triangleCount; i++)
			{
				tangent = m_calculateTangent(vertices[indices[i * 3 + 0]], vertices[indices[i * 3 + 1]], vertices[indices[i * 3 + 2]]);
				vertices[indices[i * 3 + 0]].tangent = tangent;
				vertices[indices[i * 3 + 1]].tangent = tangent;
				vertices[indices[i * 3 + 2]].tangent = tangent;
			}

			for (size_t j = 0; j < vertices.size(); j++)
			{
				DirectX::XMVECTOR n = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&vertices[j].tangent));
				DirectX::XMStoreFloat3(&vertices[j].tangent, n);
			}
		}

		return Mesh(device, vertices, indices);
	}
}