//Header file.
#include <Rendering/Native/ShaderLightComponent.h>

//Core.
#include <Core/General/Perceiver.h>

/*
*	Copy by LightComponent constructor.
*/
ShaderLightComponent::ShaderLightComponent(const LightComponent& light_component) NOEXCEPT
{
	switch (light_component._LightType)
	{
		case LightType::DIRECTIONAL:
		{
			_Direction = light_component._Direction;

			break;
		}

		case LightType::POINT:
		{
			_WorldPosition = light_component._WorldPosition.GetRelativePosition(Perceiver::Instance->GetWorldTransform().GetCell());

			break;
		}
	}

	_Color = light_component._Color;
	_LightType = static_cast<uint32>(light_component._LightType);
	_LightProperties = light_component._LightProperties;
	_Intensity = light_component._Intensity;
	_Radius = light_component._Radius;
	_Size = light_component._Size;
}