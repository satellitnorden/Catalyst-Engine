//Header file.
#include <Systems/CullingSystem.h>

//Components.
#include <Components/ComponentManager.h>

//Rendering.
#include <Rendering/Engine/RenderingUtilities.h>
#include <Rendering/Engine/Viewer.h>

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
	const StaticArray<Vector4<float>, 6> *const RESTRICT directionalLightFrustumPlanes{ LightingSystem::Instance->GetDirectionalLight()->GetFrustumPlanes() };
	const StaticArray<Vector4<float>, 6> *const RESTRICT viewerFrustumPlanes{ Viewer::Instance->GetFrustumPlanes() };

	//Iterate over all debris vegetation type informations, and cull the grid points that is too far away from the viewer.
	for (DebrisVegetationTypeInformation &information : *VegetationSystem::Instance->GetDebrisVegetationTypeInformations())
	{
		for (uint64 i = 0, size = information._PatchInformations.Size(); i < size; ++i)
		{
			//If this patch isn't even valid, don't test it.
			if (!information._PatchInformations[i]._Valid)
			{
				continue;
			}

			//Test this patch's axis-aligned bounding box against the current frustum planes.
			if (RenderingUtilities::IsWithinViewFrustum(*viewerFrustumPlanes, information._PatchInformations[i]._AxisAlignedBoundingBox))
			{
				SET_BIT(information._PatchRenderInformations[i]._Visibility, VisibilityFlag::Viewer);
			}

			else
			{
				CLEAR_BIT(information._PatchRenderInformations[i]._Visibility, VisibilityFlag::Viewer);
			}

			if (RenderingUtilities::IsWithinViewFrustum(*directionalLightFrustumPlanes, information._PatchInformations[i]._AxisAlignedBoundingBox))
			{
				SET_BIT(information._PatchRenderInformations[i]._Visibility, VisibilityFlag::DirectionalLight);
			}

			else
			{
				CLEAR_BIT(information._PatchRenderInformations[i]._Visibility, VisibilityFlag::DirectionalLight);
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
	const StaticArray<Vector4<float>, 6> *const RESTRICT frustumPlanes{ Viewer::Instance->GetFrustumPlanes() };

	//Iterate over all grass vegetation type informations, and cull the grid points that is too far away from the viewer.
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
			for (uint8 j{ 0 }; j < UNDERLYING(GrassVegetationLevelOfDetail::NumberOfGrassVegetationLevelOfDetails); ++j)
			{
				if (RenderingUtilities::IsWithinViewFrustum(*frustumPlanes, information._PatchInformations[i]._AxisAlignedBoundingBoxes[j]))
				{
					SET_BIT(information._PatchRenderInformations[i]._Visibilities[j], VisibilityFlag::Viewer);
				}

				else
				{
					CLEAR_BIT(information._PatchRenderInformations[i]._Visibilities[j], VisibilityFlag::Viewer);
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
	const StaticArray<Vector4<float>, 6> *const RESTRICT frustumPlanes{ Viewer::Instance->GetFrustumPlanes() };

	//Retrieve component data.
	const ParticleSystemComponent *RESTRICT component{ ComponentManager::GetParticleSystemParticleSystemComponents() };
	ParticleSystemRenderComponent *RESTRICT renderComponent{ ComponentManager::GetParticleSystemParticleSystemRenderComponents() };
	const uint64 numberOfComponents{ ComponentManager::GetNumberOfParticleSystemComponents() };

	for (uint64 i{ 0 }; i < numberOfComponents; ++i, ++component, ++renderComponent)
	{
		//Test this particle system's axis-aligned bounding box against the current frustum planes.
		if (RenderingUtilities::IsWithinViewFrustum(*frustumPlanes, component->_AxisAlignedBoundingBox))
		{
			SET_BIT(renderComponent->_Visibility, VisibilityFlag::Viewer);
		}

		else
		{
			CLEAR_BIT(renderComponent->_Visibility, VisibilityFlag::Viewer);
		}
	}
}

/*
*	Culls solid vegetation.
*/
void CullingSystem::CullSolidVegetation() NOEXCEPT
{
	//Get the current frustum planes.
	const StaticArray<Vector4<float>, 6> *const RESTRICT directionalLightFrustumPlanes{ LightingSystem::Instance->GetDirectionalLight()->GetFrustumPlanes() };
	const StaticArray<Vector4<float>, 6> *const RESTRICT viewerFrustumPlanes{ Viewer::Instance->GetFrustumPlanes() };

	//Iterate over all grass vegetation type informations, and cull the grid points that is too far away from the viewer.
	for (SolidVegetationTypeInformation &information : *VegetationSystem::Instance->GetSolidVegetationTypeInformations())
	{
		for (uint64 i = 0, size = information._PatchInformations.Size(); i < size; ++i)
		{
			//If this patch isn't even valid, don't test it.
			if (!information._PatchInformations[i]._Valid)
			{
				continue;
			}

			//Test this patch's axis-aligned bounding box against the current frustum planes.
			if (RenderingUtilities::IsWithinViewFrustum(*viewerFrustumPlanes, information._PatchInformations[i]._AxisAlignedBoundingBox))
			{
				SET_BIT(information._PatchRenderInformations[i]._Visibility, VisibilityFlag::Viewer);
			}

			else
			{
				CLEAR_BIT(information._PatchRenderInformations[i]._Visibility, VisibilityFlag::Viewer);
			}

			if (RenderingUtilities::IsWithinViewFrustum(*directionalLightFrustumPlanes, information._PatchInformations[i]._AxisAlignedBoundingBox))
			{
				SET_BIT(information._PatchRenderInformations[i]._Visibility, VisibilityFlag::DirectionalLight);
			}

			else
			{
				CLEAR_BIT(information._PatchRenderInformations[i]._Visibility, VisibilityFlag::DirectionalLight);
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
	const StaticArray<Vector4<float>, 6> *const RESTRICT frustumPlanes{ Viewer::Instance->GetFrustumPlanes() };

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
			SET_BIT(patchRenderInformation._Visibility, VisibilityFlag::Viewer);
		}

		else
		{
			CLEAR_BIT(patchRenderInformation._Visibility, VisibilityFlag::Viewer);
		}
	}
}