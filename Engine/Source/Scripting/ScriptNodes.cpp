//Header file.
#include <Scripting/ScriptNodes.h>

//Components.
#include <Components/Components/ScriptComponent.h>
#include <Components/Components/StaticModelComponent.h>
#include <Components/Components/WorldTransformComponent.h>

//Math.
#include <Math/Core/CatalystBaseMath.h>

//Systems.
#include <Systems/ContentSystem.h>
#include <Systems/EntitySystem.h>

namespace Script
{
	namespace Math
	{
		NO_DISCARD float32 Distance(ScriptContext &script_context, const Vector3<float32> &A, const Vector3<float32> &B) NOEXCEPT
		{
			return Vector3<float32>::Length(A - B);
		}

		NO_DISCARD float32 Lerp(ScriptContext &script_context, const float32 A, const float32 B, const float32 alpha) NOEXCEPT
		{
			return CatalystBaseMath::LinearlyInterpolate(A, B, alpha);
		}

		NO_DISCARD float32 Sine(ScriptContext &script_context, const float32 phase) NOEXCEPT
		{
			return CatalystBaseMath::Sine(phase);
		}
	}

	namespace Script
	{

		/*
		*	Sends an event to linked entities.
		*/
		void Event(ScriptContext &script_context, const HashString event) NOEXCEPT
		{
			for (Entity *const RESTRICT linked_entity : EntitySystem::Instance->GetEntityLinks()->GetLinks(script_context._Entity))
			{
				if (ScriptComponent::Instance->Has(linked_entity))
				{
					ScriptComponent::Instance->Event(linked_entity, event);
				}
			}
		}

	}

	namespace StaticModel
	{
		void SetMaterial(ScriptContext &script_context, const uint8 mesh_index, const HashString material) NOEXCEPT
		{
			ASSERT(StaticModelComponent::Instance->Has(script_context._Entity), "Can't call StaticModel::SetMaterial on an entity without a static model component!");
		
			StaticModelInstanceData& instance_data{ StaticModelComponent::Instance->InstanceData(script_context._Entity) };

			instance_data._Materials[mesh_index] = ContentSystem::Instance->GetAsset<MaterialAsset>(material);
		}
	}

	namespace WorldTransform
	{
		NO_DISCARD Vector3<float32> GetPosition(ScriptContext &script_context) NOEXCEPT
		{
			ASSERT(WorldTransformComponent::Instance->Has(script_context._Entity), "Can't call WorldTransform::GetPosition on an entity with no world transform component!");

			return WorldTransformComponent::Instance->InstanceData(script_context._Entity)._CurrentWorldTransform.GetAbsolutePosition();
		}

		void SetRotation(ScriptContext &script_context, const EulerAngles &rotation) NOEXCEPT
		{
			ASSERT(WorldTransformComponent::Instance->Has(script_context._Entity), "Can't call WorldTransform::SetRotation on an entity with no world transform component!");
			
			WorldTransformComponent::Instance->InstanceData(script_context._Entity)._CurrentWorldTransform.SetRotation(rotation);
		}

		void SetScale(ScriptContext& script_context, const float32 scale) NOEXCEPT
		{
			ASSERT(WorldTransformComponent::Instance->Has(script_context._Entity), "Can't call WorldTransform::SetScale on an entity with no world transform component!");

			WorldTransformComponent::Instance->InstanceData(script_context._Entity)._CurrentWorldTransform.SetScale(scale);
		}
	}
}