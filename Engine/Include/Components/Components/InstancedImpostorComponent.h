#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Components.
#include <Components/Core/Component.h>

//Content.
#include <Content/Core/AssetPointer.h>
#include <Content/Assets/MaterialAsset.h>

//World.
#include <World/Core/WorldSpaceAxisAlignedBoundingBox3D.h>
#include <World/Core/WorldPosition.h>

class InstancedImpostorInitializationData final : public ComponentInitializationData
{

public:

	//The material.
	AssetPointer<MaterialAsset> _Material;

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

class InstancedImpostorInstanceData final
{

public:

	//The material.
	AssetPointer<MaterialAsset> _Material;

	//The dimensions.
	Vector2<float32> _Dimensions;

	//The cell.
	Vector3<int32> _Cell;

	//The world space axis aligned bounding box.
	WorldSpaceAxisAlignedBoundingBox3D _WorldSpaceAxisAlignedBoundingBox;

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

	//The visibility flags.
	VisibilityFlags _VisibilityFlags;

};

DECLARE_COMPONENT(InstancedImpostorComponent, InstancedImpostorInitializationData, InstancedImpostorInstanceData);