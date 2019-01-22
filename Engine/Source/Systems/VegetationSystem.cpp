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

		for (uint8 i{ 0 }; i < UNDERLYING(LevelOfDetail::NumberOfLevelOfDetails); ++i)
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

		for (uint8 i{ 0 }; i < UNDERLYING(LevelOfDetail::NumberOfLevelOfDetails); ++i)
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

		for (uint8 i{ 0 }; i < UNDERLYING(LevelOfDetail::NumberOfLevelOfDetails); ++i)
		{
			information->_PatchRenderInformations[i]._Visibilities[i] = VisibilityFlag::None;
		}
	}
}

/*
*	Adds a tree vegetation type.
*/
void VegetationSystem::AddTreeVegetationType(const TreeVegetationTypeProperties &properties, const TreeVegetationModel &model, const TreeVegetationMaterial &material) NOEXCEPT
{
	//Create the new tree vegetation information.
	_TreeVegetationTypeInformations.EmplaceSlow();
	TreeVegetationTypeInformation *const RESTRICT information{ &_TreeVegetationTypeInformations.Back() };

	//Just copy the properties, the model and the material.
	information->_Properties = properties;
	information->_Model = model;
	information->_Material = material;

	//Fill in the patch and the patch render informations.
	for (uint8 i = 0; i < 9; ++i)
	{
		information->_PatchInformations[i]._Valid = false;

		for (uint8 i{ 0 }; i < UNDERLYING(TreeVegetationLevelOfDetail::NumberOfTreeVegetationLevelOfDetails); ++i)
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
			VegetationUtilities::ProcessUpdate(&_DebrisVegetationTypeInformationUpdate);

			break;
		}

		case VegetationType::Grass:
		{
			VegetationUtilities::ProcessUpdate(&_GrassVegetationTypeInformationUpdate);

			break;
		}

		case VegetationType::Solid:
		{
			VegetationUtilities::ProcessUpdate(&_SolidVegetationTypeInformationUpdate);

			break;
		}

		case VegetationType::Tree:
		{
			VegetationUtilities::ProcessTreeVegetationUpdate(&_TreeVegetationTypeInformationUpdate);

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

		case VegetationType::Tree:
		{
			VegetationUtilities::UpdateTreeVegetationVegetationType(	_TreeVegetationTypeInformations,
														&_TreeVegetationTypeInformationUpdate);

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