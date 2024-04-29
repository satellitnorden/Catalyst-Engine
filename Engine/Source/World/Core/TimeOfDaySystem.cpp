//Header file.
#include <World/Core/TimeOfDaySystem.h>

//Components.
#include <Components/Components/LightComponent.h>

//File.
#include <File/Core/File.h>
#include <File/Core/BinaryInputFile.h>
#include <File/Core/BinaryOutputFile.h>

//Math.
#include <Math/Core/CatalystCoordinateSpaces.h>

//Rendering.
#include <Rendering/Native/Color.h>

//Systems.
#if defined(CATALYST_EDITOR)
	#include <Systems/CatalystEditorSystem.h>
#endif
#include <Systems/CatalystEngineSystem.h>
#include <Systems/EntitySystem.h>
#include <Systems/WorldSystem.h>

/*
*	Terminates the time of day system.
*/
void TimeOfDaySystem::Terminate() NOEXCEPT
{
	//If the time of day system was enabled, save save data.
	if (_Enabled)
	{
		DynamicString time_of_day_save_data_file_path{ _TimeOfDayParameters._SaveFolder };
		time_of_day_save_data_file_path += "\\TimeOfDaySaveData.save";

		BinaryOutputFile time_of_day_save_data_file{ time_of_day_save_data_file_path.Data() };

		time_of_day_save_data_file.Write(&_CurrentTimeOfDay, sizeof(float32));

		time_of_day_save_data_file.Close();
	}
}

/*
*	Enables the time of day system.
*/
void TimeOfDaySystem::Enable(const TimeOfDayParameters& time_of_day_parameters) NOEXCEPT
{
	ASSERT(!_Enabled, "Enabling the time of day system twice!");

	//Try to load the time of day data, if it exists.
	bool loaded_save_data{ false };

	if (time_of_day_parameters._SaveFolder.Length() > 0)
	{
		DynamicString time_of_day_save_data_file_path{ time_of_day_parameters._SaveFolder };
		time_of_day_save_data_file_path += "\\TimeOfDaySaveData.save";

		if (File::Exists(time_of_day_save_data_file_path.Data()))
		{
			BinaryInputFile time_of_day_save_data_file{ time_of_day_save_data_file_path.Data() };

			time_of_day_save_data_file.Read(&_CurrentTimeOfDay, sizeof(float32));

			time_of_day_save_data_file.Close();

			loaded_save_data = true;
		}
	}

	//Set initial data, if save data wasn't loaded.
	if (!loaded_save_data)
	{
		_CurrentTimeOfDay = time_of_day_parameters._InitialTimeOfDay;
	}

	//Set the time of day parameters.
	_TimeOfDayParameters = time_of_day_parameters;

	//Create the sky light.
	{
		LightInitializationData *const RESTRICT data{ LightComponent::Instance->AllocateInitializationData() };

		data->_DirectionalLightData._Rotation = EulerAngles();
		data->_Color = VectorConstants::ONE;
		data->_LightType = LightType::DIRECTIONAL;
		data->_LightProperties = CatalystShaderConstants::LIGHT_PROPERTY_SURFACE_SHADOW_CASTING_BIT | CatalystShaderConstants::LIGHT_PROPERTY_VOLUMETRIC_BIT | CatalystShaderConstants::LIGHT_PROPERTY_VOLUMETRIC_SHADOW_CASTING_BIT;
		data->_Intensity = 1.0f;

		ComponentInitializationData *RESTRICT component_configuration{ data };
		_SkyLight = EntitySystem::Instance->CreateEntity(ArrayProxy<ComponentInitializationData *RESTRICT>(component_configuration));
	}

	//Initialize the sky light curve.
	InitializeSkyLightCurve();

	//Initialize the sky gradient curve.
	InitializeSkyGradientCurve();

	//Register the update.
	CatalystEngineSystem::Instance->RegisterUpdate([](void* const RESTRICT arguments)
	{
		static_cast<TimeOfDaySystem *const RESTRICT>(arguments)->PreUpdate();
	},
	this,
	UpdatePhase::PRE,
	UpdatePhase::RENDER,
	false,
	false);

	//The time of day system is now enabled!
	_Enabled = true;
}

/*
*	Initializes the sky light curve.
*/
void TimeOfDaySystem::InitializeSkyLightCurve() NOEXCEPT
{
	uint64 current_index{ 0 };

	#define ADD_CURVE_POINT(R, G, B, INTENSITY)												\
	{																						\
	_SkyLightCurve.SetValue(current_index++, Vector4<float32>(R, G, B, INTENSITY * 1.0f));	\
	}

	//00.00.
	ADD_CURVE_POINT(0.5f, 0.75f, 1.0f, 2.0f);
	//03.00.
	ADD_CURVE_POINT(0.5f, 0.75f, 1.0f, 1.5f);
	//06.00.
	ADD_CURVE_POINT(1.0f, 0.875f, 0.25f, 0.0f);
	//06.00.
	ADD_CURVE_POINT(1.0f, 0.875f, 0.375f, 12.0f);
	//12.00.
	ADD_CURVE_POINT(1.0f, 0.875f, 0.5f, 16.0f);
	//15.00.
	ADD_CURVE_POINT(1.0f, 0.875f, 0.375f, 12.0f);
	//18.00.
	ADD_CURVE_POINT(1.0f, 0.25f, 0.125f, 0.0f);
	//03.00.
	ADD_CURVE_POINT(0.5f, 0.75f, 1.0f, 1.5f);

	#undef ADD_CURVE_POINT
}

/*
*	Initializes the sky gradient curve.
*/
void TimeOfDaySystem::InitializeSkyGradientCurve() NOEXCEPT
{
	uint64 current_index{ 0 };

	#define ADD_CURVE_POINT(UPPER_R, UPPER_G, UPPER_B, LOWER_R, LOWER_G, LOWER_B, UPPER_MULTIPLIER, LOWER_MULTIPLIER)																									\
	{																																																					\
	Color upper{ static_cast<uint8>(UPPER_R), static_cast<uint8>(UPPER_G), static_cast<uint8>(UPPER_B) };																												\
	Color lower{ static_cast<uint8>(LOWER_R), static_cast<uint8>(LOWER_G), static_cast<uint8>(LOWER_B) };																												\
	upper.ApplyGammaCorrection();																																														\
	lower.ApplyGammaCorrection();																																														\
	const Vector4<float32> _upper{ upper.Get() };																																										\
	const Vector4<float32> _lower{ lower.Get() };																																										\
	_SkyGradientCurve.SetValue(current_index++, SkyGradient(Vector3<float32>(_upper._R, _upper._G, _upper._B) * UPPER_MULTIPLIER * 1.0f, Vector3<float32>(_lower._R, _lower._G, _lower._B) * LOWER_MULTIPLIER * 1.0f));	\
	}

	//00.00.
	ADD_CURVE_POINT(50, 100, 250, 50, 100, 250, 0.125f, 0.125f * 0.125f);
	//06.00.
	ADD_CURVE_POINT(100, 225, 250, 250, 225, 100, 1.0f, 1.0f);
	//12.00.
	ADD_CURVE_POINT(100, 225, 250, 125, 250, 225, 1.0f, 1.0f);
	//18.00.
	ADD_CURVE_POINT(75, 125, 250, 250, 100, 100, 1.0f, 1.0f);
	
	#undef ADD_CURVE_POINT
}

/*
*	Updates the time of day system during the pre update phase.
*/
void TimeOfDaySystem::PreUpdate() NOEXCEPT
{
	ASSERT(_Enabled, "The time of day system should not be updated if it's not enabled");

	//Cache the delta time.
#if defined(CATALYST_EDITOR)
	const float32 delta_time{ CatalystEditorSystem::Instance->IsInGame() ? CatalystEngineSystem::Instance->GetDeltaTime() : 0.0f };
#else
	const float32 delta_time{ CatalystEngineSystem::Instance->GetDeltaTime() };
#endif

	//Update the current time of day.
	_CurrentTimeOfDay += delta_time / 60.0f / 60.0f * _TimeOfDayParameters._TimeMultiplier;

	//Wrap around.
	while (_CurrentTimeOfDay < 0.0f)
	{
		_CurrentTimeOfDay += 24.0f;
	}

	while (_CurrentTimeOfDay >= 24.0f)
	{
		_CurrentTimeOfDay -= 24.0f;
	}

	//Update the sky light.
	UpdateSkyLight();

	//Update the sky.
	UpdateSky();
}

/*
*	Updates the sky light.
*/
void TimeOfDaySystem::UpdateSkyLight() NOEXCEPT
{
	//Define constants.
	constexpr float32 NIGHT_SKY_INTENSITY{ 8.0f };
	constexpr float32 DAY_SKY_INTENSITY{ 32.0f };

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
	EulerAngles rotation;

	if (_CurrentTimeOfDay >= 18.0f || _CurrentTimeOfDay < 6.0f)
	{
		rotation._Roll = BaseMath::LinearlyInterpolate(BaseMath::DegreesToRadians(-100.0f), BaseMath::DegreesToRadians(100.0f), time_of_day_alpha);
		rotation._Yaw = 0.0f;
		rotation._Pitch = BaseMath::DegreesToRadians(22.5f);
	}
	
	else
	{
		rotation._Roll = BaseMath::DegreesToRadians(22.5f);
		rotation._Yaw = 0.0f;
		rotation._Pitch = BaseMath::LinearlyInterpolate(BaseMath::DegreesToRadians(-100.0f), BaseMath::DegreesToRadians(100.0f), time_of_day_alpha);
	}

	//Cache the instance data.
	LightInstanceData &instance_data{ LightComponent::Instance->InstanceData(_SkyLight) };

	//Set the sky light rotation.
	instance_data._DirectionalLightData._Rotation = rotation;

	const Vector4<float32> current_sky_light_luminance{ _SkyLightCurve.Sample(_CurrentTimeOfDay / 24.0f) };

	instance_data._Color = Vector3<float32>(current_sky_light_luminance._R, current_sky_light_luminance._G, current_sky_light_luminance._B);
	instance_data._Intensity = current_sky_light_luminance._W;
}

/*
*	Updates the sky.
*/
void TimeOfDaySystem::UpdateSky() NOEXCEPT
{
	//If the time of day system shouldn't control the sky, just quit already.
	if (!_TimeOfDayParameters._ControlSky)
	{
		return;
	}

	//Set the sky intensity.
	WorldSystem::Instance->GetSkySystem()->SetSkyIntensity(1.0f);

	WorldSystem::Instance->GetSkySystem()->SetSkyGradient(_SkyGradientCurve.Sample(_CurrentTimeOfDay / 24.0f));
	//WorldSystem::Instance->GetSkySystem()->SetStarIntensity(BaseMath::LinearlyInterpolate(TimeOfDaySystemConstants::STAR_INTENSITY_LOOKUP[first_index], TimeOfDaySystemConstants::STAR_INTENSITY_LOOKUP[second_index], alpha));
} 