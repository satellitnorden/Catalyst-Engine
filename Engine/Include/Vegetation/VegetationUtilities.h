#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Rendering.
#include <Rendering/Engine/Viewer.h>

//Systems.
#include <Systems/RenderingSystem.h>

namespace VegetationUtilities
{

	/*
	*	Generates the transformations.
	*/
	template <typename Type>
	static void GenerateTransformations(const GridPoint2 &gridPoint, const Type &properties, DynamicArray<Matrix4> *const RESTRICT transformations) NOEXCEPT
	{
		ASSERT(transformations->Empty(), "Transformations array must be empty!");

		//Construct the box.
		const Vector3<float> worldPosition{ GridPoint2::GridPointToWorldPosition(gridPoint, properties._CutoffDistance) };
		const AxisAlignedBoundingBox box{ worldPosition - properties._CutoffDistance * 0.5f, worldPosition + properties._CutoffDistance * 0.5f };

		//Calculate the number of placements.
		uint32 placements{ static_cast<uint32>(properties._CutoffDistance * properties._CutoffDistance * properties._Density) };

		for (uint32 i = 0; i < placements; ++i)
		{
			Matrix4 newTransformation;

			if (properties._PlacementFunction(box, &newTransformation))
			{
				transformations->EmplaceSlow(newTransformation);
			}
		}
	}

	/*
	*	Sorts a set of transformations into different level of details.
	*/
	void SortTransformations(	const DynamicArray<Matrix4> &transformations,
								const GrassVegetationTypeProperties &properties,
								StaticArray<DynamicArray<Matrix4>, UNDERLYING(GrassVegetationLevelOfDetail::NumberOfGrassVegetationLevelOfDetails)> *const RESTRICT levelOfDetailTransformations,
								StaticArray<ConstantBufferHandle, UNDERLYING(GrassVegetationLevelOfDetail::NumberOfGrassVegetationLevelOfDetails)> *const RESTRICT buffers,
								StaticArray<uint32, UNDERLYING(GrassVegetationLevelOfDetail::NumberOfGrassVegetationLevelOfDetails)> *const RESTRICT numberOfTransformations) NOEXCEPT
	{
		//Cache the viewer position.
		const Vector3<float> viewerPosition{ Viewer::Instance->GetPosition() };

		for (const Matrix4 &transformation : transformations)
		{
			//Calculate the distance to the viewer.
			const float distanceToViewer{ Vector3<float>::Length(viewerPosition - transformation.GetTranslation()) };

			if (distanceToViewer > properties._LowDetailDistance)
			{
				levelOfDetailTransformations->At(UNDERLYING(GrassVegetationLevelOfDetail::Low)).EmplaceSlow(transformation);
			}

			else
			{
				levelOfDetailTransformations->At(UNDERLYING(GrassVegetationLevelOfDetail::High)).EmplaceSlow(transformation);
			}
		}

		for (uint8 i{ 0 }; i < UNDERLYING(GrassVegetationLevelOfDetail::NumberOfGrassVegetationLevelOfDetails); ++i)
		{
			if (!levelOfDetailTransformations->At(i).Empty())
			{
				RenderingUtilities::CreateTransformationsBuffer(levelOfDetailTransformations->At(i), &buffers->At(i));
				numberOfTransformations->At(i) = static_cast<uint32>(levelOfDetailTransformations->At(i).Size());
			}

			else
			{
				numberOfTransformations->At(i) = 0;
			}
		}
	}

	/*
	*	Invalidates a patch at the given index for the given vegetation type information.
	*/
	template <typename Type>
	static void InvalidatePatch(Type *const RESTRICT information, const uint8 index) NOEXCEPT
	{
		//Invalidate the patch.
		information->_PatchInformations[index]._Valid = false;
		information->_PatchRenderInformations[index]._Visibility = VisibilityFlag::None;

		if (information->_PatchRenderInformations[index]._NumberOfTransformations > 0)
		{
			RenderingSystem::Instance->DestroyConstantBuffer(information->_PatchRenderInformations[index]._TransformationsBuffer);
		}
	}

	/*
	*	Invalidates a grass vegetation patch at the given index for the given vegetation type information.
	*/
	static void InvalidateGrassVegetationPatch(GrassVegetationTypeInformation *const RESTRICT information, const uint8 index) NOEXCEPT
	{
		//Invalidate the patch.
		information->_PatchInformations[index]._Valid = false;

		for (uint8 i{ 0 }; i < UNDERLYING(GrassVegetationLevelOfDetail::NumberOfGrassVegetationLevelOfDetails); ++i)
		{
			information->_PatchRenderInformations[index]._Visibilities[i] = VisibilityFlag::None;

			if (information->_PatchRenderInformations[index]._NumberOfTransformations[i] > 0)
			{
				RenderingSystem::Instance->DestroyConstantBuffer(information->_PatchRenderInformations[index]._TransformationsBuffers[i]);

				information->_PatchRenderInformations[index]._TransformationsBuffers[i] = nullptr;
			}
		}
	}
}