#include "CommonBuffers.hlsl"

struct ComputeShaderInput
{
	uint3 groupID           : SV_GroupID;           // 3D index of the thread group in the dispatch.
	uint3 groupThreadID     : SV_GroupThreadID;     // 3D index of local thread ID in a thread group.
	uint3 dispatchThreadID  : SV_DispatchThreadID;  // 3D index of global thread ID in the dispatch.
	uint  groupIndex        : SV_GroupIndex;        // Flattened local index of the thread within a thread group.
};

// Global variables
cbuffer DispatchParams : register(b5) 
{
    uint4 numThreadGroups;
    uint4 numThreads;
}
// Parameters required to convert screen space coordinates to view space params.
cbuffer ScreenToViewParams : register(b6)
{
    float4x4 InverseProjection;
    float2 ScreenDimensions;
	
}
// Convert clip space coordinates to view space
float4 ClipToView(float4 clip)
{
	// View space position.
    float4 view = mul(InverseProjection, clip);
	// Perspecitive projection.
    view = view / view.w;

    return view;
}

// Convert screen space coordinates to view space.
float4 ScreenToView(float4 screen)
{
	// Convert to normalized texture coordinates
    float2 texCoord = screen.xy / ScreenDimensions;

	// Convert to clip space
    float4 clip = float4(float2(texCoord.x, 1.0f - texCoord.y) * 2.0f - 1.0f, screen.z, screen.w);

    return ClipToView(clip);
}

// The depth from the screen space texture.
Texture2D depthTexture : register(t1);
// Precomputed frustums for the grid.
StructuredBuffer<Frustum> in_Frustums : register(t9);

// Debug texture for debugging purposes.
//Texture2D LightCountHeatMap : register(t10);
//RWTexture2D<float4> DebugTexture : register(u0);

// Global counter for current index into the light index list.
// "o_" prefix indicates light lists for opaque geometry while 
// "t_" prefix indicates light lists for transparent geometry.
RWStructuredBuffer<uint> o_LightIndexCounter : register(u1);
RWStructuredBuffer<uint> t_LightIndexCounter : register(u2);

// Light index lists and light grids.
RWStructuredBuffer<uint> o_LightIndexList : register(u3);
RWStructuredBuffer<uint> t_LightIndexList : register(u4);
RWTexture2D<uint2> o_LightGrid : register(u5);
RWTexture2D<uint2> t_LightGrid : register(u6);

//group shared variables
groupshared uint uMinDepth;
groupshared uint uMaxDepth;
groupshared Frustum GroupFrustum;

// Opaque geometry light lists.
groupshared uint o_LightCount;
groupshared uint o_LightIndexStartOffset;
groupshared uint o_LightList[1024];

// Transparent geometry light lists.
groupshared uint t_LightCount;
groupshared uint t_LightIndexStartOffset;
groupshared uint t_LightList[1024];

// Add the light to the visible light list for opaque geometry.
void o_AppendLight(uint lightIndex)
{
	uint index; // Index into the visible lights array.
	InterlockedAdd(o_LightCount, 1, index);
    if (index < 1024)
	{
		o_LightList[index] = lightIndex;
	}
}

// Add the light to the visible light list for transparent geometry.
void t_AppendLight(uint lightIndex)
{
	uint index; // Index into the visible lights array.
	InterlockedAdd(t_LightCount, 1, index);
    if (index < 1024)
	{
		t_LightList[index] = lightIndex;
	}
}


// Implementation of light culling compute shader is based on the presentation
// "DirectX 11 Rendering in Battlefield 3" (2011) by Johan Andersson, DICE.
// Retrieved from: http://www.slideshare.net/DICEStudio/directx-11-rendering-in-battlefield-3
// Retrieved: July 13, 2015
// And "Forward+: A Step Toward Film-Style Shading in Real Time", Takahiro Harada (2012)
// published in "GPU Pro 4", Chapter 5 (2013) Taylor & Francis Group, LLC.

[numthreads(BLOCK_SIZE, BLOCK_SIZE, 1)]
void main(ComputeShaderInput IN) // light culling everyframe 
{
	// Calculate min & max depth in threadgroup / tile.
	int2 texCoord = IN.dispatchThreadID.xy;
	float fDepth = depthTexture.Load(int3(texCoord, 0)).r;
	//fDepth = (0.5 * projection[3][2]) / (fDepth + 0.5 * projection[2][2] - 0.5);
    const float near = 0.01f; // NEAR PLANE
    const float far = 500.0f; // FAR PLANE

    fDepth = ((2.0f * near) / (far + near - fDepth * (far - near)));
	
	uint uDepth = asuint(fDepth);

	if (IN.groupIndex == 0) // Avoid contention by other threads in the group.
	{
		uMinDepth = 0xffffffff;
		uMaxDepth = 0;
		o_LightCount = 0;
		t_LightCount = 0;
		GroupFrustum = in_Frustums[IN.groupID.x + (IN.groupID.y * numThreadGroups.x)];
	}

	GroupMemoryBarrierWithGroupSync();

	InterlockedMin(uMinDepth, uDepth);
	InterlockedMax(uMaxDepth, uDepth);

	GroupMemoryBarrierWithGroupSync();

	float fMinDepth = asfloat(uMinDepth);
	float fMaxDepth = asfloat(uMaxDepth);

	// Convert depth values to view space.
	float minDepthVS = ClipToView(float4(0, 0, fMinDepth, 1)).z;
	float maxDepthVS = ClipToView(float4(0, 0, fMaxDepth, 1)).z;
	float nearClipVS = ClipToView(float4(0, 0, 0, 1)).z;

	// Clipping plane for minimum depth value 
	// (used for testing lights within the bounds of opaque geometry).
	Plane minPlane = { float3(0, 0, 1), minDepthVS }; //made z  positive
	
	// Cull lights
	// Each thread in a group will cull 1 light until all lights have been culled.

	for (uint i = IN.groupIndex; i < LIGHT_COUNT; i += BLOCK_SIZE * BLOCK_SIZE)//LIGHT_COUNT
	{

		if (Lights[i].enabled)
		{
			Light light = Lights[i];

			switch (light.type)
			{
			case POINT_LIGHT:
			{

				float3 lightPositionVS = mul(light.lightPosition, view).xyz;
				Sphere sphere = { lightPositionVS, light.range };
				if (SphereInsideFrustum(sphere, GroupFrustum, nearClipVS, maxDepthVS))
				{
					// Add light to light list for transparent geometry.
					t_AppendLight(i);

					if (!SphereInsidePlane(sphere, minPlane))
					{
						// Add light to light list for opaque geometry.
						o_AppendLight(i);
					}
				}
			}
			break;
			case SPOT_LIGHT:
			{
				float coneRadius = tan(radians(light.spotlightAngle)) * light.range;
                float3 lightPositionVS = mul(light.lightPosition, view).xyz;
				Cone cone = { lightPositionVS, light.range, light.lightDirection, coneRadius };
				if (ConeInsideFrustum(cone, GroupFrustum, nearClipVS, maxDepthVS))
				{
					// Add light to light list for transparent geometry.
					t_AppendLight(i);

					if (!ConeInsidePlane(cone, minPlane))
					{
						// Add light to light list for opaque geometry.
						o_AppendLight(i);
					}
				}
			}
			break;
			case DIRECTIONAL_LIGHT:
			{
				// Directional lights always get added to our light list.
				// (Hopefully there are not too many directional lights!)
				t_AppendLight(i);
				o_AppendLight(i);
			}
			break;
			}
		}
	}

	// Wait till all threads in group have caught up.
	GroupMemoryBarrierWithGroupSync();

	// Update global memory with visible light buffer.
	// First update the light grid (only thread 0 in group needs to do this)
	if (IN.groupIndex == 0)
	{
		// Update light grid for opaque geometry.
		InterlockedAdd(o_LightIndexCounter[0], o_LightCount, o_LightIndexStartOffset);
		o_LightGrid[IN.groupID.xy] = uint2(o_LightIndexStartOffset, o_LightCount);

		// Update light grid for transparent geometry.
		InterlockedAdd(t_LightIndexCounter[0], t_LightCount, t_LightIndexStartOffset);
		t_LightGrid[IN.groupID.xy] = uint2(t_LightIndexStartOffset, t_LightCount);
	}

	GroupMemoryBarrierWithGroupSync();

	// Now update the light index list (all threads).
	// For opaque geometry.
	for (i = IN.groupIndex; i < o_LightCount; i += BLOCK_SIZE * BLOCK_SIZE)
	{
		o_LightIndexList[o_LightIndexStartOffset + i] = o_LightList[i];
	}
	// For transparent geometry.
	for (i = IN.groupIndex; i < t_LightCount; i += BLOCK_SIZE * BLOCK_SIZE)
	{
		t_LightIndexList[t_LightIndexStartOffset + i] = t_LightList[i];
	}

	
}


// View space frustums for the grid cells.
RWStructuredBuffer<Frustum> out_Frustums : register(u0); //used in compute frustrum

// A kernel to compute frustums for the grid
// This kernel is executed once per grid cell. Each thread
// computes a frustum for a grid cell.
[numthreads(BLOCK_SIZE, BLOCK_SIZE, 1)]
void ComputeFrustums(ComputeShaderInput IN) // compute ONCE or when resize window
{
	// View space eye position is always at the origin.
    const float3 eyePos = float3(0, 0, 0);

	// Compute the 4 corner points on the far clipping plane to use as the 
	// frustum vertices.
    float4 screenSpace[4];
	// Top left point
    screenSpace[0] = float4(IN.dispatchThreadID.xy * BLOCK_SIZE, 1.0f, 1.0f); //changed z from negative to positive in z component, all of these 4.
	// Top right point
    screenSpace[1] = float4(float2(IN.dispatchThreadID.x + 1, IN.dispatchThreadID.y) * BLOCK_SIZE, 1.0f, 1.0f);
	// Bottom left point
    screenSpace[2] = float4(float2(IN.dispatchThreadID.x, IN.dispatchThreadID.y + 1) * BLOCK_SIZE, 1.0f, 1.0f);
	// Bottom right point
    screenSpace[3] = float4(float2(IN.dispatchThreadID.x + 1, IN.dispatchThreadID.y + 1) * BLOCK_SIZE, 1.0f, 1.0f);

    float3 viewSpace[4];
	// Now convert the screen space points to view space
    for (int i = 0; i < 4; i++)
    {
        viewSpace[i] = ScreenToView(screenSpace[i]).xyz;
    }

	// Now build the frustum planes from the view space points
    Frustum frustum;

	// Left plane
    frustum.planes[0] = ComputePlane(eyePos, viewSpace[2], viewSpace[0]);
	// Right plane
    frustum.planes[1] = ComputePlane(eyePos, viewSpace[1], viewSpace[3]);
	// Top plane
    frustum.planes[2] = ComputePlane(eyePos, viewSpace[0], viewSpace[1]);
	// Bottom plane
    frustum.planes[3] = ComputePlane(eyePos, viewSpace[3], viewSpace[2]);

	// Store the computed frustum in global memory (if our thread ID is in bounds of the grid).
    if (IN.dispatchThreadID.x < numThreads.x && IN.dispatchThreadID.y < numThreads.y)
    {
        uint index = IN.dispatchThreadID.x + (IN.dispatchThreadID.y * numThreads.x);
        out_Frustums[index] = frustum;
    }
  
}



