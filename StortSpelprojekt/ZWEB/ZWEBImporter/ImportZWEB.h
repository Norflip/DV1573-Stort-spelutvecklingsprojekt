#pragma once
#include "Headers.h"
#include <vector>
#include <string>
#include <map>
#include <fstream>





namespace ZWEB
{
	class ZWEBImporter
	{
	public:

		//IMPORT FUNKTIONER:


		//Import the files content except animation data. Must be used before accessing the data.
		bool importScene(std::string path);

		//Import animation data, such as blendshapes, keyframes, bones etc.
		bool importAnimation(std::string path);

		//SCENE DATA:

		//Get general information about what is in the file.
		const MainHeader& getSceneInfo();

		//MESH DATA:
		
		//Returns index for mesh in scene
		const unsigned int getMeshIndexByName(std::string meshName);


		//Get general info about what is in the mesh, how many vertices etc.
		const MeshHeader& getMeshInfo(unsigned int meshIndex);


		
		//Get vertexdata for a specific mesh.
		std::vector<VertexHeader>& fillVertices(unsigned int meshIndex);

		
		
		//Get index data for a specific mesh.
		std::vector<unsigned int>& getIndices(unsigned int meshIndex);



		//Get material data per mesh, both the material name and the mesh name it belongs too.
		//Materialindex is 0 if there is only one material per mesh.
		const MaterialIDHeader& getMaterialIDInfo(unsigned int meshIndex, unsigned int materialIndex);




		//MATERIAL DATA:

		//Get general information about the material.
		const MaterialHeader& getMaterialInfo(unsigned int materialIndex);

		//Returns materialindex for the material in the scene.
		const unsigned int getMaterialIndexByName(std::string materialName);

		//Get information about a texture, name and path etc.
		const TextureHeader& getTextureInfo(unsigned int textureIndex);

		//Get textureindex for the texture in the scene, example: "name.jpg".
		const unsigned int getTextureIndexByName(std::string textureName);



		//MORPH ANIMATION DATA:

		//Get the blendshapeindex by name
		const unsigned int getMorphIndexByName(std::string morphTargetName);

		//Get general information about the morph animation.
		const MorphAnimationHeader& getMorphAnimationInfo();

		//Get information about how many keyframes and control points the specific morph target has etc.
		const MorphTargetHeader& getTarget(unsigned int morphTargetIndex);
		
		//Get control points for a morph target via morph target indice.
		std::vector<MorphVertexHeader>& fillMorphVertices(unsigned int morphTargetIndex);

		

		
	
		
		//Get keyframes beloning to a morphtarget, it's weights and time stamp.
		std::vector<MorphKeyFrameHeader>& getMorphKeys(unsigned int morphTargetIndex);


		
		
		//Get the morph target control point indices.
		std::vector<unsigned int>& getMorphIndices(unsigned int morphTargetIndex);



		//LIGHT/CAMERA DATA:

		//Get information about a light, what type it is etc.
		const LightHeader& getLightInfo(unsigned int lightIndex);
		
		//Get camera information, up and forward vector etc.
		const CameraHeader& getCamera(unsigned int cameraIndex);

		//Get camera animation data
		const std::vector<CameraKeyFramesHeader>& getCameraKeys(); //always camera at slot 0 I only support 1 camera.


		//SKELETON DATA:
		
		
		//Get the controlpoints of the mesh.
		std::vector<VertexHeader>& getControlPoints(unsigned int meshIndex);

		//Get general information about the skeletonAnimation.
		const SkeletonAnimationHeader& getSkeletonAnimationHeader();
		
		//Get offset matrices from the skeleton
		const SkeletonOffsetsHeader& getSkeletonOffset(unsigned int boneIndex);
		
		//Get data from specific keyframes from the skeleton.
		std::vector<SkeletonKeysHeader>& getSkeletonKeys(unsigned int boneIndex);

		//WayPointData
		//Read Waypoints from file
		std::vector<WayPointHeader>& getWayPoints();

		
		//Dummy Animation, this simply is an exported mesh/node, with scale, rotatation and translation attached to it, during a lenght of time.

		const DummyAnimationHeader& getDummyAnimation();
		const std::vector<DummyAnimationFrameHeader>& getDummyFrames();

		//Get ERROR MESSAGES:
		// Get a hint of what might have gone wrong.
		const char* getErrorCode();
		
	};


}