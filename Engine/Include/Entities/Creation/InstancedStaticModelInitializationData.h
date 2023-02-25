#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/Optional.h>

//Entities.
#include <Entities/Creation/EntityInitializationData.h>

//Resources.
#include <Resources/Core/MaterialResource.h>
#include <Resources/Core/ModelResource.h>
#include <Resources/Core/ResourcePointer.h>

//Rendering.
#include <Rendering/Native/ModelFadeData.h>

//World.
#include <World/Core/WorldSpaceAxisAlignedBoundingBox3D.h>
#include <World/Core/WorldTransform.h>

class InstancedStaticModelInitializationData final : public EntityInitializationData
{

public:

	/*
	*	Preprocessed data class definition.
	*/
	class PreprocessedData final
	{

	public:

		//The world space axis aligned bounding box.
		WorldSpaceAxisAlignedBoundingBox3D _WorldSpaceAxisAlignedBoundingBox;
		
		//The cell.
		Vector3<int32> _Cell;

		//The transformations buffer.
		BufferHandle _TransformationsBuffer;

		//The number of transformations.
		uint32 _NumberOfTransformations;

	};

	//The world transforms.
	DynamicArray<WorldTransform> _WorldTransforms;

	//The model resource.
	ResourcePointer<ModelResource> _ModelResource;

	//The material resources.
	StaticArray<ResourcePointer<MaterialResource>, RenderingConstants::MAXIMUM_NUMBER_OF_MESHES_PER_MODEL> _MaterialResources;

	//The model flags.
	ModelFlags _ModelFlags;

	//The model fade data.
	Optional<ModelFadeData> _ModelFadeData;

	//The preprocessed data.
	PreprocessedData _PreprocessedData;

};