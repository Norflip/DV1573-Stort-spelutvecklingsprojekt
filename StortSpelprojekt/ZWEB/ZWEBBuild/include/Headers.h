#pragma once
namespace ZWEBFILE {

	enum SectionType
	{
		Section,
		Main,
		Mesh,
		Material,
		Light,
		Morph,
		Camera,
		Texture,
		Vertex,
		Index,
		MaterialID,
		Skeleton,
		Waypoint,
		CameraAnimation,
		VertexAnimation,
		DummyAnimation
	};



enum MaterialType
{
	Lambert, //bara phong har specular
	Phong
};

enum TextureType
{
	Diffuse,
	Normal,
	Emissive,
	Bump,
	Specular,
	Rough,
	AO,
	Metal,
	Opacity


};
enum LightType
{
	PointLight,
	DirectionalLight,
	SpotLight,
	AreaLight,
	VolumeLight
};
enum AreaLightShape {
	Rectangle,
	Sphere
};
enum VertexType
{
	NoAnimation,
	AllAnimation,
	BlendShapeAnimation,
	SkeletonAnimation
};
}
struct SectionHeader
{
	ZWEBFILE::SectionType type;
	SectionHeader(ZWEBFILE::SectionType type)
	{
		this->type = type;
	}


};

struct MainHeader : SectionHeader
{
	
	unsigned short int nrOfMeshes;
	unsigned short int nrOfLights;
	unsigned short int nrOfCameras;
	unsigned short int nrOfMaterials;
	unsigned short int nrOfTextures;
	unsigned int nrOfWayPoints;
	MainHeader() : SectionHeader(ZWEBFILE::Main)
	{
		nrOfCameras = 0;
		nrOfLights = 0;
		nrOfMeshes = 0;
		nrOfMaterials = 0;
		nrOfTextures = 0;
		nrOfWayPoints = 0;
	}
};

struct VertexHeader
{
	
	float pos[3];
	float normal[3];
	float uv[2];
	float biNormal[3];
	float tangent[3];
	unsigned short int boneIDNrs[5];
	char boneID[5][100] = { {' '} }; //om vertextypen inte stödjer detta så används aldrig dessa helt enkelt.
	float skeletonSkinWeight[5]; //Man kan exportera med högst 5 tror jag men det är dåligt för prestanda att ha mer än 3 typ.
	
	VertexHeader()
	{
		pos[0] = 0;
		pos[1] = 0;
		pos[2] = 0;

		normal[0] = 0;
		normal[1] = 0;
		normal[2] = 0;

		uv[0] = 0;
		uv[1] = 0;

		biNormal[0] = 0;
		biNormal[1] = 0;
		biNormal[2] = 0;

		tangent[0] = 0;
		tangent[1] = 0;
		tangent[2] = 0;

		

		skeletonSkinWeight[0] = 0;
		skeletonSkinWeight[1] = 0;
		skeletonSkinWeight[2] = 0;
		skeletonSkinWeight[3] = 0;
		skeletonSkinWeight[4] = 0;

		boneIDNrs[0] = 0;
		boneIDNrs[1] = 0;
		boneIDNrs[2] = 0;
		boneIDNrs[3] = 0;
		boneIDNrs[4] = 0;
		
	}
};

struct MeshHeader : SectionHeader
{
	ZWEBFILE::VertexType vertexType;
	char name[100] = {' '};
	unsigned int nrOfVertices;
	unsigned int nrOfindices;
	unsigned short nrOfMeshMaterials;
	unsigned short nrOfMeshTextures;
	unsigned short animationLodlevel; //shorts lägg ner i strukten på grund av padding.
	unsigned int nrOfControlVertices;
	float center[3];
	float min[3];
	float max[3];
	float rotation[3];
	float scale[3];
	float translation[3];
	
	MeshHeader() : SectionHeader(ZWEBFILE::Mesh)
	{
		
		vertexType = ZWEBFILE::NoAnimation;
		nrOfindices = 0;
		nrOfMeshMaterials = 0;
		nrOfVertices = 0;
		animationLodlevel = 0;
		nrOfControlVertices = 0;
		center[0] = 0.0f;
		center[1] = 0.0f;
		center[2] = 0.0f;
		min[0] = 0.0f;
		min[1] = 0.0f;
		min[2] = 0.0f;
		max[0] = 0.0f;
		max[1] = 0.0f;
		max[2] = 0.0f;
		rotation[0] = 0.0f;
		rotation[1] = 0.0f;
		rotation[2] = 0.0f;
		scale[0] = 0.0f;
		scale[1] = 0.0f;
		scale[2] = 0.0f;
		translation[0] = 0.0f;
		translation[1] = 0.0f;
		translation[2] = 0.0f;
		
	}
	

};
struct MaterialIDHeader
{
	char materialName[100] = {' '};
	char meshName[100] = {' '};
	
	MaterialIDHeader()
	{
		
	}

};
struct MaterialHeader : SectionHeader
{
	ZWEBFILE::MaterialType mType;
	float ka[3];
	float kd[3];
	float ks[3];
	float kr[3];
	float specularPower;
	float reflectivepower;
	char name[100] = { ' ' };
	char albedoMapName[200] = { ' ' };
	char normalMapName[200] = { ' ' };
	char opacityMapName[200] = { ' ' };
	char emissiveMapName[200] = { ' ' };
	MaterialHeader() :SectionHeader(ZWEBFILE::Material)
	{
		mType = ZWEBFILE::Phong;
		specularPower = 0;
		reflectivepower = 0;
		for (int i = 0; i < 3; i++)
		{
			ka[i] = 0;
			kd[i] = 0;
			ks[i] = 0;
			kr[i] = 0;
		}

	}

};

struct LightHeader : SectionHeader
{
	//All Lights
	ZWEBFILE::LightType lightType;
	float color[3];
	float intensity;

	//Directional Light
	float lightDirection[3];

	//Point Light
	float position[3];

	//Spotlight
	//spotDirection (same as directional's lightDirection)
	float cutoffAngle;
	float rotation[3];

	//Area
	ZWEBFILE::AreaLightShape shape;

	//Volume ej implementerad än


	LightHeader() :SectionHeader(ZWEBFILE::Light)
	{
		lightType = ZWEBFILE::PointLight;
		color[0] = 1.0f;
		color[1] = 1.0f;
		color[2] = 1.0f;
		intensity = 100.0f;
		cutoffAngle = 45.0f;
		shape = ZWEBFILE::AreaLightShape::Rectangle;

		for (int i = 0; i < 3; i++) {
			lightDirection[i] = 0.0f;
			position[i] = 0.0f;
			rotation[i] = 0.0f;
		}
	}
};

struct MorphAnimationHeader : SectionHeader
{
	float nrOfAnimationFrames;
	unsigned int nrOfKeyFrames;
	float fps;
	unsigned short nrOfBlendShapes;
	char animationName[100] = {' '};

	MorphAnimationHeader() : SectionHeader(ZWEBFILE::Morph)
	{
		nrOfBlendShapes = 0;
		fps = 0;
		nrOfKeyFrames = 0;
		nrOfAnimationFrames = 0;
	}
};



struct MorphTargetHeader
{
	char targetMesh[100] = { ' ' };
	char blendShape[100] = { ' ' };
	unsigned int nrOfKeyFrames; 
	unsigned int nrOfVertices;
	unsigned int nrOfNormals;
	unsigned int nrOfIndices;
	unsigned int nrOfNormalIndices;
};
struct MorphVertexHeader
{
	
	float morphAttribute[3];
	
};
struct MorphNormalHeader
{
	float biNormalAttr[3];
	float tangentAttr[3];
	float morphNormalAttr[3];
	
};
struct MorphKeyFrameHeader
{
	float currentWeight;
	float currentKeyFrameIndex;
	
	
};




struct CameraHeader : SectionHeader
{
	char name[100] = { ' ' };
	float position[3];
	float up[3];
	float forward[3];
	float right[3];
	float FOV;
	float nearD;
	float farD;
	unsigned int nrOfKeys;
	float aniLenght;
	float fps;
	CameraHeader() : SectionHeader(ZWEBFILE::Camera)
	{
		position[0] = 0.0f;
		up[0] = 0.0f;
		forward[0] = 0.0f;
		position[1] = 0.0f;
		up[1] = 0.0f;
		forward[1] = 0.0f;
		position[2] = 0.0f;
		up[2] = 0.0f;
		forward[2] = 0.0f;
		right[0] = 0.0f;
		right[1] = 0.0f;
		right[2] = 0.0f;
		FOV = 0.0f;
		nearD = 0.0f;
		farD = 0.0f;
		nrOfKeys = 0;
		aniLenght = 0.0f;
		fps = 0.0f;
	}
};

struct CameraKeyFramesHeader : SectionHeader
{
	float t[4];
	float r[4];
	float time;
	CameraKeyFramesHeader() : SectionHeader(ZWEBFILE::CameraAnimation)
	{
		time = 0.0f;
		t[0] = 0.0f;
		r[0] = 0.0f;
		t[1] = 0.0f;
		r[1] = 0.0f;
		t[2] = 0.0f;
		r[2] = 0.0f;
		t[3] = 0.0f;
		r[3] = 0.0f;
	}



};


struct SkeletonAnimationHeader : SectionHeader
{
	char meshName[100] = { ' ' };
	float nrOfAnimationFrames;
	unsigned int nrOfKeyFrames;
	float fps;
	unsigned short nrOfBones;
	SkeletonAnimationHeader() : SectionHeader(ZWEBFILE::Skeleton)
	{
		fps = 0;
		nrOfAnimationFrames = 0;
		nrOfKeyFrames = 0;
		nrOfBones = 0;
	}


};



struct SkeletonOffsetsHeader
{
	unsigned short int linkNr;
	char linkName[100] = {' '};
	char parentName[100] = { ' ' };
	float m[16] =
	{0,0,0,0
	,0,0,0,0,
	0,0,0,0,
	0,0,0,0};
	float r[4] = { 0,0,0,0 };
	float t[4] = { 0,0,0,0 };
	
	
};

struct SkeletonKeysHeader
{
	unsigned short linkNr; //en transformationsmatris innehåller massa data, men jag tror dessa två räcker. Kanske måste bygga offset från detta så det är likt.
	char linkName[100] = { ' ' };
	char parentName[100] = { ' ' };
	float r[4] = {0,0,0,0};
	float t[4]={ 0,0,0,0 };
	float currentKeyFrameIndex;
};

struct TextureHeader : SectionHeader
{
	char path[200] = { ' ' };
	char name[100] = { ' ' };
	ZWEBFILE::TextureType textureType;

	TextureHeader() : SectionHeader(ZWEBFILE::Texture)
	{
		textureType = ZWEBFILE::Diffuse;
	}

};

struct WayPointHeader : SectionHeader
{
	int IDNr;
	char ID[100] = {' '};
	float pos[3];
	char edges[5][100] = {' '};
	float weights[5];
	float lenght[5];
	WayPointHeader() : SectionHeader(ZWEBFILE::Waypoint)
	{
	
		pos[0] = 0.0f;
		pos[1] = 0.0f;
		pos[2] = 0.0f;
		

		weights[0] = 0.0f;
		weights[1] = 0.0f;
		weights[2] = 0.0f;
		weights[3] = 0.0f;
		weights[4] = 0.0f;

		lenght[0] = 0.0f;
		lenght[1] = 0.0f;
		lenght[2] = 0.0f;
		lenght[3] = 0.0f;
		lenght[4] = 0.0f;

		IDNr = -1;
	}



	
};

struct VertexAnimationHeader : SectionHeader
{
	unsigned short nrOfTargets;

	VertexAnimationHeader() : SectionHeader(ZWEBFILE::VertexAnimation)
	{
		nrOfTargets = 0;
	}
};



struct VertexAnimationTargetHeader
{
	unsigned int nrOfVertices = 0;
	float frame = 0.0f;
};


struct VertexAnimationVertHeader
{
	float pos[3] = {0.0f, 0.0f,0.0f};
};




struct DummyAnimationHeader : SectionHeader
{
	unsigned int nrOfFrames;
	float fps;
	float lenght;
	DummyAnimationHeader() : SectionHeader(ZWEBFILE::DummyAnimation)
	{
		nrOfFrames = 0;
		fps = 0.0f;
		lenght = 0.0f;
	}
};


struct DummyAnimationFrameHeader
{
	float s[4] = {0,0,0,0};
	float r[4] = { 0,0,0,0 };
	float t[4] = { 0,0,0,0 };

	float frameTime = 0.0f;
};