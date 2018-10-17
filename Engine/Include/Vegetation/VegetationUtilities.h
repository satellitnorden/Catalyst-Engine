#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Systems.
#include <Systems/RenderingSystem.h>

namespace VegetationUtilities
{

	/*
	*	Generates the transformations.
	*/
	template <typename Type>
	static void GenerateTransformations(const GridPoint2 &gridPoint, const Type &properties, const float gridSize, ConstantBufferHandle *const RESTRICT buffer, uint32 *const RESTRICT numberOfTransformations) NOEXCEPT
	{
		//Construct the box.
		const Vector3 worldPosition{ GridPoint2::GridPointToWorldPosition(gridPoint, properties._CutoffDistance * gridSize) };
		const AxisAlignedBoundingBox box{ worldPosition - properties._CutoffDistance * (gridSize * 0.5f), worldPosition + properties._CutoffDistance * (gridSize * 0.5f) };

		DynamicArray<Matrix4> transformations;

		for (uint32 i = 0; i < properties._Density; ++i)
		{
			Matrix4 newTransformation;

			if (properties._PlacementFunction(box, &newTransformation))
			{
				transformations.EmplaceSlow(newTransformation);
			}
		}

		if (!transformations.Empty())
		{
			RenderingUtilities::CreateTransformationsBuffer(transformations, buffer);
			*numberOfTransformations = static_cast<uint32>(transformations.Size());
		}

		else
		{
			*numberOfTransformations = 0;
		}
	}

	/*
	*	Invalidates a a patch at the given index for the given vegetation type information.
	*/
	template <typename Type>
	static void InvalidatePatch(Type *const RESTRICT information, const uint8 index) NOEXCEPT
	{
		//Invalidate the patch.
		information->_PatchInformations[index]._Valid = false;
		information->_PatchRenderInformations[index]._Draw = false;

		if (information->_PatchRenderInformations[index]._NumberOfTransformations > 0)
		{
			RenderingSystem::Instance->DestroyConstantBuffer(information->_PatchRenderInformations[index]._TransformationsBuffer);
		}
	}
}