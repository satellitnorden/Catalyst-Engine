//Header file.
#include <Systems/CullingSystem.h>

//Core.
#include <Core/General/Perceiver.h>

//Components.
#include <Components/ComponentManager.h>

//Rendering.
#include <Rendering/Engine/RenderingUtilities.h>

//Systems.
#include <Systems/RenderingSystem.h>
#include <Systems/LightingSystem.h>
#include <Systems/TaskSystem.h>
#include <Systems/TerrainSystem.h>
#include <Systems/VegetationSystem.h>

//Singleton definition.
DEFINE_SINGLETON(CullingSystem);

/*
*	Initializes the culling system.
*/
void CullingSystem::InitializeSystem() NOEXCEPT
{
	//Initialize all culling tasks.
	_CullingTasks[UNDERLYING(CullingTask::DebrisVegetation)]._Function = [](void *const RESTRICT)
	{
		CullingSystem::Instance->CullDebrisVegetation();
	};
	_CullingTasks[UNDERLYING(CullingTask::DebrisVegetation)]._Arguments = nullptr;

	_CullingTasks[UNDERLYING(CullingTask::GrassVegetation)]._Function = [](void *const RESTRICT)
	{
		CullingSystem::Instance->CullGrassVegetation();
	};
	_CullingTasks[UNDERLYING(CullingTask::GrassVegetation)]._Arguments = nullptr;

	_CullingTasks[UNDERLYING(CullingTask::ParticleSystems)]._Function = [](void *const RESTRICT)
	{
		CullingSystem::Instance->CullParticleSystems();
	};
	_CullingTasks[UNDERLYING(CullingTask::ParticleSystems)]._Arguments = nullptr;

	_CullingTasks[UNDERLYING(CullingTask::SolidVegetation)]._Function = [](void *const RESTRICT)
	{
		CullingSystem::Instance->CullSolidVegetation();
	};
	_CullingTasks[UNDERLYING(CullingTask::SolidVegetation)]._Arguments = nullptr;

	_CullingTasks[UNDERLYING(CullingTask::Terrain)]._Function = [](void *const RESTRICT)
	{
		CullingSystem::Instance->CullTerrain();
	};
	_CullingTasks[UNDERLYING(CullingTask::Terrain)]._Arguments = nullptr;

	_CullingTasks[UNDERLYING(CullingTask::TreeVegetation)]._Function = [](void *const RESTRICT)
	{
		CullingSystem::Instance->CullTreeVegetation();
	};
	_CullingTasks[UNDERLYING(CullingTask::TreeVegetation)]._Arguments = nullptr;
}

/*
*	Updates the culling system synchronously.
*/
void CullingSystem::UpdateSystemSynchronous(const UpdateContext *const RESTRICT context) NOEXCEPT
{
	//Fire off all culling tasks.
	for (Task &task : _CullingTasks)
	{
		TaskSystem::Instance->ExecuteTask(&task);
	}
}

/*
*	Culls debris vegetation.
*/
void CullingSystem::CullDebrisVegetation() NOEXCEPT
{
	//Get the current frustum planes.
	const StaticArray<Vector4<float>, 6> *const RESTRICT frustumPlanes{ Perceiver::Instance->GetFrustumPlanes() };

	//Iterate over all debris vegetation type informations, and cull the grid points that is too far away from the perceiver.
	for (DebrisVegetationTypeInformation &information : *VegetationSystem::Instance->GetDebrisVegetationTypeInformations())
	{
		for (uint64 i = 0, size = information._PatchInformations.Size(); i < size; ++i)
		{
			//If this patch isn't even valid, don't test it.
			if (!information._PatchInformations[i]._Valid)
			{
				continue;
			}

			//Test this patch's axis-aligned bounding boxes against the current frustum planes.
			for (uint8 j{ 0 }; j < UNDERLYING(LevelOfDetail::NumberOfLevelOfDetails); ++j)
			{
				if (RenderingUtilities::IsWithinViewFrustum(*frustumPlanes, information._PatchInformations[i]._AxisAlignedBoundingBoxes[j]))
				{
					SET_BIT(information._PatchRenderInformations[i]._Visibilities[j], VisibilityFlag::Perceiver);
				}

				else
				{
					CLEAR_BIT(information._PatchRenderInformations[i]._Visibilities[j], VisibilityFlag::Perceiver);
				}
			}
		}
	}
}

/*
*	Culls grass vegetation.
*/
void CullingSystem::CullGrassVegetation() NOEXCEPT
{
	//Get the current frustum planes.
	const StaticArray<Vector4<float>, 6> *const RESTRICT frustumPlanes{ Perceiver::Instance->GetFrustumPlanes() };

	//Iterate over all grass vegetation type informations, and cull the grid points that is too far away from the perceiver.
	for (GrassVegetationTypeInformation &information : *VegetationSystem::Instance->GetGrassVegetationTypeInformations())
	{
		for (uint64 i = 0, size = information._PatchInformations.Size(); i < size; ++i)
		{
			//If this patch isn't even valid, don't test it.
			if (!information._PatchInformations[i]._Valid)
			{
				continue;
			}

			//Test this patch's axis-aligned bounding boxes against the current frustum planes.
			for (uint8 j{ 0 }; j < UNDERLYING(LevelOfDetail::NumberOfLevelOfDetails); ++j)
			{
				if (RenderingUtilities::IsWithinViewFrustum(*frustumPlanes, information._PatchInformations[i]._AxisAlignedBoundingBoxes[j]))
				{
					SET_BIT(information._PatchRenderInformations[i]._Visibilities[j], VisibilityFlag::Perceiver);
				}

				else
				{
					CLEAR_BIT(information._PatchRenderInformations[i]._Visibilities[j], VisibilityFlag::Perceiver);
				}
			}
		}
	}
}

/*
*	Culls particle systems.
*/
void CullingSystem::CullParticleSystems() NOEXCEPT
{
	//Get the current frustum planes.
	const StaticArray<Vector4<float>, 6> *const RESTRICT frustumPlanes{ Perceiver::Instance->GetFrustumPlanes() };

	//Retrieve component data.
	const ParticleSystemComponent *RESTRICT component{ ComponentManager::GetParticleSystemParticleSystemComponents() };
	ParticleSystemRenderComponent *RESTRICT renderComponent{ ComponentManager::GetParticleSystemParticleSystemRenderComponents() };
	const uint64 numberOfComponents{ ComponentManager::GetNumberOfParticleSystemComponents() };

	for (uint64 i{ 0 }; i < numberOfComponents; ++i, ++component, ++renderComponent)
	{
		//Test this particle system's axis-aligned bounding box against the current frustum planes.
		if (RenderingUtilities::IsWithinViewFrustum(*frustumPlanes, component->_AxisAlignedBoundingBox))
		{
			SET_BIT(renderComponent->_Visibility, VisibilityFlag::Perceiver);
		}

		else
		{
			CLEAR_BIT(renderComponent->_Visibility, VisibilityFlag::Perceiver);
		}
	}
}

/*
*	Culls solid vegetation.
*/
void CullingSystem::CullSolidVegetation() NOEXCEPT
{
	//Get the current frustum planes.
	const StaticArray<Vector4<float>, 6> *const RESTRICT frustumPlanes{ Perceiver::Instance->GetFrustumPlanes() };

	//Iterate over all solid vegetation type informations, and cull the grid points that is too far away from the perceiver.
	for (SolidVegetationTypeInformation &information : *VegetationSystem::Instance->GetSolidVegetationTypeInformations())
	{
		for (uint64 i = 0, size = information._PatchInformations.Size(); i < size; ++i)
		{
			//If this patch isn't even valid, don't test it.
			if (!information._PatchInformations[i]._Valid)
			{
				continue;
			}

			//Test this patch's axis-aligned bounding boxes against the current frustum planes.
			for (uint8 j{ 0 }; j < UNDERLYING(LevelOfDetail::NumberOfLevelOfDetails); ++j)
			{
				if (RenderingUtilities::IsWithinViewFrustum(*frustumPlanes, information._PatchInformations[i]._AxisAlignedBoundingBoxes[j]))
				{
					SET_BIT(information._PatchRenderInformations[i]._Visibilities[j], VisibilityFlag::Perceiver);
				}

				else
				{
					CLEAR_BIT(information._PatchRenderInformations[i]._Visibilities[j], VisibilityFlag::Perceiver);
				}
			}
		}
	}
}

/*
*	Culls terrain.
*/
void CullingSystem::CullTerrain() NOEXCEPT
{
	//Get the current frustum planes.
	const StaticArray<Vector4<float>, 6> *const RESTRICT frustumPlanes{ Perceiver::Instance->GetFrustumPlanes() };

	//Iterate over all terrain patches and cull them.
	DynamicArray<TerrainPatchInformation> *const RESTRICT patchInformations{ TerrainSystem::Instance->GetTerrainPatchInformations() };
	DynamicArray<TerrainPatchRenderInformation> *const RESTRICT patchRenderInformations{ TerrainSystem::Instance->GetTerrainPatchRenderInformations() };

	for (uint64 i = 0, size = patchInformations->Size(); i < size; ++i)
	{
		TerrainPatchInformation &patchInformation{ (*patchInformations)[i] };
		TerrainPatchRenderInformation &patchRenderInformation{ (*patchRenderInformations)[i] };

		//If this patch is invalid, no need to check it.
		if (!patchInformation._Valid)
		{
			continue;
		}

		//Test this patch's axis-aligned bounding box against the current frustum planes.
		if (RenderingUtilities::IsWithinViewFrustum(*frustumPlanes, patchInformation._AxisAlignedBoundingBox))
		{
			SET_BIT(patchRenderInformation._Visibility, VisibilityFlag::Perceiver);
		}

		else
		{
			CLEAR_BIT(patchRenderInformation._Visibility, VisibilityFlag::Perceiver);
		}
	}
}

/*
*	Culls tree vegetation.
*/
void CullingSystem::CullTreeVegetation() NOEXCEPT
{
	//Get the current frustum planes.
	const StaticArray<Vector4<float>, 6> *const RESTRICT frustumPlanes{ Perceiver::Instance->GetFrustumPlanes() };

	//Iterate over all tree vegetation type informations, and cull the grid points that is too far away from the perceiver.
	for (TreeVegetationTypeInformation &information : *VegetationSystem::Instance->GetTreeVegetationTypeInformations())
	{
		for (uint64 i = 0, size = information._PatchInformations.Size(); i < size; ++i)
		{
			//If this patch isn't even valid, don't test it.
			if (!information._PatchInformations[i]._Valid)
			{
				continue;
			}

			//Test this patch's axis-aligned bounding boxes against the current frustum planes.
			for (uint8 j{ 0 }; j < UNDERLYING(TreeVegetationLevelOfDetail::NumberOfTreeVegetationLevelOfDetails); ++j)
			{
				if (RenderingUtilities::IsWithinViewFrustum(*frustumPlanes, information._PatchInformations[i]._AxisAlignedBoundingBoxes[j]))
				{
					SET_BIT(information._PatchRenderInformations[i]._Visibilities[j], VisibilityFlag::Perceiver);
				}

				else
				{
					CLEAR_BIT(information._PatchRenderInformations[i]._Visibilities[j], VisibilityFlag::Perceiver);
				}
			}
		}
	}
}