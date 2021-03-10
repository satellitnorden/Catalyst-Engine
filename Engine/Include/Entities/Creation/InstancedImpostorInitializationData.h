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

	//The start fade in distance.
	float32 _StartFadeInDistance;

	//The end fade in distance.
	float32 _EndFadeInDistance;

	//The start fade out distance.
	float32 _StartFadeOutDistance;

	//The end fade out distance.
	float32 _EndFadeOutDistance;

	//The world positions.
	DynamicArray<WorldPosition> _WorldPositions;

};