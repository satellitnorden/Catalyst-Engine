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
	*	Invalidates a patch at the given index for the given vegetation type information.
	*/
	template <typename Type>
	static void InvalidatePatch(Type *const RESTRICT information, const uint8 index) NOEXCEPT
	{
		//Invalidate the patch.
		information->_PatchInformations[index]._Valid = false;

		for (uint8 i{ 0 }; i < UNDERLYING(VegetationLevelOfDetail::NumberOfVegetationLevelOfDetails); ++i)
		{
			information->_PatchRenderInformations[index]._Visibilities[i] = VisibilityFlag::None;

			if (information->_PatchRenderInformations[index]._NumberOfTransformations[i] > 0)
			{
				RenderingSystem::Instance->DestroyConstantBuffer(information->_PatchRenderInformations[index]._TransformationsBuffers[i]);

				information->_PatchRenderInformations[index]._TransformationsBuffers[i] = nullptr;
			}
		}
	}

	/*
	*	Processes a vegetation type update.
	*/
	template <typename TYPE>
	void ProcessUpdate(TYPE *const RESTRICT update) NOEXCEPT
	{
		if (update->_LevelOfDetailUpdate)
		{
			for (uint8 i{ 0 }; i < UNDERLYING(VegetationLevelOfDetail::NumberOfVegetationLevelOfDetails); ++i)
			{
				if (update->_Information->_PatchRenderInformations[update->_Index]._NumberOfTransformations[i] > 0)
				{
					RenderingSystem::Instance->DestroyConstantBuffer(update->_Information->_PatchRenderInformations[update->_Index]._TransformationsBuffers[i]);
				}
			}

			update->_Information->_PatchInformations[update->_Index]._TimeStamp = update->_NewPatchInformation._TimeStamp;
			update->_Information->_PatchInformations[update->_Index]._AxisAlignedBoundingBoxes = update->_NewPatchInformation._AxisAlignedBoundingBoxes;
			update->_Information->_PatchRenderInformations[update->_Index]._TransformationsBuffers = update->_NewPatchRenderInformation._TransformationsBuffers;
			update->_Information->_PatchRenderInformations[update->_Index]._NumberOfTransformations = update->_NewPatchRenderInformation._NumberOfTransformations;
		}

		else
		{
			for (const uint8 index : update->_PatchesToInvalidate)
			{
				VegetationUtilities::InvalidatePatch(update->_Information, index);
			}

			for (uint64 i = 0, size = update->_Information->_PatchInformations.Size(); i < size; ++i)
			{
				if (!update->_Information->_PatchInformations[i]._Valid)
				{
					update->_Information->_PatchInformations[i] = update->_NewPatchInformation;
					update->_Information->_PatchRenderInformations[i] = update->_NewPatchRenderInformation;

					break;
				}
			}
		}
	}

	/*
	*	Sorts a set of transformations into different level of details.
	*/
	template <typename TYPE>
	void SortTransformations(	const DynamicArray<Matrix4> &transformations,
								const TYPE &properties,
								StaticArray<DynamicArray<Matrix4>, UNDERLYING(VegetationLevelOfDetail::NumberOfVegetationLevelOfDetails)> *const RESTRICT levelOfDetailTransformations,
								StaticArray<ConstantBufferHandle, UNDERLYING(VegetationLevelOfDetail::NumberOfVegetationLevelOfDetails)> *const RESTRICT buffers,
								StaticArray<uint32, UNDERLYING(VegetationLevelOfDetail::NumberOfVegetationLevelOfDetails)> *const RESTRICT numberOfTransformations) NOEXCEPT
	{
		//Cache the viewer position.
		const Vector3<float> viewerPosition{ Viewer::Instance->GetPosition() };

		for (const Matrix4 &transformation : transformations)
		{
			//Calculate the distance to the viewer.
			const float distanceToViewer{ Vector3<float>::Length(viewerPosition - transformation.GetTranslation()) };

			if (distanceToViewer > properties._LowDetailDistance)
			{
				levelOfDetailTransformations->At(UNDERLYING(VegetationLevelOfDetail::Low)).EmplaceSlow(transformation);
			}

			else if (distanceToViewer > properties._MediumDetailDistance)
			{
				levelOfDetailTransformations->At(UNDERLYING(VegetationLevelOfDetail::Medium)).EmplaceSlow(transformation);
			}

			else
			{
				levelOfDetailTransformations->At(UNDERLYING(VegetationLevelOfDetail::High)).EmplaceSlow(transformation);
			}
		}

		for (uint8 i{ 0 }; i < UNDERLYING(VegetationLevelOfDetail::NumberOfVegetationLevelOfDetails); ++i)
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
	*	Updates a vegetation type.
	*/
	template <typename TYPE_INFORMATION_TYPE, typename UPDATE_TYPE>
	void UpdateVegetationType(DynamicArray<TYPE_INFORMATION_TYPE> &informations, UPDATE_TYPE *const RESTRICT update) NOEXCEPT
	{
		//Cache the viewer position.
		const Vector3<float> viewerPosition{ Viewer::Instance->GetPosition() };

		//Reset the vegetation type information update.
		update->_Information = nullptr;
		update->_PatchesToInvalidate.ClearFast();
		update->_NewPatchInformation._Transformations.ClearFast();

		//Update all vegetation type informations.
		for (TYPE_INFORMATION_TYPE &information : informations)
		{
			//Calculate the current grid point based on the current viewer position.
			const GridPoint2 currentGridPoint{ GridPoint2::WorldPositionToGridPoint(viewerPosition, information._Properties._CutoffDistance) };

			//Create an array with the valid grid positions.
			StaticArray<GridPoint2, 9> validGridPoints
			{
				GridPoint2(currentGridPoint._X - 1, currentGridPoint._Y - 1),
				GridPoint2(currentGridPoint._X, currentGridPoint._Y - 1),
				GridPoint2(currentGridPoint._X + 1, currentGridPoint._Y - 1),

				GridPoint2(currentGridPoint._X - 1, currentGridPoint._Y),
				GridPoint2(currentGridPoint._X, currentGridPoint._Y),
				GridPoint2(currentGridPoint._X + 1, currentGridPoint._Y),

				GridPoint2(currentGridPoint._X - 1, currentGridPoint._Y + 1),
				GridPoint2(currentGridPoint._X, currentGridPoint._Y + 1),
				GridPoint2(currentGridPoint._X + 1, currentGridPoint._Y + 1),
			};

			//Construct the sorting data.
			class SortingData final
			{

			public:

				//The current viewer position.
				Vector3<float> _ViewerPosition;

				//The cutoff distance.
				float _CutoffDistance;
			};

			SortingData sortingData;

			sortingData._ViewerPosition = viewerPosition;
			sortingData._CutoffDistance = information._Properties._CutoffDistance;

			//Sort the array so that the closest grid point is first.
			SortingAlgorithms::InsertionSort<GridPoint2>(validGridPoints.begin(), validGridPoints.end(), &sortingData, [](const void *const RESTRICT userData, const GridPoint2 *const RESTRICT first, const GridPoint2 *const RESTRICT second)
			{
				const SortingData *const RESTRICT sortingData{ static_cast<const SortingData *const RESTRICT>(userData) };

				const Vector3<float> firstGridPosition{ GridPoint2::GridPointToWorldPosition(*first, sortingData->_CutoffDistance) };
				const Vector3<float> secondGridPosition{ GridPoint2::GridPointToWorldPosition(*second, sortingData->_CutoffDistance) };

				return Vector3<float>::LengthSquaredXZ(sortingData->_ViewerPosition - firstGridPosition) < Vector3<float>::LengthSquaredXZ(sortingData->_ViewerPosition - secondGridPosition);
			});

			//Construct the update.
			update->_Information = &information;
			update->_NewPatchInformation._Valid = false;

			//Determine the patch indices to invalidate.
			for (uint64 i = 0, size = information._PatchInformations.Size(); i < size; ++i)
			{
				//If this patch is already invalid, no need to invalidate it.
				if (!information._PatchInformations[i]._Valid)
				{
					continue;
				}

				bool valid{ false };

				for (const GridPoint2 &gridPoint : validGridPoints)
				{
					if (information._PatchInformations[i]._GridPoint == gridPoint)
					{
						valid = true;

						break;
					}
				}

				if (!valid)
				{
					update->_PatchesToInvalidate.EmplaceSlow(static_cast<uint8>(i));
				}
			}

			//Determine the patches to update.
			for (const GridPoint2 &gridPoint : validGridPoints)
			{
				bool exists{ false };

				for (uint64 i = 0, size = information._PatchInformations.Size(); i < size; ++i)
				{
					if (information._PatchInformations[i]._Valid && information._PatchInformations[i]._GridPoint == gridPoint)
					{
						exists = true;

						break;
					}
				}

				if (!exists)
				{
					//Construct the update.
					update->_NewPatchInformation._Valid = true;
					update->_NewPatchInformation._GridPoint = gridPoint;

					//Generate the transformations.
					VegetationUtilities::GenerateTransformations(	gridPoint,
																	information._Properties,
																	&update->_NewPatchInformation._Transformations);

					//Sort the transformations.
					StaticArray<DynamicArray<Matrix4>, UNDERLYING(VegetationLevelOfDetail::NumberOfVegetationLevelOfDetails)> levelOfDetailTransformations;

					VegetationUtilities::SortTransformations(	update->_NewPatchInformation._Transformations,
																information._Properties,
																&levelOfDetailTransformations,
																&update->_NewPatchRenderInformation._TransformationsBuffers,
																&update->_NewPatchRenderInformation._NumberOfTransformations);

					update->_NewPatchInformation._TimeStamp = EngineSystem::Instance->GetTotalFrames();

					for (uint8 i{ 0 }; i < UNDERLYING(VegetationLevelOfDetail::NumberOfVegetationLevelOfDetails); ++i)
					{
						update->_NewPatchRenderInformation._Visibilities[i] = VisibilityFlag::None;
					}

					for (uint8 i{ 0 }; i < UNDERLYING(VegetationLevelOfDetail::NumberOfVegetationLevelOfDetails); ++i)
					{
						RenderingUtilities::CalculateAxisAlignedBoundingBoxFromTransformations(	levelOfDetailTransformations[i],
																								information._Model._AxisAlignedBoundingBox,
																								&update->_NewPatchInformation._AxisAlignedBoundingBoxes[i]);
					}

					break;
				}
			}

			//If the new update is valid, return.
			if (!update->_PatchesToInvalidate.Empty() || update->_NewPatchInformation._Valid)
			{
				update->_LevelOfDetailUpdate = false;
				update->_Index = 0;

				return;
			}
		}

		//If no patches needs to be added or replaced, update the level of detail on the patch with the oldest timestamp.
		uint64 oldestTimeStamp{ UINT64_MAXIMUM };
		uint8 index{ UINT8_MAXIMUM };

		for (TYPE_INFORMATION_TYPE &information : informations)
		{
			for (uint8 i{ 0 }; i < information._PatchInformations.Size(); ++i)
			{
				if (information._PatchInformations[i]._Valid && oldestTimeStamp > information._PatchInformations[i]._TimeStamp)
				{
					update->_Information = &information;
					oldestTimeStamp = information._PatchInformations[i]._TimeStamp;
					index = i;
				}
			}
		}

		//Denote that this is a level of detail update.
		update->_LevelOfDetailUpdate = true;
		update->_Index = index;

		//Construct the update.
		update->_NewPatchInformation._TimeStamp = EngineSystem::Instance->GetTotalFrames();

		//Sort the transformations.
		StaticArray<DynamicArray<Matrix4>, UNDERLYING(VegetationLevelOfDetail::NumberOfVegetationLevelOfDetails)> levelOfDetailTransformations;

		VegetationUtilities::SortTransformations(	update->_Information->_PatchInformations[index]._Transformations,
													update->_Information->_Properties,
													&levelOfDetailTransformations,
													&update->_NewPatchRenderInformation._TransformationsBuffers,
													&update->_NewPatchRenderInformation._NumberOfTransformations);

		for (uint8 i{ 0 }; i < UNDERLYING(VegetationLevelOfDetail::NumberOfVegetationLevelOfDetails); ++i)
		{
			RenderingUtilities::CalculateAxisAlignedBoundingBoxFromTransformations(	levelOfDetailTransformations[i],
																					update->_Information->_Model._AxisAlignedBoundingBox,
																					&update->_NewPatchInformation._AxisAlignedBoundingBoxes[i]);
		}
	}
}