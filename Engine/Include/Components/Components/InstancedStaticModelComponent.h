#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/Optional.h>

//Components.
#include <Components/Core/Component.h>

//Content.
#include <Content/Core/AssetPointer.h>
#include <Content/Assets/ModelAsset.h>

//Rendering.
#include <Rendering/Native/RenderingCore.h>
#include <Rendering/Native/ModelFadeData.h>

//Resources.
#include <Resources/Core/ResourcePointer.h>
#include <Resources/Core/MaterialResource.h>

//World.
#include <World/Core/WorldSpaceAxisAlignedBoundingBox3D.h>
#include <World/Core/WorldTransform.h>

class InstancedStaticModelInitializationData final : public ComponentInitializationData
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

	//The model.
	AssetPointer<ModelAsset> _Model;

	//The material resources.
	StaticArray<ResourcePointer<MaterialResource>, RenderingConstants::MAXIMUM_NUMBER_OF_MESHES_PER_MODEL> _MaterialResources;

	//The model flags.
	ModelFlags _ModelFlags;

	//The model fade data.
	Optional<ModelFadeData> _ModelFadeData;

	//The preprocessed data.
	PreprocessedData _PreprocessedData;

};

class InstancedStaticModelInstanceData final
{

public:

	//The world space axis aligned bounding box.
	WorldSpaceAxisAlignedBoundingBox3D _WorldSpaceAxisAlignedBoundingBox;

	//The model.
	AssetPointer<ModelAsset> _Model;

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

	//Denotes whether or not this instanced static model is visible.
	bool _Visibility;

};

DECLARE_COMPONENT(InstancedStaticModelComponent, InstancedStaticModelInitializationData, InstancedStaticModelInstanceData);