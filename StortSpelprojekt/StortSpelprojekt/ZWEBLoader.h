#pragma once
#include <ImportZWEB.h>
#include <DirectXMath.h>
#include "MeshComponent.h"
enum ZWEBLoadType
{
	NoAnimation,
	SkeletonAnimation,
};
namespace ZWEBLoader //TO BE ADDED: FUNCTION TO LOAD LIGHTS AND TO LOAD TEXTURES INOT MATERIALS FROM PATHWAY
{
	inline  std::vector<Object> LoadZWEB(ZWEBLoadType type, std::string scenePath, std::string animationPath,const Shader& shader, ID3D11Device* device) //If object doesn't have animation you can leave that parameter empty
	{
		ZWEB::ZWEBImporter importer;

		importer.importScene(scenePath);

		importer.importAnimation(animationPath);

		
		std::vector<Object> objects;
		for (int mesh = 0; mesh < importer.getSceneInfo().nrOfMeshes; mesh++)
		{
			std::vector<unsigned int> indicesZweb(importer.getMeshInfo(mesh).nrOfindices);
			std::vector<VertexHeader> verticesZweb(importer.getMeshInfo(mesh).nrOfVertices);
			indicesZweb = importer.getIndices(mesh);
			verticesZweb = importer.fillVertices(mesh);

			for (unsigned int index = 0; index < indicesZweb.size(); index++)
			{
				indicesZweb[index] = index; //This is because the number of vertices must match the number of indices.
			}
			std::vector<Mesh::Vertex> vertices(verticesZweb.size());

			for (unsigned int vertex = 0; vertex < verticesZweb.size(); vertex++)
			{
				vertices[vertex].position = DirectX::XMFLOAT3(verticesZweb[vertex].pos[0], verticesZweb[vertex].pos[1], verticesZweb[vertex].pos[2]);

				vertices[vertex].uv = DirectX::XMFLOAT2(verticesZweb[vertex].uv[0], 1.0f - verticesZweb[vertex].uv[1]);

				vertices[vertex].normal = DirectX::XMFLOAT3(verticesZweb[vertex].normal[0], verticesZweb[vertex].normal[1], verticesZweb[vertex].normal[2]);// *-1.0f);

				vertices[vertex].tangent = DirectX::XMFLOAT3(verticesZweb[vertex].tangent[0], verticesZweb[vertex].tangent[1], verticesZweb[vertex].tangent[2]);

				vertices[vertex].binormal = DirectX::XMFLOAT3(verticesZweb[vertex].biNormal[0], verticesZweb[vertex].biNormal[1], verticesZweb[vertex].biNormal[2]);

				
			}
			std::map<std::string, unsigned int> boneIDMap; //This is to make sure correct Vertex is mapped to the Correct Bone/Joint.
			if (type == ZWEBLoadType::SkeletonAnimation)
			{
				std::vector<VertexHeader> controlVerticesZweb = importer.getControlPoints(mesh); //Controlpoints are indexed, converting them into non indexed here.
				std::vector<Mesh::Vertex> controlVertices(controlVerticesZweb.size());
				
				for (unsigned int controlVertex = 0; controlVertex < controlVerticesZweb.size(); controlVertex++)
				{
					controlVertices[controlVertex].boneID = DirectX::XMUINT3(controlVerticesZweb[controlVertex].boneIDNrs[0], controlVerticesZweb[controlVertex].boneIDNrs[1]
						, controlVerticesZweb[controlVertex].boneIDNrs[2]);

					controlVertices[controlVertex].skinWeight = DirectX::XMFLOAT3(controlVerticesZweb[controlVertex].skeletonSkinWeight[0], controlVerticesZweb[controlVertex].skeletonSkinWeight[1]
						, controlVerticesZweb[controlVertex].skeletonSkinWeight[2]);

					if (boneIDMap.find((std::string)controlVerticesZweb[controlVertex].boneID[0]) == boneIDMap.end())
					{
						boneIDMap.insert({ (std::string)controlVerticesZweb[controlVertex].boneID[0], controlVerticesZweb[controlVertex].boneIDNrs[0] });
					}
					if (boneIDMap.find((std::string)controlVerticesZweb[controlVertex].boneID[1]) == boneIDMap.end())
					{
						boneIDMap.insert({ (std::string)controlVerticesZweb[controlVertex].boneID[1], controlVerticesZweb[controlVertex].boneIDNrs[1] });
					}
					if (boneIDMap.find((std::string)controlVerticesZweb[controlVertex].boneID[2]) == boneIDMap.end())
					{
						boneIDMap.insert({ (std::string)controlVerticesZweb[controlVertex].boneID[2], controlVerticesZweb[controlVertex].boneIDNrs[2] });
					}
					controlVertices[controlVertex].position = DirectX::XMFLOAT3(controlVerticesZweb[controlVertex].pos[0], controlVerticesZweb[controlVertex].pos[1]
						, controlVerticesZweb[controlVertex].pos[2]);
				}

				std::vector<Mesh::Vertex>::iterator it;
				for (unsigned int vertex = 0; vertex < vertices.size(); vertex++) //for every vertex
				{

					it = std::find(controlVertices.begin(), controlVertices.end(), vertices[vertex]); //check if the vertex is the same as the indexed.
					//note, in the comparison function I only compare the positions.

					if (it != controlVertices.end()) //if it is the same
					{
						UINT index = (UINT)std::distance(controlVertices.begin(), it); //find wich one

						vertices[vertex].boneID = controlVertices[index].boneID; //add it to the list
						vertices[vertex].skinWeight.x = controlVertices[index].skinWeight.x;
						vertices[vertex].skinWeight.y = controlVertices[index].skinWeight.y;
						vertices[vertex].skinWeight.z = controlVertices[index].skinWeight.z;
					}



				}


			}
			
			Material mat(shader);

			cb_Material materialData; //Should this be on the heap?
			//A scene imported may contain 5 meshes but only 2 materials.
			unsigned int matIndex = importer.getMaterialIndexByName((std::string)importer.getMaterialIDInfo(mesh, 0).materialName); //a single mesh can contain multiple materials but I only support at index 0 in dX.

			materialData.ambient = DirectX::XMFLOAT4(importer.getMaterialInfo(matIndex).ka[0], importer.getMaterialInfo(matIndex).ka[1], importer.getMaterialInfo(matIndex).ka[2], 1.0f);

			materialData.diffuse = DirectX::XMFLOAT4(importer.getMaterialInfo(matIndex).kd[0], importer.getMaterialInfo(matIndex).kd[1], importer.getMaterialInfo(matIndex).kd[2], 1.0f);

			materialData.specular = DirectX::XMFLOAT4(importer.getMaterialInfo(matIndex).ks[0], importer.getMaterialInfo(matIndex).ks[1], importer.getMaterialInfo(matIndex).ks[2], importer.getMaterialInfo(matIndex).specularPower);


			//materialData.specularFactor = importer.getMaterialInfo(matIndex).specularPower;


			mat.SetMaterialData(materialData);
			
			
			

			Mesh meshObject(device, vertices, indicesZweb);


			meshObject.SetMeshName((std::string)importer.getMeshInfo(mesh).name);
				

			if (type == ZWEBLoadType::SkeletonAnimation) 
			{
				SkeletonAni skeletonAnimation;
				//map must be set first so it can be used to set up the other stuff.
				skeletonAnimation.setUpIDMapAndFrames(boneIDMap, importer.getSkeletonAnimationHeader().fps, importer.getSkeletonAnimationHeader().nrOfAnimationFrames);

				//The offset matrices are loaded in directly as matrices, the local bone space matrices are not because they need to be interpolated during runtime.
				std::vector<SkeletonOffsetsHeader> offsets;
				for (unsigned int bone = 0; bone < importer.getSkeletonAnimationHeader().nrOfBones; bone++)
				{

					offsets.push_back(importer.getSkeletonOffset(bone));


				}
				skeletonAnimation.setUpOffsetsFromMatrices(offsets);

				for (unsigned int bone = 0; bone < importer.getSkeletonAnimationHeader().nrOfBones; bone++)
				{
					std::vector<SkeletonKeysHeader> keys;
					keys = importer.getSkeletonKeys(bone);
					skeletonAnimation.setUpKeys((std::string)keys[0].linkName, keys);
				}

				
				meshObject.SetAnimationTrack(skeletonAnimation);
				
			}

			Object object((std::string)importer.getMeshInfo(mesh).name);

			

			object.AddComponent<MeshComponent>(meshObject, mat); //Should I pass in a fourth argument in the vectors???

			object.GetTransform().SetPosition(DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(importer.getMeshInfo(mesh).translation[0], importer.getMeshInfo(mesh).translation[1], importer.getMeshInfo(mesh).translation[2])));

			object.GetTransform().SetRotation(DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(importer.getMeshInfo(mesh).rotation[0], importer.getMeshInfo(mesh).rotation[1], importer.getMeshInfo(mesh).rotation[2])));

			object.GetTransform().SetScale(DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(importer.getMeshInfo(mesh).scale[0], importer.getMeshInfo(mesh).scale[1], importer.getMeshInfo(mesh).scale[2])));


			objects.push_back(object);
		}

		return objects;
		
	}
	
}