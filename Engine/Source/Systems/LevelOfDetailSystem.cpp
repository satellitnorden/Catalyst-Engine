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
	//Initialize the static models level of detail task.
	_StaticModelsLevelOfDetailTask._Function = [](void *const RESTRICT)
	{
		LevelOfDetailSystem::Instance->LevelOfDetailStaticModels();
	};
	_StaticModelsLevelOfDetailTask._Arguments = nullptr;
	_StaticModelsLevelOfDetailTask._ExecutableOnSameThread = true;

	//Initialize the dynamic models level of detail task.
	_DynamicModelsLevelOfDetailTask._Function = [](void *const RESTRICT)
	{
		LevelOfDetailSystem::Instance->LevelOfDetailDynamicModels();
	};
	_DynamicModelsLevelOfDetailTask._Arguments = nullptr;
	_DynamicModelsLevelOfDetailTask._ExecutableOnSameThread = true;
}

/*
*	Updates the level of detail system during the render update phase.
*/
void LevelOfDetailSystem::RenderUpdate(const UpdateContext *const RESTRICT context) NOEXCEPT
{
	//Execute all tasks.
	TaskSystem::Instance->ExecuteTask(&_StaticModelsLevelOfDetailTask);
	TaskSystem::Instance->ExecuteTask(&_DynamicModelsLevelOfDetailTask);
}

/*
*	Calculates level of detail for static models.
*/
void LevelOfDetailSystem::LevelOfDetailStaticModels() const NOEXCEPT
{
	//Define constants.
	constexpr float32 MAXIMUM_DISTANCE{ 1'024.0f };
	constexpr float32 MAXIMUM_DISTANCE_SQUARED{ MAXIMUM_DISTANCE * MAXIMUM_DISTANCE };

	//Cache the perceiver position.
	const Vector3<float> perceiver_position{ Perceiver::Instance->GetWorldTransform().GetAbsolutePosition() };

	//Iterate over all static model components and calculate their level of detail.
	const uint64 number_of_static_model_components{ ComponentManager::GetNumberOfStaticModelComponents() };
	StaticModelComponent *RESTRICT component{ ComponentManager::GetStaticModelStaticModelComponents() };

	for (uint64 i{ 0 }; i < number_of_static_model_components; ++i, ++component)
	{
		for (uint64 j{ 0 }, size{ component->_ModelResource->_Meshes.Size() }; j < size; ++j)
		{
			//If the mesh used only has one level of detail, skip it.
			if (component->_ModelResource->_Meshes[j]._MeshLevelOfDetails.Size() == 1)
			{
				component->_LevelOfDetailIndices[j] = 0;

				continue;
			}

			//Calculate the squared distance.
			const float32 squared_distance{ Vector3<float32>::LengthSquared(perceiver_position - AxisAlignedBoundingBox3D::GetClosestPointInside(component->_WorldSpaceAxisAlignedBoundingBox.GetAbsoluteAxisAlignedBoundingBox(), perceiver_position)) };

			//Calculate the distance coefficient.
			float32 distance_coefficient{ CatalystBaseMath::Minimum<float32>(squared_distance / MAXIMUM_DISTANCE_SQUARED, 1.0f) };

			//Modify the distance coefficient a bit to "flatten the curve", since squared distances are being used.
			distance_coefficient = 1.0f - distance_coefficient;
			distance_coefficient *= distance_coefficient;
			distance_coefficient = 1.0f - distance_coefficient;

			//Calculate the level of detail index.
			component->_LevelOfDetailIndices[j] = static_cast<uint32>(distance_coefficient * static_cast<float32>(component->_ModelResource->_Meshes[j]._MeshLevelOfDetails.Size() - 1));
		}
	}
}

/*
*	Calculates level of detail for dynamice models.
*/
void LevelOfDetailSystem::LevelOfDetailDynamicModels() const NOEXCEPT
{
	//Define constants.
	constexpr float32 MAXIMUM_DISTANCE{ 512.0f };
	constexpr float32 MAXIMUM_DISTANCE_SQUARED{ MAXIMUM_DISTANCE * MAXIMUM_DISTANCE };

	//Cache the perceiver position.
	const Vector3<float> perceiver_position{ Perceiver::Instance->GetWorldTransform().GetAbsolutePosition() };

	//Iterate over all dynamic model components and calculate their level of detail.
	const uint64 number_of_dynamic_model_components{ ComponentManager::GetNumberOfDynamicModelComponents() };
	DynamicModelComponent *RESTRICT component{ ComponentManager::GetDynamicModelDynamicModelComponents() };

	for (uint64 i{ 0 }; i < number_of_dynamic_model_components; ++i, ++component)
	{
		for (uint64 j{ 0 }, size{ component->_ModelResource->_Meshes.Size() }; j < size; ++j)
		{
			//If the mesh used only has one level of detail, skip it.
			if (component->_ModelResource->_Meshes[j]._MeshLevelOfDetails.Size() == 1 || true)
			{
				component->_LevelOfDetailIndices[j] = 0;

				continue;
			}

			//TODO: Shouldn't recaulcate AABB here!
			RenderingUtilities::TransformAxisAlignedBoundingBox(component->_ModelResource->_ModelSpaceAxisAlignedBoundingBox, component->_CurrentWorldTransform.ToAbsoluteMatrix4x4(), &component->_WorldSpaceAxisAlignedBoundingBox);

			//Calculate the squared distance.
			const float32 squared_distance{ Vector3<float32>::LengthSquared(perceiver_position - AxisAlignedBoundingBox3D::GetClosestPointInside(component->_WorldSpaceAxisAlignedBoundingBox, perceiver_position)) };

			//Calculate the distance coefficient.
			float32 distance_coefficient{ CatalystBaseMath::Minimum<float32>(squared_distance / MAXIMUM_DISTANCE_SQUARED, 1.0f) };

			//Modify the distance coefficient a bit to "flatten the curve", since squared distances are being used..
			distance_coefficient = 1.0f - distance_coefficient;
			distance_coefficient *= distance_coefficient;
			distance_coefficient = 1.0f - distance_coefficient;

			//Calculate the level of detail index.
			component->_LevelOfDetailIndices[j] = static_cast<uint32>(distance_coefficient * static_cast<float32>(component->_ModelResource->_Meshes[j]._MeshLevelOfDetails.Size() - 1));
		}
	}
}