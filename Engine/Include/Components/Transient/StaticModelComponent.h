#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Resources.
#include <Resources/Core/ModelResource.h>
#include <Resources/Core/ResourcePointer.h>

//World.
#include <World/Core/WorldSpaceAxisAlignedBoundingBox3D.h>

class StaticModelComponent final
{

public:

	//The model resource.
	ResourcePointer<ModelResource> _ModelResource;

	//The world transform.
	WorldTransform _WorldTransform;

	//The world space axis aligned bounding box.
	WorldSpaceAxisAlignedBoundingBox3D _WorldSpaceAxisAlignedBoundingBox;

	//The material resources.
	StaticArray<ResourcePointer<MaterialResource>, RenderingConstants::MAXIMUM_NUMBER_OF_MESHES_PER_MODEL> _MaterialResources;

	//The level of detail indices.
	StaticArray<uint8, RenderingConstants::MAXIMUM_NUMBER_OF_MESHES_PER_MODEL> _LevelOfDetailIndices;

	//Bitmask for the visible meshes.
	uint8 _MeshesVisibleMask;

	//Denotes whether or not this static model is visible.
	bool _Visibility;

};