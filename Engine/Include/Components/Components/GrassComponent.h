#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Components.
#include <Components/Core/Component.h>

//Content.
#include <Content/Core/AssetPointer.h>
#include <Content/Assets/MaterialAsset.h>

//Rendering.
#include <Rendering/Native/GrassCore.h>

//World.
#include <World/Core/WorldPosition.h>
#include <World/Core/WorldSpaceAxisAlignedBoundingBox3D.h>

class GrassInitializationData final : public ComponentInitializationData
{

public:

	//The world grid cell.
	Vector3<int32> _WorldGridCell;

	//The grass instances.
	DynamicArray<GrassInstance> _GrassInstances;

	//The material.
	AssetPointer<MaterialAsset> _Material;

	//The thickness.
	float32 _Thickness;

	//The height.
	float32 _Height;

	//The tilt.
	float32 _Tilt;

	//The bend.
	float32 _Bend;

	//The fade out distance.
	float32 _FadeOutDistance;

};

class GrassInstanceData final
{

public:

	//The world grid cell.
	Vector3<int32> _WorldGridCell;

	//The world space axis aligned bounding box.
	WorldSpaceAxisAlignedBoundingBox3D _WorldSpaceAxisAlignedBoundingBox;

	//The instance buffer.
	BufferHandle _InstanceBuffer;

	//The number of instances.
	uint32 _NumberOfInstances;

	//The material.
	AssetPointer<MaterialAsset> _Material;

	//The thickness.
	float32 _Thickness;

	//The height.
	float32 _Height;

	//The tilt.
	float32 _Tilt;

	//The bend.
	float32 _Bend;

	//The fade out distance.
	float32 _FadeOutDistance;

	//The visibility flags.
	VisibilityFlags _VisibilityFlags;

	//The level of detail.
	uint8 _LevelOfDetail;

};

DECLARE_COMPONENT
(
	GrassComponent,
	GrassInitializationData,
	GrassInstanceData,
	COMPONENT_PARALLEL_BATCH_UPDATE(UpdatePhase::PRE_RENDER, 128)
);