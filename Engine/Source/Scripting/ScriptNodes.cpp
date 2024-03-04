//Header file.
#include <Scripting/ScriptNodes.h>

//Components.
#include <Components/Components/WorldTransformComponent.h>

//Math.
#include <Math/Core/CatalystBaseMath.h>

namespace Script
{
	namespace Math
	{
		NO_DISCARD float32 Lerp(ScriptContext &script_context, const float32 A, const float32 B, const float32 alpha) NOEXCEPT
		{
			return CatalystBaseMath::LinearlyInterpolate(A, B, alpha);
		}

		NO_DISCARD float32 Sine(ScriptContext &script_context, const float32 phase) NOEXCEPT
		{
			return CatalystBaseMath::Sine(phase);
		}
	}

	namespace WorldTransform
	{
		void SetRotation(ScriptContext &script_context, const EulerAngles &rotation) NOEXCEPT
		{
			ASSERT(WorldTransformComponent::Instance->Has(script_context._Entity), "Can't call WorldTransform::SetRotation on an entity with no world transform component!");
			
			WorldTransformComponent::Instance->InstanceData(script_context._Entity)._CurrentWorldTransform.SetRotation(rotation);
		}
	}
}