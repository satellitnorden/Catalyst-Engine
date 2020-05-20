#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Resources.
#include <Resources/Core/ModelResource.h>
#include <Resources/Core/ResourcePointer.h>

//World.
#include <World/Core/WorldTransform.h>

class DynamicModelComponent final
{

public:

	//Enumeration covering all update flags.
	enum class UpdateFlag : uint8
	{
		WORLD_SPACE_AXIS_ALIGNED_BOUNDING_BOX = BIT(0)
	};

	/*
	*	Default constructor.
	*/
	FORCE_INLINE DynamicModelComponent() NOEXCEPT
	{

	}

	//The model resource.
	ResourcePointer<ModelResource> _ModelResource;

	//The previous world transform.
	WorldTransform _PreviousWorldTransform;

	//The current world transform.
	WorldTransform _CurrentWorldTransform;

	//The world space axis aligned bounding box.
	AxisAlignedBoundingBox3 _WorldSpaceAxisAlignedBoundingBox;

	//The material indices.
	StaticArray<uint32, RenderingConstants::MAXIMUM_NUMBER_OF_MESHES_PER_MODEL> _MaterialIndices;

	//The level of detail indices.
	StaticArray<uint32, RenderingConstants::MAXIMUM_NUMBER_OF_MESHES_PER_MODEL> _LevelOfDetailIndices;

	//Denotes the update flags.
	UpdateFlag _UpdateFlags{ static_cast<UpdateFlag>(0) };

};

//Define enumeration bit operations.
ENUMERATION_BIT_OPERATIONS(DynamicModelComponent::UpdateFlag);