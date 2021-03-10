#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Entities.
#include <Entities/Creation/EntityInitializationData.h>

//Resources.
#include <Resources/Core/MaterialResource.h>
#include <Resources/Core/ResourcePointer.h>

//World.
#include <World/Core/WorldPosition.h>

class InstancedImpostorInitializationData final : public EntityInitializationData
{

public:

	//The material resource.
	ResourcePointer<MaterialResource> _MaterialResource;

	//The dimensions.
	Vector2<float32> _Dimensions;

	//The world positions.
	DynamicArray<WorldPosition> _WorldPositions;

};