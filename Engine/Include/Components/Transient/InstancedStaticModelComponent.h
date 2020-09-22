#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Resources.
#include <Resources/Core/MaterialResource.h>
#include <Resources/Core/ModelResource.h>
#include <Resources/Core/ResourcePointer.h>

//Rendering.
#include <Rendering/Native/RenderingCore.h>
#include <Rendering/Native/ModelFadeData.h>

class InstancedStaticModelComponent final
{

public:

	//The model resource.
	ResourcePointer<ModelResource> _ModelResource;

	//The material resources.
	StaticArray<ResourcePointer<MaterialResource>, RenderingConstants::MAXIMUM_NUMBER_OF_MESHES_PER_MODEL> _MaterialResources;

	//The cell.
	Vector3<int32> _Cell;

	//The transformations buffer.
	BufferHandle _TransformationsBuffer;

	//The number of transformations.
	uint32 _NumberOfTransformations;

	//The model flags.
	ModelFlags _ModelFlags;

	//The model fade data.
	Optional<ModelFadeData> _ModelFadeData;

};