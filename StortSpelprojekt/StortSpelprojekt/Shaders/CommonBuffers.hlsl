//static const int POINT_LIGHT_COUNT = 16;
static const int LIGHT_COUNT = 1;

#ifndef BLOCK_SIZE
#pragma message( "BLOCK_SIZE undefined. Default to 32.")
#define BLOCK_SIZE 32 // should be defined by the application.
#endif

#define POINT_LIGHT 0
#define SPOT_LIGHT 1
#define DIRECTIONAL_LIGHT 2
//struct PointLight
//{
//    float4 lightColor;
//    float3 lightPosition;
//	float range;
//    float3 attenuation;
//	float paddingThree;
//};

struct Light //slot t8
{
    float4 lightColor;
    float4 lightPosition;
    float4 lightPositionVS;
    float3 lightDirection;
    float range;
    float3 attenuation;
    float spotlightAngle;
    uint enabled;
    uint type;
    float intensity;
    float padding;
};
StructuredBuffer<Light> Lights : register(t8); 
StructuredBuffer<uint> LightIndexList : register(t10);
Texture2D<uint2> LightGrid : register(t11);
cbuffer cb_Object : register(b0)
{
    row_major matrix mvp;
    row_major matrix world;
    row_major matrix vp;
    row_major matrix wv;
};

cbuffer cb_Material : register(b1)
{
    float4 matAmbient;
    float4 matDiffuse;
    float4 matSpecular;
	
    bool hasAlbedo;
    bool hasNormalMap;
    float2 pad_cbm;
}

cbuffer cb_Scene : register(b2)
{
    row_major matrix invProjection;
    row_major matrix invView;

    float3 cameraPosition;
    int id;
    float factor;
    float time;
    float2 mousePos;
    float2 screenSize;
    float2 pad_cbs;
    row_major matrix view;
}

//cbuffer cb_Lights : register(b3)
//{
//	PointLight pointLights[POINT_LIGHT_COUNT];
//	int nrOfPointLights;
//	float3 sunDirection;
//	float sunIntensity;
//	float3 pad3;
//};

//cbuffer cb_Lights : register(b3)
//{
//    Light lights[LIGHT_COUNT];
//    int nrOfLights;
//    float3 sunDirection;
//    float sunIntensity;
//    float3 pad_cbl;
//};

StructuredBuffer<float4x4> bones : register(t2);

struct Grass
{
    float4 position;
    float4 normal;
    float4 uv;
};

StructuredBuffer<Grass> grassStraws : register(t3);

StructuredBuffer<float4> grassIndices : register(t4);

StructuredBuffer<float4> BCCoordinates : register(t5);

cbuffer cb_grass : register(b4)
{
    float pixelSize;
    float grassWidth;
    float grassRadius;
    float grassDisplacement;
};

struct Plane
{
    float3 N; // Plane normal.
    float d; // Distance to origin.
};

struct Frustum
{
    Plane planes[4]; // left, right, top, bottom frustum planes.
};

Plane ComputePlane(float3 p0, float3 p1, float3 p2)
{
	//This equation assumes a right - handed
    Plane plane;
    float3 v0 = p1 - p0;
    float3 v2 = p2 - p0;
    plane.N = normalize(cross(v2, v0)); //switched these for left

	//Compute the distance to the origin using p0.
    plane.d = dot(plane.N, p0);
    return plane;
}

struct Sphere
{
    float3 c; // Center point.
    float r; // Radius.
};

struct Cone
{
    float3 T; // Cone tip.
    float h; // Height of the cone.
    float3 d; // Direction of the cone.
    float r; // bottom radius of the cone.
};
// Check to see if a point is fully behind (inside the negative halfspace of) a plane.
bool PointInsidePlane(float3 p, Plane plane)
{
    return dot(plane.N, p) - plane.d < 0;
}
// Check to see if a sphere is fully behind (inside the negative halfspace of) a plane.
// Source: Real-time collision detection, Christer Ericson (2005)
bool SphereInsidePlane(Sphere sphere, Plane plane)
{
    return dot(plane.N, sphere.c) - plane.d < -sphere.r;
}

// Check to see if a cone if fully behind (inside the negative halfspace of) a plane.
// Source: Real-time collision detection, Christer Ericson (2005)
bool ConeInsidePlane(Cone cone, Plane plane)
{
	// Compute the farthest point on the end of the cone to the positive space of the plane.
    float3 m = cross(cross(plane.N, cone.d), cone.d);
    float3 Q = cone.T + cone.d * cone.h - m * cone.r;

	// The cone is in the negative halfspace of the plane if both
	// the tip of the cone and the farthest point on the end of the cone to the 
	// positive halfspace of the plane are both inside the negative halfspace 
	// of the plane.
    return PointInsidePlane(cone.T, plane) && PointInsidePlane(Q, plane);
}
// Check to see of a light is partially contained within the frustum.
bool SphereInsideFrustum(Sphere sphere, Frustum frustum, float zNear, float zFar)
{
    bool result = true;

	// First check depth
	// Note: Here, the view vector points in the -Z axis so the 
	// far depth value will be approaching -infinity.
    if (sphere.c.z - sphere.r > zFar || sphere.c.z + sphere.r < zNear ) //Switched places for zNear and zFar
    {
        result = false;
    }

	// Then check frustum planes
    for (int i = 0; i < 4 && result; i++)
    {
        if (SphereInsidePlane(sphere, frustum.planes[i]))
        {
            result = false;
        }
    }

    return result;
}
bool ConeInsideFrustum(Cone cone, Frustum frustum, float zNear, float zFar)
{
    bool result = true;

    Plane nearPlane = { float3(0, 0, -1), -zNear };
    Plane farPlane = { float3(0, 0, 1), zFar };

	// First check the near and far clipping planes.
    if (ConeInsidePlane(cone, nearPlane) || ConeInsidePlane(cone, farPlane))
    {
        result = false;
    }

	// Then check frustum planes
    for (int i = 0; i < 4 && result; i++)
    {
        if (ConeInsidePlane(cone, frustum.planes[i]))
        {
            result = false;
        }
    }

    return result;
}