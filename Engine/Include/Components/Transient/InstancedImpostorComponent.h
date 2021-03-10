#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/Optional.h>

//Math.
#include <Math/General/Vector.h>

//Resources.
#include <Resources/Core/MaterialResource.h>
#include <Resources/Core/ResourcePointer.h>

class InstancedImpostorComponent final
{

public:

	//The material resource.
	ResourcePointer<MaterialResource> _MaterialResource;

	//The dimensions.
	Vector2<float32> _Dimensions;

	//The cell.
	Vector3<int32> _Cell;

	//The start fade in distance.
	float32 _StartFadeInDistance;

	//The end fade in distance.
	float32 _EndFadeInDistance;

	//The start fade out distance.
	float32 _StartFadeOutDistance;

	//The end fade out distance.
	float32 _EndFadeOutDistance;

	//The transformations buffer.
	BufferHandle _TransformationsBuffer;

	//The number of transformations.
	uint32 _NumberOfTransformations;

};