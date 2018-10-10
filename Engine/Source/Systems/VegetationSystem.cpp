//Header file.
#include <Systems/VegetationSystem.h>

//Core.
#include <Core/Algorithms/SortingAlgorithms.h>

//Entities.
#include <Entities/CameraEntity.h>

//Rendering.
#include <Rendering/Engine/RenderingUtilities.h>

//Systems.
#include <Systems/RenderingSystem.h>
#include <Systems/TaskSystem.h>

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

		const CameraEntity *const RESTRICT camera{ RenderingSystem::Instance->GetActiveCamera() };
		_CurrentCameraPosition = camera->GetPosition();

		TaskSystem::Instance->ExecuteTask(&_UpdateTask);
	}
}

/*
*	Adds a vegetation type.
*/
void VegetationSystem::AddVegetationType(const VegetationTypeProperties &properties, const VegetationModel &model, const GrassVegetationMaterial &material) NOEXCEPT
{
	//Create the new vegetation information.
	_VegetationTypeInformations.EmplaceSlow();
	VegetationTypeInformation *const RESTRICT information{ &_VegetationTypeInformations.Back() };

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
*	Processes the vegetation type information update.
*/
void VegetationSystem::ProcessVegetationTypeInformationUpdate() NOEXCEPT
{
	//Return early if there's no vegetation type information to update.
	if (!_VegetationTypeInformationUpdate._Information)
	{
		return;
	}

	for (const uint8 index : _VegetationTypeInformationUpdate._PatchesToInvalidate)
	{
		InvalidatePatch(_VegetationTypeInformationUpdate._Information, index);
	}

	for (uint64 i = 0, size = _VegetationTypeInformationUpdate._Information->_PatchInformations.Size(); i < size; ++i)
	{
		if (!_VegetationTypeInformationUpdate._Information->_PatchInformations[i]._Valid)
		{
			_VegetationTypeInformationUpdate._Information->_PatchInformations[i] = _VegetationTypeInformationUpdate._NewPatchInformation;
			_VegetationTypeInformationUpdate._Information->_PatchRenderInformations[i] = _VegetationTypeInformationUpdate._NewPatchRenderInformation;

			break;
		}
	}
}

/*
*	Invalidates one patch.
*/
void VegetationSystem::InvalidatePatch(VegetationTypeInformation *const RESTRICT information, const uint8 index) NOEXCEPT
{
	//Invalidate the patch.
	information->_PatchInformations[index]._Valid = false;
	information->_PatchRenderInformations[index]._Draw = false;

	if (information->_PatchRenderInformations[index]._NumberOfTransformations > 0)
	{
		RenderingSystem::Instance->DestroyConstantBuffer(information->_PatchRenderInformations[index]._TransformationsBuffer);
	}
}

/*
*	Updates the vegetation system asynchronously.
*/
void VegetationSystem::UpdateSystemAsynchronous() NOEXCEPT
{
	//Reset vegetation type information update.
	_VegetationTypeInformationUpdate._Information = nullptr;

	//Update all vegetation type informations.
	for (VegetationTypeInformation &information : _VegetationTypeInformations)
	{
		//Calculate the current grid point based on the current camera position.
		const GridPoint2 currentGridPoint{ GridPoint2::WorldPositionToGridPoint(_CurrentCameraPosition, information._Properties._CutoffDistance * 2.0f) };

		//Create an array with the valid grid positions.
		StaticArray<GridPoint2, 9> validGridPoints
		{
			GridPoint2(currentGridPoint._X, currentGridPoint._Y),

			GridPoint2(currentGridPoint._X - 1, currentGridPoint._Y - 1),
			GridPoint2(currentGridPoint._X, currentGridPoint._Y - 1),
			GridPoint2(currentGridPoint._X + 1, currentGridPoint._Y - 1),

			GridPoint2(currentGridPoint._X - 1, currentGridPoint._Y),
			GridPoint2(currentGridPoint._X + 1, currentGridPoint._Y),

			GridPoint2(currentGridPoint._X - 1, currentGridPoint._Y + 1),
			GridPoint2(currentGridPoint._X, currentGridPoint._Y + 1),
			GridPoint2(currentGridPoint._X + 1, currentGridPoint._Y + 1),
		};

		//Construct the sorting data.
		class SortingData final
		{

		public:

			//The current camera position.
			Vector3 _CameraPosition;

			//The cutoff distance.
			float _CutoffDistance;
		};

		SortingData sortingData;

		sortingData._CameraPosition = _CurrentCameraPosition;
		sortingData._CutoffDistance = information._Properties._CutoffDistance;

		//Sort the array so that the closest grid point is first.
		SortingAlgorithms::InsertionSort<GridPoint2>(validGridPoints.begin(), validGridPoints.end(), &sortingData, [](const void *const RESTRICT userData, const GridPoint2 *const RESTRICT first, const GridPoint2 *const RESTRICT second)
		{
			const SortingData *const RESTRICT sortingData{ static_cast<const SortingData *const RESTRICT>(userData) };

			const Vector3 firstGridPosition{ GridPoint2::GridPointToWorldPosition(*first, sortingData->_CutoffDistance * 2.0f) };
			const Vector3 secondGridPosition{ GridPoint2::GridPointToWorldPosition(*second, sortingData->_CutoffDistance * 2.0f) };

			return Vector3::LengthSquaredXZ(sortingData->_CameraPosition - firstGridPosition) < Vector3::LengthSquaredXZ(sortingData->_CameraPosition - secondGridPosition);
		});

		//Construct the update.
		VegetationTypeInformationUpdate update;

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
				update._NewPatchInformation._Valid = true;
				update._NewPatchInformation._GridPoint = gridPoint;

				update._NewPatchRenderInformation._Draw = true;
				GenerateTransformations(	gridPoint,
											information._Properties,
											&update._NewPatchRenderInformation._TransformationsBuffer,
											&update._NewPatchRenderInformation._NumberOfTransformations);

				break;
			}
		}

		//If the new update is valid, copy it and return.
		if (!update._PatchesToInvalidate.Empty() || update._NewPatchInformation._Valid)
		{
			_VegetationTypeInformationUpdate = update;

			return;
		}
	}
}

/*
*	Generates the transformations.
*/
void VegetationSystem::GenerateTransformations(const GridPoint2 &gridPoint, const VegetationTypeProperties &properties, ConstantBufferHandle *const RESTRICT buffer, uint32 *const RESTRICT numberOfTransformations) NOEXCEPT
{
	//Construct the box.
	const Vector3 worldPosition{ GridPoint2::GridPointToWorldPosition(gridPoint, properties._CutoffDistance * 2.0f) };
	const AxisAlignedBoundingBox box{ worldPosition - properties._CutoffDistance , worldPosition + properties._CutoffDistance };

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