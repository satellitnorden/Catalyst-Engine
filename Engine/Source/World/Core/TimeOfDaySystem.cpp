//Header file.
#include <World/Core/TimeOfDaySystem.h>

//Entities.
#include <Entities/Creation/LightInitializationData.h>

//Systems.
#include <Systems/CatalystEngineSystem.h>
#include <Systems/EntitySystem.h>

/*
*	Enables the time of day system.
*/
void TimeOfDaySystem::Enable(const float32 time_of_day, const TimeOfDayParameters& time_of_day_parameters) NOEXCEPT
{
	ASSERT(!_Enabled, "Enabling the time of day system twice!");

	//The time of day system is now enabled!
	_Enabled = true;

	//Set the current time of day.
	_CurrentTimeOfDay = time_of_day;

	//Set the time of day parameters.
	_TimeOfDayParameters = time_of_day_parameters;

	//Create the sky light.
	{
		_SkyLight = EntitySystem::Instance->CreateEntity<LightEntity>();
		LightInitializationData* const RESTRICT data{ EntitySystem::Instance->CreateInitializationData<LightInitializationData>() };

		data->_Properties = EntityInitializationData::Property::NONE;
		data->_Direction = VectorConstants::DOWN;
		data->_Color = VectorConstants::ONE;
		data->_LightType = LightType::DIRECTIONAL;
		data->_LightProperties = CatalystShaderConstants::LIGHT_PROPERTY_SURFACE_SHADOW_CASTING_BIT | CatalystShaderConstants::LIGHT_PROPERTY_VOLUMETRIC_BIT | CatalystShaderConstants::LIGHT_PROPERTY_VOLUMETRIC_SHADOW_CASTING_BIT;
		data->_Intensity = 1.0f;
		data->_Radius = 0.0f;
		data->_Size = 0.0f;

		EntitySystem::Instance->RequestInitialization(_SkyLight, data, false);
	}

	//Register the update.
	CatalystEngineSystem::Instance->RegisterUpdate([](void* const RESTRICT arguments)
	{
		static_cast<TimeOfDaySystem *const RESTRICT>(arguments)->PreUpdate();
	},
	this,
	UpdatePhase::PRE,
	UpdatePhase::RENDER,
	false);
}

/*
*	Updates the time of day system during the pre update phase.
*/
void TimeOfDaySystem::PreUpdate() NOEXCEPT
{
	ASSERT(_Enabled, "The time of day system should not be updated if it's not enabled");

	//Cache the delta time.
	const float32 delta_time{ CatalystEngineSystem::Instance->GetDeltaTime() };

	//Update the current time of day.
	_CurrentTimeOfDay +=delta_time / 60.0f / 60.0f * _TimeOfDayParameters._TimeMultiplier;

	//Wrap around.
	while (_CurrentTimeOfDay >= 24.0f)
	{
		_CurrentTimeOfDay -= 24.0f;
	}

	//Update the sky light.
	UpdateSkyLight();
}

/*
*	Updates the sky light.
*/
void TimeOfDaySystem::UpdateSkyLight() NOEXCEPT
{
	//Define constants.
	constexpr float32 NIGHT_SKY_INTENSITY{ 5.0f };
	constexpr float32 DAY_SKY_INTENSITY{ 17.5f };
	constexpr Vector3<float32> NIGHT_SKY_LUMINANCE{ 0.8f, 0.9f, 1.0f };
	constexpr Vector3<float32> DAY_SKY_LUMINANCE{ 1.0f, 0.9f, 0.8f };

	//Need that sky light.
	if (!_SkyLight->_Initialized)
	{
		return;
	}

	//Calculate the time of day alpha.
	float32 time_of_day_alpha;

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
	Vector3<float32> sky_light_direction;

	if (_CurrentTimeOfDay >= 18.0f || _CurrentTimeOfDay < 6.0f)
	{
		Vector3<float32> rotation;

		rotation._X = CatalystBaseMath::LinearlyInterpolate(CatalystBaseMath::DegreesToRadians(-100.0f), CatalystBaseMath::DegreesToRadians(100.0f), time_of_day_alpha);
		rotation._Y = 0.0f;
		rotation._Z = CatalystBaseMath::DegreesToRadians(22.5f);

		sky_light_direction = Vector3<float32>::Normalize(VectorConstants::DOWN.Rotated(rotation));
	}
	
	else
	{
		Vector3<float32> rotation;

		rotation._X = CatalystBaseMath::DegreesToRadians(22.5f);
		rotation._Y = 0.0f;
		rotation._Z = CatalystBaseMath::LinearlyInterpolate(CatalystBaseMath::DegreesToRadians(-100.0f), CatalystBaseMath::DegreesToRadians(100.0f), time_of_day_alpha);

		sky_light_direction = Vector3<float32>::Normalize(VectorConstants::DOWN.Rotated(rotation));
	}

	//Set the sky light direction.
	_SkyLight->SetDirection(sky_light_direction);

	//Update the sky light luminance.
	{
		//Calculate the sky luminance alpha.
		float32 sky_luminance_alpha;

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
			_SkyLight->SetColor(NIGHT_SKY_LUMINANCE);
			_SkyLight->SetIntensity(NIGHT_SKY_INTENSITY * sky_luminance_alpha);
		}

		else
		{
			_SkyLight->SetColor(DAY_SKY_LUMINANCE);
			_SkyLight->SetIntensity(DAY_SKY_INTENSITY * sky_luminance_alpha);
		}
	}
}