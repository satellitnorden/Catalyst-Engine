//Header file.
#include <Systems/VegetationSystem.h>

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
*	Updates the vegetation system synchronously during the closing update phase.
*/
void VegetationSystem::OpeningUpdateSystemSynchronous(const UpdateContext *const RESTRICT context) NOEXCEPT
{
	//If the asynchronous update has finished, cache the relevant data and fire off another update.
	if (_UpdateTask.IsExecuted())
	{
		//Update the vegetation type informations.
		UpdateVegetationTypeInformations();

		const CameraEntity *const RESTRICT camera{ RenderingSystem::Instance->GetActiveCamera() };
		_CurrentCameraPosition = camera->GetPosition();

		TaskSystem::Instance->ExecuteTask(&_UpdateTask);
	}
}

/*
*	Adds a vegetation type.
*/
void VegetationSystem::AddVegetationType(const VegetationTypeProperties &properties, const VegetationModel &model, const VegetationMaterial &material) NOEXCEPT
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
*	Updates the vegetation type informations.
*/
void VegetationSystem::UpdateVegetationTypeInformations() NOEXCEPT
{
	for (VegetationTypeInformationUpdate &update : _VegetationTypeInformationUpdates)
	{
		for (const uint8 index : update._PatchesToInvalidate)
		{
			InvalidatePatch(update._Information, index);
		}

		for (VegetationPatchUpdate &patchUpdate : update._Updates)
		{
			for (uint64 i = 0, size = update._Information->_PatchInformations.Size(); i < size; ++i)
			{
				if (!update._Information->_PatchInformations[i]._Valid)
				{
					update._Information->_PatchInformations[i] = patchUpdate._PatchInformation;
					update._Information->_PatchRenderInformations[i] = patchUpdate._PatchRenderInformation;

					break;
				}
			}
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
	//Clear vegetation type information updates.
	_VegetationTypeInformationUpdates.ClearFast();

	//Update all vegetation type informations.
	for (VegetationTypeInformation &information : _VegetationTypeInformations)
	{
		UpdateVegetationTypeInformation(&information);
	}
}

/*
*	Updates a single vegetation type information.
*/
void VegetationSystem::UpdateVegetationTypeInformation(VegetationTypeInformation *const RESTRICT information) NOEXCEPT
{
	//Calculate the current grid point based on the current camera position.
	const GridPoint currentGridPoint{ GridPoint::WorldPositionToGridPoint(_CurrentCameraPosition, information->_Properties._CutoffDistance * 2.0f) };

	//Create an array with the valid grid positions.
	const StaticArray<GridPoint, 9> validGridPoints
	{
		GridPoint(currentGridPoint._X - 1, currentGridPoint._Y - 1),
		GridPoint(currentGridPoint._X, currentGridPoint._Y - 1),
		GridPoint(currentGridPoint._X + 1, currentGridPoint._Y - 1),

		GridPoint(currentGridPoint._X - 1, currentGridPoint._Y),
		GridPoint(currentGridPoint._X, currentGridPoint._Y),
		GridPoint(currentGridPoint._X + 1, currentGridPoint._Y),

		GridPoint(currentGridPoint._X - 1, currentGridPoint._Y + 1),
		GridPoint(currentGridPoint._X, currentGridPoint._Y + 1),
		GridPoint(currentGridPoint._X + 1, currentGridPoint._Y + 1),
	};

	//Construct the update.
	VegetationTypeInformationUpdate update;

	update._Information = information;

	//Determine the patch indices to invalidate.
	for (uint64 i = 0, size = information->_PatchInformations.Size(); i < size; ++i)
	{
		//If this patch is already invalid, no need to invalidate it.
		if (!information->_PatchInformations[i]._Valid)
		{
			continue;
		}

		bool valid{ false };

		for (const GridPoint &gridPoint : validGridPoints)
		{
			if (information->_PatchInformations[i]._GridPoint == gridPoint)
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
	for (const GridPoint &gridPoint : validGridPoints)
	{
		bool exists{ false };

		for (uint64 i = 0, size = information->_PatchInformations.Size(); i < size; ++i)
		{
			if (information->_PatchInformations[i]._Valid && information->_PatchInformations[i]._GridPoint == gridPoint)
			{
				exists = true;

				break;
			}
		}

		if (!exists)
		{
			//Construct the update.
			VegetationPatchUpdate patchUpdate;

			patchUpdate._PatchInformation._Valid = true;
			patchUpdate._PatchInformation._GridPoint = gridPoint;

			patchUpdate._PatchRenderInformation._Draw = true;
			GenerateTransformations(gridPoint,
									information->_Properties,
									&patchUpdate._PatchRenderInformation._TransformationsBuffer,
									&patchUpdate._PatchRenderInformation._NumberOfTransformations);

			update._Updates.EmplaceSlow(patchUpdate);
		}
	}

	//Add the new update to the vegetation type information updates.
	if (!update._PatchesToInvalidate.Empty() || !update._Updates.Empty())
	{
		_VegetationTypeInformationUpdates.EmplaceSlow(update);
	}
}

/*
*	Generates the transformations.
*/
void VegetationSystem::GenerateTransformations(const GridPoint &gridPoint, const VegetationTypeProperties &properties, ConstantBufferHandle *const RESTRICT buffer, uint32 *const RESTRICT numberOfTransformations) NOEXCEPT
{
	//Construct the box.
	const Vector3 worldPosition{ GridPoint::GridPointToWorldPosition(gridPoint, properties._CutoffDistance * 2.0f) };
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