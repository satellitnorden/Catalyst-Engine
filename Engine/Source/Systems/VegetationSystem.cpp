//Header file.
#include <Systems/VegetationSystem.h>

//Core.
#include <Core/Algorithms/SortingAlgorithms.h>

//Rendering.
#include <Rendering/Engine/RenderingUtilities.h>
#include <Rendering/Engine/Viewer.h>

//Systems.
#include <Systems/EngineSystem.h>
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
*	Adds a debris vegetation type.
*/
void VegetationSystem::AddDebrisVegetationType(const DebrisVegetationTypeProperties &properties, const PhysicalModel &model, const PhysicalMaterial &material) NOEXCEPT
{
	//Create the new debris vegetation information.
	_DebrisVegetationTypeInformations.EmplaceSlow();
	DebrisVegetationTypeInformation *const RESTRICT information{ &_DebrisVegetationTypeInformations.Back() };

	//Just copy the properties, the model and the material.
	information->_Properties = properties;
	information->_Model = model;
	information->_Material = material;

	//Fill in the patch and the patch render informations.
	for (uint8 i = 0; i < 9; ++i)
	{
		information->_PatchInformations[i]._Valid = false;

		for (uint8 i{ 0 }; i < UNDERLYING(VegetationLevelOfDetail::NumberOfVegetationLevelOfDetails); ++i)
		{
			information->_PatchRenderInformations[i]._Visibilities[i] = VisibilityFlag::None;
		}
	}
}

/*
*	Adds a grass vegetation type.
*/
void VegetationSystem::AddGrassVegetationType(const GrassVegetationTypeProperties &properties, const GrassVegetationModel &model, const GrassVegetationMaterial &material) NOEXCEPT
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

		for (uint8 i{ 0 }; i < UNDERLYING(VegetationLevelOfDetail::NumberOfVegetationLevelOfDetails); ++i)
		{
			information->_PatchRenderInformations[i]._Visibilities[i] = VisibilityFlag::None;
		}
	}
}

/*
*	Adds a solid vegetation type.
*/
void VegetationSystem::AddSolidVegetationType(const SolidVegetationTypeProperties &properties, const PhysicalModel &model, const PhysicalMaterial &material) NOEXCEPT
{
	//Create the new solid vegetation information.
	_SolidVegetationTypeInformations.EmplaceSlow();
	SolidVegetationTypeInformation *const RESTRICT information{ &_SolidVegetationTypeInformations.Back() };

	//Just copy the properties, the model and the material.
	information->_Properties = properties;
	information->_Model = model;
	information->_Material = material;

	//Fill in the patch and the patch render informations.
	for (uint8 i = 0; i < 9; ++i)
	{
		information->_PatchInformations[i]._Valid = false;

		for (uint8 i{ 0 }; i < UNDERLYING(VegetationLevelOfDetail::NumberOfVegetationLevelOfDetails); ++i)
		{
			information->_PatchRenderInformations[i]._Visibilities[i] = VisibilityFlag::None;
		}
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
		case VegetationType::Debris:
		{
			if (_DebrisVegetationTypeInformationUpdate._LevelOfDetailUpdate)
			{
				for (uint8 i{ 0 }; i < UNDERLYING(VegetationLevelOfDetail::NumberOfVegetationLevelOfDetails); ++i)
				{
					if (_DebrisVegetationTypeInformationUpdate._Information->_PatchRenderInformations[_DebrisVegetationTypeInformationUpdate._Index]._NumberOfTransformations[i] > 0)
					{
						RenderingSystem::Instance->DestroyConstantBuffer(_DebrisVegetationTypeInformationUpdate._Information->_PatchRenderInformations[_DebrisVegetationTypeInformationUpdate._Index]._TransformationsBuffers[i]);
					}
				}

				_DebrisVegetationTypeInformationUpdate._Information->_PatchInformations[_GrassVegetationTypeInformationUpdate._Index]._TimeStamp = _DebrisVegetationTypeInformationUpdate._NewPatchInformation._TimeStamp;
				_DebrisVegetationTypeInformationUpdate._Information->_PatchInformations[_GrassVegetationTypeInformationUpdate._Index]._AxisAlignedBoundingBoxes = _DebrisVegetationTypeInformationUpdate._NewPatchInformation._AxisAlignedBoundingBoxes;
				_DebrisVegetationTypeInformationUpdate._Information->_PatchRenderInformations[_GrassVegetationTypeInformationUpdate._Index]._TransformationsBuffers = _DebrisVegetationTypeInformationUpdate._NewPatchRenderInformation._TransformationsBuffers;
				_DebrisVegetationTypeInformationUpdate._Information->_PatchRenderInformations[_GrassVegetationTypeInformationUpdate._Index]._NumberOfTransformations = _DebrisVegetationTypeInformationUpdate._NewPatchRenderInformation._NumberOfTransformations;
			}

			else
			{
				for (const uint8 index : _DebrisVegetationTypeInformationUpdate._PatchesToInvalidate)
				{
					VegetationUtilities::InvalidatePatch(_DebrisVegetationTypeInformationUpdate._Information, index);
				}

				for (uint64 i = 0, size = _DebrisVegetationTypeInformationUpdate._Information->_PatchInformations.Size(); i < size; ++i)
				{
					if (!_DebrisVegetationTypeInformationUpdate._Information->_PatchInformations[i]._Valid)
					{
						_DebrisVegetationTypeInformationUpdate._Information->_PatchInformations[i] = _DebrisVegetationTypeInformationUpdate._NewPatchInformation;
						_DebrisVegetationTypeInformationUpdate._Information->_PatchRenderInformations[i] = _DebrisVegetationTypeInformationUpdate._NewPatchRenderInformation;

						break;
					}
				}
			}

			break;
		}

		case VegetationType::Grass:
		{
			if (_GrassVegetationTypeInformationUpdate._LevelOfDetailUpdate)
			{
				for (uint8 i{ 0 }; i < UNDERLYING(VegetationLevelOfDetail::NumberOfVegetationLevelOfDetails); ++i)
				{
					if (_GrassVegetationTypeInformationUpdate._Information->_PatchRenderInformations[_GrassVegetationTypeInformationUpdate._Index]._NumberOfTransformations[i] > 0)
					{
						RenderingSystem::Instance->DestroyConstantBuffer(_GrassVegetationTypeInformationUpdate._Information->_PatchRenderInformations[_GrassVegetationTypeInformationUpdate._Index]._TransformationsBuffers[i]);
					}
				}

				_GrassVegetationTypeInformationUpdate._Information->_PatchInformations[_GrassVegetationTypeInformationUpdate._Index]._TimeStamp = _GrassVegetationTypeInformationUpdate._NewPatchInformation._TimeStamp;
				_GrassVegetationTypeInformationUpdate._Information->_PatchInformations[_GrassVegetationTypeInformationUpdate._Index]._AxisAlignedBoundingBoxes = _GrassVegetationTypeInformationUpdate._NewPatchInformation._AxisAlignedBoundingBoxes;
				_GrassVegetationTypeInformationUpdate._Information->_PatchRenderInformations[_GrassVegetationTypeInformationUpdate._Index]._TransformationsBuffers = _GrassVegetationTypeInformationUpdate._NewPatchRenderInformation._TransformationsBuffers;
				_GrassVegetationTypeInformationUpdate._Information->_PatchRenderInformations[_GrassVegetationTypeInformationUpdate._Index]._NumberOfTransformations = _GrassVegetationTypeInformationUpdate._NewPatchRenderInformation._NumberOfTransformations;
			}
			
			else
			{
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
			}

			break;
		}

		case VegetationType::Solid:
		{
			if (_SolidVegetationTypeInformationUpdate._LevelOfDetailUpdate)
			{
				for (uint8 i{ 0 }; i < UNDERLYING(VegetationLevelOfDetail::NumberOfVegetationLevelOfDetails); ++i)
				{
					if (_SolidVegetationTypeInformationUpdate._Information->_PatchRenderInformations[_SolidVegetationTypeInformationUpdate._Index]._NumberOfTransformations[i] > 0)
					{
						RenderingSystem::Instance->DestroyConstantBuffer(_SolidVegetationTypeInformationUpdate._Information->_PatchRenderInformations[_SolidVegetationTypeInformationUpdate._Index]._TransformationsBuffers[i]);
					}
				}

				_SolidVegetationTypeInformationUpdate._Information->_PatchInformations[_SolidVegetationTypeInformationUpdate._Index]._TimeStamp = _SolidVegetationTypeInformationUpdate._NewPatchInformation._TimeStamp;
				_SolidVegetationTypeInformationUpdate._Information->_PatchInformations[_SolidVegetationTypeInformationUpdate._Index]._AxisAlignedBoundingBoxes = _SolidVegetationTypeInformationUpdate._NewPatchInformation._AxisAlignedBoundingBoxes;
				_SolidVegetationTypeInformationUpdate._Information->_PatchRenderInformations[_SolidVegetationTypeInformationUpdate._Index]._TransformationsBuffers = _SolidVegetationTypeInformationUpdate._NewPatchRenderInformation._TransformationsBuffers;
				_SolidVegetationTypeInformationUpdate._Information->_PatchRenderInformations[_SolidVegetationTypeInformationUpdate._Index]._NumberOfTransformations = _SolidVegetationTypeInformationUpdate._NewPatchRenderInformation._NumberOfTransformations;
			}

			else
			{
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
		case VegetationType::Debris:
		{
			VegetationUtilities::UpdateVegetationType(	_DebrisVegetationTypeInformations,
														&_DebrisVegetationTypeInformationUpdate);

			break;
		}

		case VegetationType::Grass:
		{
			VegetationUtilities::UpdateVegetationType(	_GrassVegetationTypeInformations,
														&_GrassVegetationTypeInformationUpdate);

			break;
		}

		case VegetationType::Solid:
		{
			VegetationUtilities::UpdateVegetationType(	_SolidVegetationTypeInformations,
														&_SolidVegetationTypeInformationUpdate);

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