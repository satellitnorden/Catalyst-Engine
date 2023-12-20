#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Entities.
#include <Entities/Creation/EntityInitializationData.h>

//World.
#include <World/Core/WorldPosition.h>

class GrassInitializationData final : public EntityInitializationData
{

public:

	//The world positions.
	DynamicArray<WorldPosition> _WorldPositions;

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