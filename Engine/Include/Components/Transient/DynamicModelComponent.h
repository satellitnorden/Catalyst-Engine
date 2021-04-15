#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Physics.
#include <Physics/Native/ModelCollisionConfiguration.h>
#include <Physics/Native/ModelSimulationConfiguration.h>

//Resources.
#include <Resources/Core/MaterialResource.h>
#include <Resources/Core/ModelResource.h>
#include <Resources/Core/ResourcePointer.h>

//World.
#include <World/Core/WorldTransform.h>

class DynamicModelComponent final
{

public:

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
	AxisAlignedBoundingBox3D _WorldSpaceAxisAlignedBoundingBox;

	//The material resources.
	StaticArray<ResourcePointer<MaterialResource>, RenderingConstants::MAXIMUM_NUMBER_OF_MESHES_PER_MODEL> _MaterialResources;

	//The model collision configuration.
	ModelCollisionConfiguration _ModelCollisionConfiguration;

	//The model simulation configuration.
	ModelSimulationConfiguration _ModelSimulationConfiguration;

	//The level of detail indices.
	StaticArray<uint32, RenderingConstants::MAXIMUM_NUMBER_OF_MESHES_PER_MODEL> _LevelOfDetailIndices;

	//Denotes whether or not this dynamic model is visible.
	bool _Visibility;

};