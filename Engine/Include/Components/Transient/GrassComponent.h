#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/RenderingCore.h>

//World.
#include <World/Core/WorldSpaceAxisAlignedBoundingBox3D.h>

class GrassComponent final
{

public:

	//The cell.
	Vector3<int32> _Cell;

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