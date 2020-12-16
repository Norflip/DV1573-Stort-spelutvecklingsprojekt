#pragma once
#include <DirectXMath.h>
namespace dx = DirectX;

constexpr size_t CB_OBJECT_SLOT = 0;
constexpr size_t CB_MATERIAL_SLOT = 1;
constexpr size_t CB_SCENE_SLOT = 2; 
//constexpr size_t CB_LIGHT_SLOT = 3;
constexpr size_t CB_GRASS_PARAMETERS_SLOT = 4;
//constexpr size_t CB_FOG_SLOT = 5;
constexpr size_t CB_DISPATCH_PARAMS_SLOT = 5;
constexpr size_t CB_SCREEN_TOVIEW_PARAMS_SLOT = 6;

//constexpr size_t POINT_LIGHT_COUNT = 32;
//constexpr size_t LIGHT_COUNT = 40;

ALIGN16
struct cb_Object
{
	dx::XMFLOAT4X4 mvp;
	dx::XMFLOAT4X4 world;
	dx::XMFLOAT4X4 vp;
	dx::XMFLOAT4X4 wv;
};

ALIGN16
struct cb_Material
{
	dx::XMFLOAT4 ambient;
	dx::XMFLOAT4 diffuse;
	dx::XMFLOAT4 specular;
	
	int hasAlbedo;
	int hasNormalMap;
	float paddingOne;
	float paddingTwo;
};



ALIGN16
struct s_Plane
{
	dx::XMFLOAT3 N; //plane normal
	float d; //distance to origin
};

ALIGN16
struct s_Frustum
{
	s_Plane planes[4];
};


ALIGN16
struct s_Light //slot t8
{
	dx::XMFLOAT4 lightColor;
	dx::XMFLOAT4 lightPosition;
	//dx::XMFLOAT4 lightPositionVS;
	dx::XMFLOAT3 lightDirection;
	float range;
	//dx::XMFLOAT3 attenuation;
	float spotlightAngle;
	UINT enabled;
	UINT type;
	float intensity;
	//float padding;
};

ALIGN16
struct cb_DispatchParams
{
	dx::XMUINT4 numThreadGroups;
	dx::XMUINT4 numThreads;
};

ALIGN16
struct cb_ScreenToViewParams
{
	dx::XMFLOAT4X4 inverseProjection;
	dx::XMFLOAT2 screenDimensions;
	
};

ALIGN16
struct cb_Scene
{
	dx::XMFLOAT4X4 invProjection;

	dx::XMFLOAT4X4 invView;

	dx::XMFLOAT3 cameraPosition;
	int id;

	float factor;
	float time;
	dx::XMFLOAT2 mousePos;

	dx::XMFLOAT2 screenSize;
	float distanceToHouse;
	float pad_cbs;

	dx::XMFLOAT4X4 view;
	
	


};

ALIGN16
struct cb_grass
{
	float pixelSize;
	float grassWidth;
	float grassRadius;
	float grassDisplacement;
};
