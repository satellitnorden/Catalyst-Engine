//Header file.
#include <Rendering/Native/ShaderLightComponent.h>

//Components.
#include <Components/Components/LightComponent.h>
#include <Components/Components/WorldTransformComponent.h>

//Math.
#include <Math/Core/CatalystCoordinateSpaces.h>

//Systems.
#include <Systems/RenderingSystem.h>

/*
*	Copy by LightComponent constructor.
*/
ShaderLightComponent::ShaderLightComponent(const Entity *const RESTRICT entity) NOEXCEPT
{
	const LightInstanceData &light_instance_data{ LightComponent::Instance->InstanceData(entity) };

	switch (light_instance_data._LightType)
	{
		case LightType::DIRECTIONAL:
		{
			_Direction = CatalystCoordinateSpacesUtilities::RotatedWorldDownVector(light_instance_data._DirectionalLightData._Rotation);

			break;
		}

		case LightType::POINT:
		{
			const WorldTransformInstanceData &world_transform_instance_data{ WorldTransformComponent::Instance->InstanceData(entity) };
			_WorldPosition = world_transform_instance_data._CurrentWorldTransform.GetRelativePosition(RenderingSystem::Instance->GetCameraSystem()->GetCurrentCamera()->GetWorldTransform().GetCell());

			break;
		}

		/*
		case LightType::BOX:
		{
			_MinimumWorldPosition = light_component._MinimumWorldPosition.GetRelativePosition(RenderingSystem::Instance->GetCameraSystem()->GetCurrentCamera()->GetWorldTransform().GetCell());
			_MaximumWorldPosition = light_component._MaximumWorldPosition.GetRelativePosition(RenderingSystem::Instance->GetCameraSystem()->GetCurrentCamera()->GetWorldTransform().GetCell());

			break;
		}
		*/
	}

	_Color = light_instance_data._Color;
	_LightType = static_cast<uint32>(light_instance_data._LightType);
	_LightProperties = light_instance_data._LightProperties;
	_Intensity = light_instance_data._Intensity;
	_Radius = light_instance_data._PointLightData._Radius;
	_Size = light_instance_data._PointLightData._Size;
}