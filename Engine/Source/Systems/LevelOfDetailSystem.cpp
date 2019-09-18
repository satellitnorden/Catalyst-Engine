//Header file.
#include <Systems/LevelOfDetailSystem.h>

//Core.
#include <Core/General/Perceiver.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Rendering.
#include <Rendering/Native/RenderingUtilities.h>

//Systems.
#include <Systems/TaskSystem.h>

//Singleton definition.
DEFINE_SINGLETON(LevelOfDetailSystem);

/*
*	Initializes the level of detail system.
*/
void LevelOfDetailSystem::Initialize() NOEXCEPT
{
	//Initialize the vegetation level of detail task.
	_VegetationLevelOfDetailTask._Function = [](void *const RESTRICT)
	{
		LevelOfDetailSystem::Instance->LevelOfDetailVegetation();
	};
	_VegetationLevelOfDetailTask._Arguments = nullptr;
}

/*
*	Updates the level of detail system during the render update phase.
*/
void LevelOfDetailSystem::RenderUpdate(const UpdateContext *const RESTRICT context) NOEXCEPT
{
	//Execute all tasks.
	TaskSystem::Instance->ExecuteTask(&_VegetationLevelOfDetailTask);
}

/*
*	Calculates level of detail for vegetation.
*/
void LevelOfDetailSystem::LevelOfDetailVegetation() const NOEXCEPT
{
	//Iterate over all vegetation components and calculate their level of detail.
	const uint64 number_of_vegetation_components{ ComponentManager::GetNumberOfVegetationComponents() };
	VegetationComponent *RESTRICT component{ ComponentManager::GetVegetationVegetationComponents() };

	for (uint64 i = 0; i < number_of_vegetation_components; ++i, ++component)
	{
		if (RenderingUtilities::CalculateScreenCoveragePercent(*Perceiver::Instance->GetViewMatrix(), component->_WorldSpaceAxisAlignedBoundingBox) >= 0.75f || true)
		{
			component->_LevelOfDetail = VegetationComponent::LevelOfDetail::Full;
		}
		
		else
		{
			component->_LevelOfDetail = VegetationComponent::LevelOfDetail::Impostor;
		}
	}
}