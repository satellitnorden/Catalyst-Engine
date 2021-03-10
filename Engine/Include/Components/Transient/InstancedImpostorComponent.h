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

	//The cell.
	Vector3<int32> _Cell;

	//The transformations buffer.
	BufferHandle _TransformationsBuffer;

	//The number of transformations.
	uint32 _NumberOfTransformations;

};