#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Components.
#include <Components/Core/Component.h>

//Resources.
#include <Resources/Core/ResourcePointer.h>
#include <Resources/Core/Texture2DResource.h>

//Terrain.
#include <Terrain/TerrainQuadTree.h>

//World.
#include <World/Core/WorldSpaceAxisAlignedBoundingBox3D.h>

class WaterSharedData final
{

};

class WaterInitializationData final : public ComponentInitializationData
{

public:

	/*
	*	Preprocessed data class definition.
	*/
	class PreprocessedData final
	{

	public:

		//The world space axis aligned bounding box.
		WorldSpaceAxisAlignedBoundingBox3D _WorldSpaceAxisAlignedBoundingBox;

		//The buffer.
		BufferHandle _Buffer;

		//The index offset.
		uint32 _IndexOffset;

		//The index count.
		uint32 _IndexCount;

	};

	//The world position.
	WorldPosition _WorldPosition;

	//The patch size.
	uint32 _PatchSize;

	//The base resolution.
	uint32 _BaseResolution;

	//The maximum subdivision steps.
	uint8 _MaximumSubdivisionSteps;

	//The texture.
	ResourcePointer<Texture2DResource> _Texture;

	//The preprocessed data.
	PreprocessedData _PreprocessedData;

};

class WaterInstanceData final
{

public:

	//The world position.
	WorldPosition _WorldPosition;

	//The world space axis aligned bounding box.
	WorldSpaceAxisAlignedBoundingBox3D _WorldSpaceAxisAlignedBoundingBox;

	//The patch size.
	uint32 _PatchSize;

	//The base resolution.
	uint32 _BaseResolution;

	//The maximum subdivision steps.
	uint8 _MaximumSubdivisionSteps;

	//The buffer.
	BufferHandle _Buffer;

	//The index offset.
	uint32 _IndexOffset;

	//The index count.
	uint32 _IndexCount;

	//The texture.
	ResourcePointer<Texture2DResource> _Texture;

	//Denotes whether or not this terrain is visible.
	bool _Visibility;

	//The quad tree.
	TerrainQuadTree _QuadTree;

};

DECLARE_COMPONENT(WaterComponent, WaterSharedData, WaterInitializationData, WaterInstanceData);