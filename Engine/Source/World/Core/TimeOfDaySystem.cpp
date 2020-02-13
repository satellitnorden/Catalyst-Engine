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
	//UpdateSkyLight();
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
		data->_Luminance = VectorConstants::ZERO;
		data->_Size = 0.0f;

		EntityCreationSystem::Instance->RequestInitialization(_SkyLight, data, false);
	}
}

/*
*	Updates the sky light.
*/
void TimeOfDaySystem::UpdateSkyLight() NOEXCEPT
{
	//Need that sky light.
	if (!_SkyLight->_Initialized)
	{
		return;
	}

	//Calculate the direction alpha.
	float direction_alpha;

	if (_CurrentTimeOfDay >= 18.0f)
	{
		direction_alpha = (_CurrentTimeOfDay - 18.0f) / 12.0f;
	}

	else if (_CurrentTimeOfDay < 6.0f)
	{
		direction_alpha = (_CurrentTimeOfDay + 6.0f) / 12.0f;
	}

	else
	{
		direction_alpha = (_CurrentTimeOfDay - 6.0f) / 12.0f;
	}

	//Calculate the sky light direction.
	Vector3<float> sky_light_direction;

	if (_CurrentTimeOfDay >= 18.0f || _CurrentTimeOfDay < 6.0f)
	{
		Vector3<float> rotation;

		rotation._X = CatalystBaseMath::DegreesToRadians(45.0f);
		rotation._Y = CatalystBaseMath::LinearlyInterpolate(CatalystBaseMath::DegreesToRadians(-90.0f), CatalystBaseMath::DegreesToRadians(90.0f), direction_alpha);
		rotation._Z = 0.0f;

		sky_light_direction = -VectorConstants::FORWARD.Rotated(rotation);
	}
	
	else
	{
		Vector3<float> rotation;

		rotation._X = CatalystBaseMath::DegreesToRadians(45.0f);
		rotation._Y = CatalystBaseMath::LinearlyInterpolate(CatalystBaseMath::DegreesToRadians(-90.0f), CatalystBaseMath::DegreesToRadians(90.0f), direction_alpha);
		rotation._Z = 0.0f;

		sky_light_direction = -VectorConstants::FORWARD.Rotated(rotation);
	}

	ASSERT(!CatalystBaseMath::IsNaN(sky_light_direction._X) && !CatalystBaseMath::IsNaN(sky_light_direction._Y) && !CatalystBaseMath::IsNaN(sky_light_direction._Z), "oh no");

	//Set the sky light direction.
	_SkyLight->SetDirection(sky_light_direction);
}