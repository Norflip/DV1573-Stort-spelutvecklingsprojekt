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

	inline SkeletonAni LoadSkeletonOnly( std::string animationPath, std::map<std::string, unsigned int>& boneIDMap)
	{
		ZWEB::ZWEBImporter importer;

		

		importer.importAnimation(animationPath);

		SkeletonAni skeletonAnimation;
		//map must be set first so it can be used to set up the other stuff.
		skeletonAnimation.SetUpIDMapAndFrames(boneIDMap, importer.getSkeletonAnimationHeader().fps, importer.getSkeletonAnimationHeader().nrOfAnimationFrames);

		//The offset matrices are loaded in directly as matrices, the local bone space matrices are not because they need to be interpolated during runtime.
		std::vector<SkeletonOffsetsHeader> offsets;
		for (unsigned int bone = 0; bone < importer.getSkeletonAnimationHeader().nrOfBones; bone++)
		{

			offsets.push_back(importer.getSkeletonOffset(bone));


		}
		skeletonAnimation.SetUpOffsetsFromMatrices(offsets);

		for (unsigned int bone = 0; bone < importer.getSkeletonAnimationHeader().nrOfBones; bone++)
		{
			std::vector<SkeletonKeysHeader> keys;
			keys = importer.getSkeletonKeys(bone);
			skeletonAnimation.SetUpKeys((std::string)keys[0].linkName, keys);
		}

		return skeletonAnimation;




	}
	inline std::vector<Mesh> LoadMeshes(ZWEBLoadType type, std::string scenePath, ID3D11Device* device)
	{

		ZWEB::ZWEBImporter importer;

		importer.importScene(scenePath);

		

		std::vector<Mesh> meshes;
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
			boneIDMap.clear();

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

			

			Mesh meshObject(device, vertices, indicesZweb);
			


			meshObject.SetMeshName((std::string)importer.getMeshInfo(mesh).name);

			meshObject.SetBoneIDS(boneIDMap);


			dx::XMFLOAT3 scale = { (importer.getMeshInfo(mesh).scale[0]), (importer.getMeshInfo(mesh).scale[1]), (importer.getMeshInfo(mesh).scale[2]) };
			dx::XMFLOAT3 rotation = { (importer.getMeshInfo(mesh).rotation[0]), (importer.getMeshInfo(mesh).rotation[1]), (importer.getMeshInfo(mesh).rotation[2]) };
			dx::XMFLOAT3 translation = { (importer.getMeshInfo(mesh).translation[0]), (importer.getMeshInfo(mesh).translation[1]), (importer.getMeshInfo(mesh).translation[2]) };

				

			meshObject.SetSRT(scale, rotation, translation);



			meshes.push_back(meshObject);
		}

		return meshes;




	}


	inline std::vector<Material> LoadMaterials(std::string scenePath, const Shader& shader, ID3D11Device* device) //Each mesh has a material name there might be 5 meshes and 3 materials.
	{
		ZWEB::ZWEBImporter importer;

		importer.importScene(scenePath);

		std::vector<Material> materials;

		std::string diffuseTName = "NULL";

		std::string normalTName = "NULL";

		std::string opacityName = "NULL";

		std::string emissiveName = "NULL";

		
		for (unsigned short material = 0; material < importer.getSceneInfo().nrOfMaterials; material++)
		{

			Material mat(shader);

			cb_Material materialData; 
			
			
			materialData.ambient = DirectX::XMFLOAT4(importer.getMaterialInfo(material).ka[0], importer.getMaterialInfo(material).ka[1], importer.getMaterialInfo(material).ka[2], 1.0f);

			materialData.diffuse = DirectX::XMFLOAT4(importer.getMaterialInfo(material).kd[0], importer.getMaterialInfo(material).kd[1], importer.getMaterialInfo(material).kd[2], 1.0f);

			materialData.specular = DirectX::XMFLOAT4(importer.getMaterialInfo(material).ks[0], importer.getMaterialInfo(material).ks[1], importer.getMaterialInfo(material).ks[2], importer.getMaterialInfo(material).specularPower);// if the material is lambert and not Phong then this is default 0.

			mat.SetName(importer.getMaterialInfo(material).name);


			diffuseTName = importer.getMaterialInfo(material).albedoMapName;

			normalTName = importer.getMaterialInfo(material).normalMapName;

			opacityName = importer.getMaterialInfo(material).opacityMapName;

			emissiveName = importer.getMaterialInfo(material).emissiveMapName; //Cannot export displacement, this must be done manually.


			if (diffuseTName != " ") //if a texture does not exist, this is it's name.
			{
				Texture texture;
				std::string path = "Textures/" + diffuseTName; //Using a fixed path so that you don't need to export the texture in the right folder, just place it manually in here instead.
				std::wstring pathWSTR(path.begin(), path.end());
				
				bool success = texture.LoadTexture(device, pathWSTR.c_str());
				assert(success);

				mat.SetTexture(texture, 0, ShaderBindFlag::PIXEL);
				materialData.hasAlbedo = 1;
			}
			else
			{
				materialData.hasAlbedo = 0;
			}
			if (normalTName != " ")
			{
				Texture texture;
				std::string path = "Textures/" + normalTName;
				std::wstring pathWSTR(path.begin(), path.end());
				bool success = texture.LoadTexture(device, pathWSTR.c_str());
				assert(success);


				mat.SetTexture(texture, 1, ShaderBindFlag::PIXEL); //This is default but can be manually changed afterwards.

				
				materialData.hasNormalMap = 1;
			}
			else
			{
				materialData.hasNormalMap = 0;

			}
			if (opacityName != " ")
			{
				Texture texture;
				std::string path = "Textures/" + opacityName;
				std::wstring pathWSTR(path.begin(), path.end());
				bool success = texture.LoadTexture(device, pathWSTR.c_str());
				assert(success);


				mat.SetTexture(texture, 1, ShaderBindFlag::PIXEL); //This is default but can be manually changed afterwards.


				
			}





			mat.SetMaterialData(materialData);

			materials.push_back(mat);

		}


		return materials;



	}




	
	
}