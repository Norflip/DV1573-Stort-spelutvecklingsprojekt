#include "pch.h"
#include "ImportZWEB.h"
namespace ZWEB
{

	SectionHeader m_sectionheader(ZWEBFILE::Section);
	MainHeader m_mainheader;
	std::vector<MeshHeader> m_meshes;
	std::vector<MaterialHeader> m_materials;
	std::vector<std::vector<MaterialIDHeader>> m_materialIDS;
	std::vector<TextureHeader> m_textures;
	std::vector<std::vector<VertexHeader>> m_vertices;
	std::vector<std::vector<unsigned int>> m_indices;
	MorphAnimationHeader m_morphAnimationHeader;
	std::vector<MorphTargetHeader> m_morphtargets;
	std::vector<std::vector<MorphKeyFrameHeader>> m_morphKeyFrames;
	std::vector<std::vector<MorphVertexHeader>> m_morphVertexAttributes;
	std::vector<std::vector<unsigned int>> m_morphIndices;
	std::vector<LightHeader> m_lights;
	std::vector<CameraHeader> m_cameras;
	std::string m_errorCode;
	std::vector<std::vector<VertexHeader>> m_controlPoints;
	SkeletonAnimationHeader m_skeletonAnimationHeader;
	std::vector<SkeletonOffsetsHeader> m_skeletonOffsets;
	std::vector<std::vector<SkeletonKeysHeader>> m_SkeletonKeys;
	std::vector<CameraKeyFramesHeader> m_cameraKeyFrames;
	std::map<std::string, unsigned int> m_meshIndexByName;
	std::map<std::string, unsigned int> m_morphIndexByName;
	std::map<std::string, unsigned int> m_materialIndexByName;
	std::map<std::string, unsigned int> m_textureIndexByName;
	std::vector<WayPointHeader> m_wayPoints;
	DummyAnimationHeader m_dummyAnimation;
	std::vector<DummyAnimationFrameHeader> m_dummyAnimationFrames;
	unsigned int m_nrOfCameraKeys = 0;
	bool ZWEBImporter::importScene(std::string path)
	{
		m_errorCode = " ";
		int meshcount = 0;
		int materialCount = 0;
		int lightCount = 0;
		char* ptr= nullptr;
		bool result = true;
		int cameraCount = 0;
		int textureCount = 0;
		int wayPointCount = 0;
		std::ifstream file(path, std::ifstream::binary | std::ifstream::in);
		if (file.is_open())
		{
			
			
			
			while (file)
			{

				ptr = (char*)&m_sectionheader;
				
				file.read(ptr, sizeof(SectionHeader));
				
				switch (m_sectionheader.type)
				{
				default:
					break;

				case(ZWEBFILE::Main):
					
					ptr = (char*)&m_mainheader;
					ptr += sizeof(SectionHeader);
					file.read(ptr, sizeof(MainHeader) - sizeof(SectionHeader));
					m_meshes.resize(m_mainheader.nrOfMeshes);
					m_vertices.resize(m_mainheader.nrOfMeshes);
					m_indices.resize(m_mainheader.nrOfMeshes);
					m_materials.resize(m_mainheader.nrOfMaterials);
					m_materialIDS.resize(m_mainheader.nrOfMeshes);
					m_lights.resize(m_mainheader.nrOfLights);
					m_cameras.resize(m_mainheader.nrOfCameras);
					m_textures.resize(m_mainheader.nrOfTextures);
					m_controlPoints.resize(m_mainheader.nrOfMeshes);
					m_wayPoints.resize(m_mainheader.nrOfWayPoints);
					break;

				case(ZWEBFILE::Mesh):

					ptr = (char*)&m_meshes[meshcount];
					ptr += sizeof(SectionHeader);
					file.read(ptr, sizeof(MeshHeader) - sizeof(SectionHeader));
					m_vertices[meshcount].resize(m_meshes[meshcount].nrOfVertices);


					for (int i = 0; i < m_vertices[meshcount].size(); i++)
					{
						ptr = (char*)&m_vertices[meshcount][i];
						file.read(ptr, (sizeof(VertexHeader)));
					}



					m_indices[meshcount].resize(m_meshes[meshcount].nrOfindices);


					for (int i = 0; i < m_indices[meshcount].size(); i++)
					{
						ptr = (char*)&m_indices[meshcount][i];
						file.read(ptr, (sizeof(unsigned int)));
					}


					m_materialIDS[meshcount].resize(m_meshes[meshcount].nrOfMeshMaterials);

					for (int i = 0; i < m_meshes[meshcount].nrOfMeshMaterials; i++)
					{
						ptr = (char*)&m_materialIDS[meshcount][i];
						file.read(ptr, (sizeof(MaterialIDHeader)));
					}

					m_controlPoints[meshcount].resize(m_meshes[meshcount].nrOfControlVertices);

					for (unsigned int i = 0; i < m_meshes[meshcount].nrOfControlVertices; i++)
					{
						ptr = (char*)&m_controlPoints[meshcount][i];

						file.read(ptr, (sizeof(VertexHeader)));

					}
					
					m_meshIndexByName.insert({ (std::string)m_meshes[meshcount].name, meshcount });
					meshcount++;
					break;

				case(ZWEBFILE::Material):

					ptr = (char*)&m_materials[materialCount];
					ptr += sizeof(SectionHeader);

					file.read(ptr, (sizeof(MaterialHeader)) - sizeof(SectionHeader));



					m_materialIndexByName.insert({ (std::string)m_materials[materialCount].name, materialCount });
					materialCount++;
					break;

				case(ZWEBFILE::Texture):

					ptr = (char*)&m_textures[textureCount];
					ptr += sizeof(SectionHeader);

					file.read(ptr, sizeof(TextureHeader) - sizeof(SectionHeader));


					m_textureIndexByName.insert({ (std::string)m_textures[textureCount].path, textureCount });

					textureCount++;
					break;

				case(ZWEBFILE::Light):

					ptr = (char*)&m_lights[lightCount];
					ptr += sizeof(SectionHeader);

					file.read(ptr, sizeof(LightHeader) - sizeof(SectionHeader));
					lightCount++;
					break;



				case(ZWEBFILE::Camera):

					ptr = (char*)&m_cameras[cameraCount];
					ptr += sizeof(SectionHeader);
					file.read(ptr, sizeof(CameraHeader) - sizeof(SectionHeader));
					m_nrOfCameraKeys = m_cameras[cameraCount].nrOfKeys; //Will be constantly overriden if there are more cameras.
					cameraCount++;
					break;

				case(ZWEBFILE::Waypoint):

					ptr= (char*)&m_wayPoints[wayPointCount];
					ptr += sizeof(SectionHeader);
					file.read(ptr, sizeof(WayPointHeader) - sizeof(SectionHeader));
					wayPointCount++;
					break;

				}
				
				if (meshcount == m_mainheader.nrOfMeshes && materialCount == m_mainheader.nrOfMaterials
					&& lightCount == m_mainheader.nrOfLights &&cameraCount==m_mainheader.nrOfCameras&&textureCount==m_mainheader.nrOfTextures
					&& wayPointCount==m_mainheader.nrOfWayPoints)
				{
					break;
				}
				

			}





			file.close();
		}
		else
		{
			result = false;
			m_errorCode += "\nCouldn't load file.";
		}

		
		
		return result;
		
	}

	bool ZWEBImporter::importAnimation(std::string path)
	{
		
		int morphCount = 0;
		char* ptr = nullptr;
		bool result = true;
		std::ifstream file(path, std::ifstream::binary | std::ifstream::in);
		if (file.is_open())
		{



			while (file)
			{

				ptr = (char*)&m_sectionheader;

				file.read(ptr, sizeof(SectionHeader));

				switch (m_sectionheader.type)
				{
				default:
					break;

				case(ZWEBFILE::CameraAnimation):

					m_cameraKeyFrames.resize(m_nrOfCameraKeys);
					for (unsigned int nrOfCamKey = 0; nrOfCamKey < m_nrOfCameraKeys; nrOfCamKey++)
					{
						ptr = (char*)&m_cameraKeyFrames[nrOfCamKey];
						ptr += sizeof(SectionHeader);

						file.read(ptr, (sizeof(CameraKeyFramesHeader)) - sizeof(SectionHeader));

					}
					

					break;

				case(ZWEBFILE::Morph):

					ptr = (char*)&m_morphAnimationHeader; //läs båda filer i samma anrop
					ptr += sizeof(SectionHeader);

					file.read(ptr, (sizeof(MorphAnimationHeader)) - sizeof(SectionHeader));
					m_morphKeyFrames.resize(m_morphAnimationHeader.nrOfBlendShapes);
					m_morphtargets.resize(m_morphAnimationHeader.nrOfBlendShapes);
					m_morphVertexAttributes.resize(m_morphAnimationHeader.nrOfBlendShapes);
					m_morphIndices.resize(m_morphAnimationHeader.nrOfBlendShapes);
					for (int i = 0; i < m_morphAnimationHeader.nrOfBlendShapes; i++)
					{
						ptr = (char*)&m_morphtargets[i];
						file.read(ptr, (sizeof(MorphTargetHeader)));
						m_morphVertexAttributes[i].resize(m_morphtargets[i].nrOfVertices);
						for (unsigned int j = 0; j < m_morphtargets[i].nrOfVertices; j++)
						{
							ptr = (char*)&m_morphVertexAttributes[i][j];
							file.read(ptr, (sizeof(MorphVertexHeader)));
						}
						m_morphIndices[i].resize(m_morphtargets[i].nrOfIndices);

						for (unsigned int j = 0; j < m_morphtargets[i].nrOfIndices; j++)
						{
							ptr = (char*)&m_morphIndices[i][j];
							file.read(ptr, (sizeof(unsigned int)));
						}

						m_morphKeyFrames[i].resize(m_morphtargets[i].nrOfKeyFrames);
						for (unsigned int j = 0; j < m_morphtargets[i].nrOfKeyFrames; j++)
						{
							ptr = (char*)&m_morphKeyFrames[i][j];
							file.read(ptr, (sizeof(MorphKeyFrameHeader)));
						}

						m_morphIndexByName.insert({ (std::string)m_morphtargets[i].blendShape, i });
					}
					
					morphCount++;
					break;




				case(ZWEBFILE::Skeleton): 

					ptr = (char*)&m_skeletonAnimationHeader;
					ptr += sizeof(SectionHeader);
					file.read(ptr, (sizeof(SkeletonAnimationHeader)) - sizeof(SectionHeader));
					
					m_skeletonOffsets.resize(m_skeletonAnimationHeader.nrOfBones);
					m_SkeletonKeys.resize(m_skeletonAnimationHeader.nrOfBones);
					for (int i = 0; i < m_skeletonAnimationHeader.nrOfBones; i++)
					{
						ptr = (char*)&m_skeletonOffsets[i];
						file.read(ptr, (sizeof(SkeletonOffsetsHeader)));
					}
					for (int i = 0; i < m_skeletonAnimationHeader.nrOfBones; i++)
					{
						m_SkeletonKeys[i].resize(m_skeletonAnimationHeader.nrOfKeyFrames);
						for (unsigned int j = 0; j < m_skeletonAnimationHeader.nrOfKeyFrames; j++)
						{
							ptr = (char*)&m_SkeletonKeys[i][j];
							file.read(ptr, (sizeof(SkeletonKeysHeader)));
						}
					}

					break;


				case(ZWEBFILE::DummyAnimation):

					ptr = (char*)&m_dummyAnimation;
					ptr += sizeof(SectionHeader);
					file.read(ptr, (sizeof(DummyAnimationHeader)) - sizeof(SectionHeader));

					m_dummyAnimationFrames.resize(m_dummyAnimation.nrOfFrames);

					for (int i = 0; i < m_dummyAnimation.nrOfFrames; i++)
					{
						ptr = (char*)&m_dummyAnimationFrames[i];
						file.read(ptr, (sizeof(DummyAnimationFrameHeader)));
					}


					break;
				}

				


			}





			file.close();
		}
		else
		{
			result = false;
			m_errorCode += "\nCouldn't load file.";
		}



		return result;
	}
	
	const MainHeader& ZWEBImporter::getSceneInfo()
	{
		return m_mainheader;
	}

	const unsigned int ZWEBImporter::getMeshIndexByName(std::string meshName)
	{

		return m_meshIndexByName[meshName];
	}

	const MeshHeader& ZWEBImporter::getMeshInfo(unsigned int meshIndex)
	{
		return m_meshes[meshIndex];
	}

	

	std::vector<VertexHeader>& ZWEBImporter::fillVertices(unsigned int meshIndex)
	{
		return m_vertices[meshIndex];
	}

	std::vector<unsigned int>& ZWEBImporter::getIndices(unsigned int meshIndex)
	{
		return m_indices[meshIndex];
	}

	
	const MaterialHeader& ZWEBImporter::getMaterialInfo(unsigned int materialIndex)
	{
		return m_materials[materialIndex];
	}
	const unsigned int ZWEBImporter::getMaterialIndexByName(std::string materialName)
	{
		return m_materialIndexByName[materialName];
	}
	const MaterialIDHeader& ZWEBImporter::getMaterialIDInfo(unsigned int meshIndex, unsigned int materialIndex)
	{
		return m_materialIDS[meshIndex][materialIndex];
	}
	const MorphAnimationHeader& ZWEBImporter::getMorphAnimationInfo()
	{
		return m_morphAnimationHeader;
	}
	
	const MorphTargetHeader& ZWEBImporter::getTarget(unsigned int morphTargetIndex)
	{
		if (morphTargetIndex < m_morphKeyFrames.size())
		{
			return m_morphtargets[morphTargetIndex];
		}
		else
		{
			m_errorCode += "\nMorphTarget Index out of Bounds";
			return m_morphtargets[0];

		}
	}

	std::vector<MorphVertexHeader>& ZWEBImporter::fillMorphVertices(unsigned int morphTargetIndex)
	{
		return m_morphVertexAttributes[morphTargetIndex];
	}
	const unsigned int ZWEBImporter::getMorphIndexByName(std::string morphTargetName)
	{
		return m_morphIndexByName[morphTargetName];
	}
	
	std::vector<MorphKeyFrameHeader>& ZWEBImporter::getMorphKeys(unsigned int morphTargetIndex)
	{
		return m_morphKeyFrames[morphTargetIndex];
	}
	std::vector<unsigned int>& ZWEBImporter::getMorphIndices(unsigned int morphTargetIndex)
	{
		return m_morphIndices[morphTargetIndex];
	}
	
	const LightHeader& ZWEBImporter::getLightInfo(unsigned int lightIndex)
	{
		return m_lights[lightIndex];
	}

	const CameraHeader& ZWEBImporter::getCamera(unsigned int cameraIndex)
	{
		return m_cameras[cameraIndex];
	}

	const std::vector<CameraKeyFramesHeader>& ZWEBImporter::getCameraKeys()
	{
		return m_cameraKeyFrames;
	}

	const char* ZWEBImporter::getErrorCode()
	{
		if (m_errorCode == " " || m_errorCode == "No errors to report")
		{
			m_errorCode = "No errors to report";
		}
		return m_errorCode.c_str(); //returnera const char*
		
	}
	const TextureHeader& ZWEBImporter::getTextureInfo(unsigned int textureIndex)
	{
		return m_textures[textureIndex];
	}

	const unsigned int ZWEBImporter::getTextureIndexByName(std::string textureName)
	{
		return m_textureIndexByName[textureName];
	}
	
	std::vector<VertexHeader>& ZWEBImporter::getControlPoints(unsigned int meshIndex)
	{
		return m_controlPoints[meshIndex];
	}
	const SkeletonAnimationHeader& ZWEBImporter::getSkeletonAnimationHeader()
	{
		return m_skeletonAnimationHeader;
	}
	
	const SkeletonOffsetsHeader& ZWEBImporter::getSkeletonOffset(unsigned int i)
	{
		return m_skeletonOffsets[i];
	}
	std::vector<SkeletonKeysHeader>& ZWEBImporter::getSkeletonKeys(unsigned int boneIndex)
	{
		return m_SkeletonKeys[boneIndex];
	}

	std::vector<WayPointHeader>& ZWEBImporter::getWayPoints()
	{
		return m_wayPoints;
	}

	const DummyAnimationHeader& ZWEBImporter::getDummyAnimation()
	{
		return m_dummyAnimation;
	}

	const std::vector<DummyAnimationFrameHeader>& ZWEBImporter::getDummyFrames()
	{
		return m_dummyAnimationFrames;
	}
	
}