//Header file.
#include <Systems/VegetationSystem.h>

//Core.
#include <Core/Algorithms/SortingAlgorithms.h>

//Rendering.
#include <Rendering/Engine/RenderingUtilities.h>
#include <Rendering/Engine/Viewer.h>

//Systems.
#include <Systems/RenderingSystem.h>
#include <Systems/TaskSystem.h>

//Vegetation.
#include <Vegetation/VegetationUtilities.h>

//Singleton definition.
DEFINE_SINGLETON(VegetationSystem);

/*
*	Initializes the vegetation system.
*/
void VegetationSystem::InitializeSystem()
{
	//Set the function for the update task.
	_UpdateTask._Function = [](void *const RESTRICT)
	{
		VegetationSystem::Instance->UpdateSystemAsynchronous();
	};
	_UpdateTask._Arguments = nullptr;
}

/*
*	Updates the vegetation system sequentially.
*/
void VegetationSystem::SequentialUpdateSystemSynchronous() NOEXCEPT
{
	//If the asynchronous update has finished, cache the relevant data and fire off another update.
	if (_UpdateTask.IsExecuted())
	{
		//Process the vegetation type information update.
		ProcessVegetationTypeInformationUpdate();

		//Select the vegetation type to update.
		if (UNDERLYING(_VegetationTypeToUpdate) < (UNDERLYING(VegetationType::NumberOfVegetationTypes) - 1))
		{
			_VegetationTypeToUpdate = static_cast<VegetationType>(UNDERLYING(_VegetationTypeToUpdate) + 1);
		}

		else
		{
			_VegetationTypeToUpdate = static_cast<VegetationType>(0);
		}

		//Fire off another update!
		TaskSystem::Instance->ExecuteTask(&_UpdateTask);
	}
}

/*
*	Adds a grass vegetation type.
*/
void VegetationSystem::AddGrassVegetationType(const GrassVegetationTypeProperties &properties, const GrassModel &model, const GrassVegetationMaterial &material) NOEXCEPT
{
	//Create the new grass vegetation information.
	_GrassVegetationTypeInformations.EmplaceSlow();
	GrassVegetationTypeInformation *const RESTRICT information{ &_GrassVegetationTypeInformations.Back() };

	//Just copy the properties, the model and the material.
	information->_Properties = properties;
	information->_Model = model;
	information->_Material = material;

	//Fill in the patch and the patch render informations.
	for (uint8 i = 0; i < 9; ++i)
	{
		information->_PatchInformations[i]._Valid = false;
		information->_PatchRenderInformations[i]._Draw = false;
	}
}

/*
*	Adds a solid vegetation type.
*/
void VegetationSystem::AddSolidVegetationType(const SolidVegetationTypeProperties &properties, const StaticArray<PhysicalModel, UNDERLYING(VegetationLevelOfDetail::NumberOfVegetationLevelOfDetails)> &models, const PhysicalMaterial &material) NOEXCEPT
{
	//Create the new solid vegetation information.
	_SolidVegetationTypeInformations.EmplaceSlow();
	SolidVegetationTypeInformation *const RESTRICT information{ &_SolidVegetationTypeInformations.Back() };

	//Just copy the properties, the model and the material.
	information->_Properties = properties;
	information->_Models = models;
	information->_Material = material;

	//Fill in the patch and the patch render informations.
	for (uint8 i = 0; i < 9; ++i)
	{
		information->_PatchInformations[i]._Valid = false;
		information->_PatchRenderInformations[i]._Draw = false;
	}
}

/*
*	Processes the vegetation type information update.
*/
void VegetationSystem::ProcessVegetationTypeInformationUpdate() NOEXCEPT
{
	//Update the current vegetation type to update.
	switch (_VegetationTypeToUpdate)
	{
		case VegetationType::Grass:
		{
			//Return early if there's no grass vegetation type information to update.
			if (!_GrassVegetationTypeInformationUpdate._Information)
			{
				return;
			}

			for (const uint8 index : _GrassVegetationTypeInformationUpdate._PatchesToInvalidate)
			{
				VegetationUtilities::InvalidatePatch(_GrassVegetationTypeInformationUpdate._Information, index);
			}

			for (uint64 i = 0, size = _GrassVegetationTypeInformationUpdate._Information->_PatchInformations.Size(); i < size; ++i)
			{
				if (!_GrassVegetationTypeInformationUpdate._Information->_PatchInformations[i]._Valid)
				{
					_GrassVegetationTypeInformationUpdate._Information->_PatchInformations[i] = _GrassVegetationTypeInformationUpdate._NewPatchInformation;
					_GrassVegetationTypeInformationUpdate._Information->_PatchRenderInformations[i] = _GrassVegetationTypeInformationUpdate._NewPatchRenderInformation;

					break;
				}
			}

			break;
		}

		case VegetationType::Solid:
		{
			//Return early if there's no solid vegetation type information to update.
			if (!_SolidVegetationTypeInformationUpdate._Information)
			{
				return;
			}

			for (const uint8 index : _SolidVegetationTypeInformationUpdate._PatchesToInvalidate)
			{
				VegetationUtilities::InvalidatePatch(_SolidVegetationTypeInformationUpdate._Information, index);
			}

			for (uint64 i = 0, size = _SolidVegetationTypeInformationUpdate._Information->_PatchInformations.Size(); i < size; ++i)
			{
				if (!_SolidVegetationTypeInformationUpdate._Information->_PatchInformations[i]._Valid)
				{
					_SolidVegetationTypeInformationUpdate._Information->_PatchInformations[i] = _SolidVegetationTypeInformationUpdate._NewPatchInformation;
					_SolidVegetationTypeInformationUpdate._Information->_PatchRenderInformations[i] = _SolidVegetationTypeInformationUpdate._NewPatchRenderInformation;

					break;
				}
			}

			break;
		}
	}
}

/*
*	Updates the vegetation system asynchronously.
*/
void VegetationSystem::UpdateSystemAsynchronous() NOEXCEPT
{
	//Update the current vegetation type to update.
	switch (_VegetationTypeToUpdate)
	{
		case VegetationType::Grass:
		{
			UpdateGrassVegetationAsynchronous();

			break;
		}

		case VegetationType::Solid:
		{
			UpdateSolidVegetationAsynchronous();

			break;
		}

#if !defined(CATALYST_FINAL)
		default:
		{
			ASSERT(false, "Unhandled case. ):");

			break;
		}
#endif
	}
}

/*
*	Updates the grass vegetation asynchonously.
*/
void VegetationSystem::UpdateGrassVegetationAsynchronous() NOEXCEPT
{
	//Cache the viewer position.
	const Vector3 viewerPosition{ Viewer::Instance->GetPosition() };

	//Reset vegetation type information update.
	_GrassVegetationTypeInformationUpdate._Information = nullptr;

	//Update all vegetation type informations.
	for (GrassVegetationTypeInformation &information : _GrassVegetationTypeInformations)
	{
		//Calculate the current grid point based on the current viewer position.
		const GridPoint2 currentGridPoint{ GridPoint2::WorldPositionToGridPoint(viewerPosition, information._Properties._CutoffDistance * VegetationConstants::VEGETATION_GRID_SIZE) };

		//Create an array with the valid grid positions.
		StaticArray<GridPoint2, 25> validGridPoints
		{

			GridPoint2(currentGridPoint._X - 2, currentGridPoint._Y - 2),
			GridPoint2(currentGridPoint._X - 1, currentGridPoint._Y - 2),
			GridPoint2(currentGridPoint._X, currentGridPoint._Y - 2),
			GridPoint2(currentGridPoint._X + 1, currentGridPoint._Y - 2),
			GridPoint2(currentGridPoint._X + 2, currentGridPoint._Y - 2),

			GridPoint2(currentGridPoint._X - 2, currentGridPoint._Y - 1),
			GridPoint2(currentGridPoint._X - 1, currentGridPoint._Y - 1),
			GridPoint2(currentGridPoint._X, currentGridPoint._Y - 1),
			GridPoint2(currentGridPoint._X + 1, currentGridPoint._Y - 1),
			GridPoint2(currentGridPoint._X + 2, currentGridPoint._Y - 1),

			GridPoint2(currentGridPoint._X - 2, currentGridPoint._Y),
			GridPoint2(currentGridPoint._X - 1, currentGridPoint._Y),
			GridPoint2(currentGridPoint._X, currentGridPoint._Y),
			GridPoint2(currentGridPoint._X + 1, currentGridPoint._Y),
			GridPoint2(currentGridPoint._X + 2, currentGridPoint._Y),

			GridPoint2(currentGridPoint._X - 2, currentGridPoint._Y + 1),
			GridPoint2(currentGridPoint._X - 1, currentGridPoint._Y + 1),
			GridPoint2(currentGridPoint._X, currentGridPoint._Y + 1),
			GridPoint2(currentGridPoint._X + 1, currentGridPoint._Y + 1),
			GridPoint2(currentGridPoint._X + 2, currentGridPoint._Y + 1),

			GridPoint2(currentGridPoint._X - 2, currentGridPoint._Y + 2),
			GridPoint2(currentGridPoint._X - 1, currentGridPoint._Y + 2),
			GridPoint2(currentGridPoint._X, currentGridPoint._Y + 2),
			GridPoint2(currentGridPoint._X + 1, currentGridPoint._Y + 2),
			GridPoint2(currentGridPoint._X + 2, currentGridPoint._Y + 2),
		};

		//Construct the sorting data.
		class SortingData final
		{

		public:

			//The current viewer position.
			Vector3 _ViewerPosition;

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

			const Vector3 firstGridPosition{ GridPoint2::GridPointToWorldPosition(*first, sortingData->_CutoffDistance * VegetationConstants::VEGETATION_GRID_SIZE) };
			const Vector3 secondGridPosition{ GridPoint2::GridPointToWorldPosition(*second, sortingData->_CutoffDistance * VegetationConstants::VEGETATION_GRID_SIZE) };

			return Vector3::LengthSquaredXZ(sortingData->_ViewerPosition - firstGridPosition) < Vector3::LengthSquaredXZ(sortingData->_ViewerPosition - secondGridPosition);
		});

		//Construct the update.
		GrassVegetationTypeInformationUpdate update;

		update._Information = &information;
		update._NewPatchInformation._Valid = false;

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
				update._PatchesToInvalidate.EmplaceSlow(static_cast<uint8>(i));
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
				DynamicArray<Matrix4> transformations;
				update._NewPatchRenderInformation._Draw = true;
				VegetationUtilities::GenerateTransformations(	gridPoint,
																information._Properties,
																VegetationConstants::VEGETATION_GRID_SIZE,
																&transformations,
																&update._NewPatchRenderInformation._TransformationsBuffer,
																&update._NewPatchRenderInformation._NumberOfTransformations);

				update._NewPatchInformation._Valid = true;
				update._NewPatchInformation._GridPoint = gridPoint;

				break;
			}
		}

		//If the new update is valid, copy it and return.
		if (!update._PatchesToInvalidate.Empty() || update._NewPatchInformation._Valid)
		{
			_GrassVegetationTypeInformationUpdate = update;

			return;
		}
	}
}

/*
*	Updates the solid vegetation asynchonously.
*/
void VegetationSystem::UpdateSolidVegetationAsynchronous() NOEXCEPT
{
	//Cache the viewer position.
	const Vector3 viewerPosition{ Viewer::Instance->GetPosition() };

	//Reset the solid vegetation type information update.
	_SolidVegetationTypeInformationUpdate._Information = nullptr;

	//Update all solid vegetation type informations.
	for (SolidVegetationTypeInformation &information : _SolidVegetationTypeInformations)
	{
		//Calculate the current grid point based on the current viewer position.
		const GridPoint2 currentGridPoint{ GridPoint2::WorldPositionToGridPoint(viewerPosition, information._Properties._CutoffDistance * VegetationConstants::VEGETATION_GRID_SIZE) };

		//Create an array with the valid grid positions.
		StaticArray<GridPoint2, 25> validGridPoints
		{

			GridPoint2(currentGridPoint._X - 2, currentGridPoint._Y - 2),
			GridPoint2(currentGridPoint._X - 1, currentGridPoint._Y - 2),
			GridPoint2(currentGridPoint._X, currentGridPoint._Y - 2),
			GridPoint2(currentGridPoint._X + 1, currentGridPoint._Y - 2),
			GridPoint2(currentGridPoint._X + 2, currentGridPoint._Y - 2),

			GridPoint2(currentGridPoint._X - 2, currentGridPoint._Y - 1),
			GridPoint2(currentGridPoint._X - 1, currentGridPoint._Y - 1),
			GridPoint2(currentGridPoint._X, currentGridPoint._Y - 1),
			GridPoint2(currentGridPoint._X + 1, currentGridPoint._Y - 1),
			GridPoint2(currentGridPoint._X + 2, currentGridPoint._Y - 1),

			GridPoint2(currentGridPoint._X - 2, currentGridPoint._Y),
			GridPoint2(currentGridPoint._X - 1, currentGridPoint._Y),
			GridPoint2(currentGridPoint._X, currentGridPoint._Y),
			GridPoint2(currentGridPoint._X + 1, currentGridPoint._Y),
			GridPoint2(currentGridPoint._X + 2, currentGridPoint._Y),

			GridPoint2(currentGridPoint._X - 2, currentGridPoint._Y + 1),
			GridPoint2(currentGridPoint._X - 1, currentGridPoint._Y + 1),
			GridPoint2(currentGridPoint._X, currentGridPoint._Y + 1),
			GridPoint2(currentGridPoint._X + 1, currentGridPoint._Y + 1),
			GridPoint2(currentGridPoint._X + 2, currentGridPoint._Y + 1),

			GridPoint2(currentGridPoint._X - 2, currentGridPoint._Y + 2),
			GridPoint2(currentGridPoint._X - 1, currentGridPoint._Y + 2),
			GridPoint2(currentGridPoint._X, currentGridPoint._Y + 2),
			GridPoint2(currentGridPoint._X + 1, currentGridPoint._Y + 2),
			GridPoint2(currentGridPoint._X + 2, currentGridPoint._Y + 2),
		};

		//Construct the sorting data.
		class SortingData final
		{

		public:

			//The current viewer position.
			Vector3 _ViewerPosition;

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

			const Vector3 firstGridPosition{ GridPoint2::GridPointToWorldPosition(*first, sortingData->_CutoffDistance * VegetationConstants::VEGETATION_GRID_SIZE) };
			const Vector3 secondGridPosition{ GridPoint2::GridPointToWorldPosition(*second, sortingData->_CutoffDistance * VegetationConstants::VEGETATION_GRID_SIZE) };

			return Vector3::LengthSquaredXZ(sortingData->_ViewerPosition - firstGridPosition) < Vector3::LengthSquaredXZ(sortingData->_ViewerPosition - secondGridPosition);
		});

		//Construct the update.
		SolidVegetationTypeInformationUpdate update;

		update._Information = &information;
		update._NewPatchInformation._Valid = false;

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
				update._PatchesToInvalidate.EmplaceSlow(static_cast<uint8>(i));
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
				DynamicArray<Matrix4> transformations;
				update._NewPatchRenderInformation._Draw = true;
				VegetationUtilities::GenerateTransformations(	gridPoint,
																information._Properties,
																VegetationConstants::VEGETATION_GRID_SIZE,
																&transformations,
																&update._NewPatchRenderInformation._TransformationsBuffer,
																&update._NewPatchRenderInformation._NumberOfTransformations);

				update._NewPatchInformation._Valid = true;
				update._NewPatchInformation._GridPoint = gridPoint;
				RenderingUtilities::CalculateAxisAlignedBoundingBoxFromTransformations(	transformations,
																						information._Models[0]._AxisAlignedBoundingBox,
																						&update._NewPatchInformation._AxisAlignedBoundingBox);

				break;
			}
		}

		//If the new update is valid, copy it and return.
		if (!update._PatchesToInvalidate.Empty() || update._NewPatchInformation._Valid)
		{
			_SolidVegetationTypeInformationUpdate = update;

			return;
		}
	}
}