#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Components.
#include <Components/Core/Component.h>

//Rendering.
#include <Rendering/Native/GrassCore.h>

//Resources.
#include <Resources/Core/ResourcePointer.h>
#include <Resources/Core/MaterialResource.h>

//World.
#include <World/Core/WorldPosition.h>
#include <World/Core/WorldSpaceAxisAlignedBoundingBox3D.h>

class GrassSharedData final
{

};

class GrassInitializationData final : public ComponentInitializationData
{

public:

	//The world grid cell.
	Vector3<int32> _WorldGridCell;

	//The grass instances.
	DynamicArray<GrassInstance> _GrassInstances;

	//The material resource.
	ResourcePointer<MaterialResource> _MaterialResource;

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

	//The material resource.
	ResourcePointer<MaterialResource> _MaterialResource;

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

DECLARE_COMPONENT(GrassComponent, GrassSharedData, GrassInitializationData, GrassInstanceData);