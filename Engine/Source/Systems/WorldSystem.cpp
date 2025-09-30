//Header file.
#include <Systems/WorldSystem.h>

//Math.
#include <Math/Core/CatalystRandomMath.h>

//Profiling.
#include <Profiling/Profiling.h>

//Systems.
#include <Systems/CatalystEngineSystem.h>
#if !defined(CATALYST_CONFIGURATION_FINAL)
#include <Systems/DebugSystem.h>
#endif
#include <Systems/EntitySystem.h>
#include <Systems/RenderingSystem.h>

/*
*	Initializes the world system.
*/
void WorldSystem::Initialize() NOEXCEPT
{
	//Set the world grid size.
	_WorldGridSize = CatalystEngineSystem::Instance->GetProjectConfiguration()->_WorldConfiguration._WorldGridSize;

	//Initialize the water system.
	_WaterSystem.Initialize();

	//Initialize the wind system.
	_WindSystem.Initialize();

	//Register the uniform buffer.
	RenderingSystem::Instance->GetBufferManager()->RegisterUniformBuffer
	(
		HashString("World"),
		&_WorldUniformData,
		sizeof(WorldUniformData)
	);
}

/*
*	Post-initializes the world system.
*/
void WorldSystem::PostInitialize() NOEXCEPT
{
	//Post-initialize the sky system.
	_SkySystem.PostInitialize();

#if !defined(CATALYST_CONFIGURATION_FINAL)
	DebugSystem::Instance->RegisterSliderDebugCommand
	(
		"World\\Environment\\Volumetric Lighting\\Density",
		[](class DebugCommand *const RESTRICT debug_command, void *const RESTRICT user_data)
		{
			WorldSystem::Instance->GetEnvironmentSystem()->GetVolumetricLightingProperties()->_Density = debug_command->_State._SliderState._Value;
		},
		nullptr,
		0.0f,
		1.0f,
		_EnvironmentSystem.GetVolumetricLightingProperties()->_Density
	);

	DebugSystem::Instance->RegisterSliderDebugCommand
	(
		"World\\Wind\\Speed",
		[](class DebugCommand *const RESTRICT debug_command, void *const RESTRICT user_data)
		{
			WorldSystem::Instance->GetWindSystem()->SetWindSpeed(debug_command->_State._SliderState._Value);
		},
		nullptr,
		0.0f,
		4.0f,
		WorldSystem::Instance->GetWindSystem()->GetWindSpeed()
	);
#endif
}

/*
*	Updates the world system.
*/
void WorldSystem::Update(const UpdatePhase phase) NOEXCEPT
{
	//Update the time of day system.
	_TimeOfDaySystem.Update();
}

/*
*	Terminates the world system.
*/
void WorldSystem::Terminate() NOEXCEPT
{
	//Terminate the time of day system.
	_TimeOfDaySystem.Terminate();
}

/*
*	Updates the world system during render.
*/
void WorldSystem::RenderUpdate() NOEXCEPT
{
	//Update the world uniform data.
	_WorldUniformData._UpperSkyColor = _SkySystem.GetSkyGradient()._UpperSkyColor;
	_WorldUniformData._LowerSkyColor = _SkySystem.GetSkyGradient()._LowerSkyColor;
	_WorldUniformData._SkyMode = static_cast<uint32>(_SkySystem.GetSkyMode());
	_WorldUniformData._MaximumSkyTextureMipLevel = _SkySystem.GetSkyTexture() ? static_cast<float32>(_SkySystem.GetSkyTexture()->_MipLevels) : 0.0f;

	_WorldUniformData._VolumetricLightingDensity = BaseMath::Square(_EnvironmentSystem.GetVolumetricLightingProperties()->_Density) * 0.1f;
	_WorldUniformData._VolumetricLightingHeight = _EnvironmentSystem.GetVolumetricLightingProperties()->_Height;

	//Update the water system.
	_WaterSystem.RenderUpdate();

	//Update the wind system.
	_WindSystem.RenderUpdate();
}

/*
*	Returns the current world grid cell.
*/
NO_DISCARD const Vector3<int32> &WorldSystem::GetCurrentWorldGridCell() const NOEXCEPT
{
	//This should probably be cached somehow, but let's just ask the current camera for now. (:
	return RenderingSystem::Instance->GetCameraSystem()->GetCurrentCamera()->GetWorldTransform().GetCell();
}