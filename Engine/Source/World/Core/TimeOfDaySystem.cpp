//Header file.
#include <World/Core/TimeOfDaySystem.h>

//Entities.
#include <Entities/Creation/LightInitializationData.h>

//Systems.
#include <Systems/EntityCreationSystem.h>

/*
*	Updates the time of day system during the pre update phase.
*/
void TimeOfDaySystem::PreUpdate(const UpdateContext* const RESTRICT context) NOEXCEPT
{
	if (!_Enabled)
	{
		return;
	}

	//Update the current time of day.
	_CurrentTimeOfDay += context->_DeltaTime / 60.0f / 60.0f * _TimeOfDayParameters._TimeMultiplier;

	//Wrap around.
	while (_CurrentTimeOfDay >= 24.0f)
	{
		_CurrentTimeOfDay -= 24.0f;
	}

	//Update the sky light.
	UpdateSkyLight();
}

/*
*	Enables the time of day system.
*/
void TimeOfDaySystem::Enable(const float time_of_day, const TimeOfDayParameters& time_of_day_parameters) NOEXCEPT
{
	//The time of day system is now enabled!
	_Enabled = true;

	//Set the current time of day.
	_CurrentTimeOfDay = time_of_day;

	//Set the time of day parameters.
	_TimeOfDayParameters = time_of_day_parameters;

	//Create the sky light.
	{
		_SkyLight = EntityCreationSystem::Instance->CreateEntity<LightEntity>();
		LightInitializationData* const RESTRICT data{ EntityCreationSystem::Instance->CreateInitializationData<LightInitializationData>() };

		data->_Properties = EntityInitializationData::Property::None;
		data->_LightType = LightType::DIRECTIONAL;
		data->_Direction = VectorConstants::DOWN;
		data->_Luminance = VectorConstants::ONE;
		data->_Size = 0.0f;

		EntityCreationSystem::Instance->RequestInitialization(_SkyLight, data, false);
	}
}

/*
*	Updates the sky light.
*/
void TimeOfDaySystem::UpdateSkyLight() NOEXCEPT
{
	//Define constants.
	constexpr float NIGHT_SKY_INTENSITY{ 2.5f };
	constexpr float DAY_SKY_INTENSITY{ 10.0f };
	constexpr Vector3<float> NIGHT_SKY_LUMINANCE{ 0.8f, 0.9f, 1.0f };
	constexpr Vector3<float> DAY_SKY_LUMINANCE{ 1.0f, 0.9f, 0.8f };

	//Need that sky light.
	if (!_SkyLight->_Initialized)
	{
		return;
	}

	//Calculate the time of day alpha.
	float time_of_day_alpha;

	if (_CurrentTimeOfDay >= 18.0f)
	{
		time_of_day_alpha = (_CurrentTimeOfDay - 18.0f) / 12.0f;
	}

	else if (_CurrentTimeOfDay < 6.0f)
	{
		time_of_day_alpha = (_CurrentTimeOfDay + 6.0f) / 12.0f;
	}

	else
	{
		time_of_day_alpha = (_CurrentTimeOfDay - 6.0f) / 12.0f;
	}

	//Calculate the sky light direction.
	Vector3<float> sky_light_direction;

	if (_CurrentTimeOfDay >= 18.0f || _CurrentTimeOfDay < 6.0f)
	{
		Vector3<float> rotation;

		rotation._X = CatalystBaseMath::LinearlyInterpolate(CatalystBaseMath::DegreesToRadians(-100.0f), CatalystBaseMath::DegreesToRadians(100.0f), time_of_day_alpha);
		rotation._Y = 0.0f;
		rotation._Z = CatalystBaseMath::DegreesToRadians(22.5f);

		sky_light_direction = Vector3<float>::Normalize(VectorConstants::DOWN.Rotated(rotation));
	}
	
	else
	{
		Vector3<float> rotation;

		rotation._X = CatalystBaseMath::DegreesToRadians(22.5f);
		rotation._Y = 0.0f;
		rotation._Z = CatalystBaseMath::LinearlyInterpolate(CatalystBaseMath::DegreesToRadians(-100.0f), CatalystBaseMath::DegreesToRadians(100.0f), time_of_day_alpha);

		sky_light_direction = Vector3<float>::Normalize(VectorConstants::DOWN.Rotated(rotation));
	}

	//Set the sky light direction.
	_SkyLight->SetDirection(sky_light_direction);

	//Update the sky light luminance.
	{
		//Calculate the sky luminance alpha.
		float sky_luminance_alpha;

		if (_CurrentTimeOfDay < 6.0f)
		{
			sky_luminance_alpha = CatalystBaseMath::SmoothStep<1>(1.0f - (_CurrentTimeOfDay / 6.0f));
		}

		else if (_CurrentTimeOfDay < 12.0f)
		{
			sky_luminance_alpha = CatalystBaseMath::SmoothStep<1>((_CurrentTimeOfDay - 6.0f) / 6.0f);
		}

		else if (_CurrentTimeOfDay < 18.0f)
		{
			sky_luminance_alpha = CatalystBaseMath::SmoothStep<1>(1.0f - ((_CurrentTimeOfDay - 12.0f) / 6.0f));
		}

		else
		{
			sky_luminance_alpha = CatalystBaseMath::SmoothStep<1>((_CurrentTimeOfDay - 18.0f) / 6.0f);
		}

		if (_CurrentTimeOfDay >= 18.0f || _CurrentTimeOfDay < 6.0f)
		{
			_SkyLight->SetLuminance(NIGHT_SKY_LUMINANCE * NIGHT_SKY_INTENSITY * sky_luminance_alpha);
		}

		else
		{
			_SkyLight->SetLuminance(DAY_SKY_LUMINANCE * DAY_SKY_INTENSITY * sky_luminance_alpha);
		}
	}
}