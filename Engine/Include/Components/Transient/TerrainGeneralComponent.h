#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/Texture2D.h>

//World.
#include <World/Core/WorldPosition.h>
#include <World/Core/WorldSpaceAxisAlignedBoundingBox3D.h>

class TerrainGeneralComponent final
{

public:

	//The world position.
	WorldPosition _WorldPosition;

	//The world space axis aligned bounding box.
	WorldSpaceAxisAlignedBoundingBox3D _WorldSpaceAxisAlignedBoundingBox;

	//The patch size.
	uint32 _PatchSize;

	//The height map.
	Texture2D<float32> _HeightMap;

	//The normal map.
	Texture2D<Vector3<float32>> _NormalMap;

	//The index map.
	Texture2D<Vector4<uint8>> _IndexMap;

	//The blend map.
	Texture2D<Vector4<float32>> _BlendMap;

	//The base resolution.
	uint32 _BaseResolution;

	//The maximum subdivision steps.
	uint8 _MaximumSubdivisionSteps;

};