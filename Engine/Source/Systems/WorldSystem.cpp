//Header file.
#include <Systems/WorldSystem.h>

//Math.
#include <Math/Core/CatalystRandomMath.h>

//Profiling.
#include <Profiling/Profiling.h>

//Resources.
#include <Resources/Core/LevelResource.h>

//Systems.
#include <Systems/CatalystEngineSystem.h>
#include <Systems/EntitySystem.h>
#include <Systems/RenderingSystem.h>
#include <Systems/ResourceSystem.h>

//Singleton definition.
DEFINE_SINGLETON(WorldSystem);

/*
*	Initializes the world system.
*/
void WorldSystem::Initialize(const CatalystProjectWorldConfiguration &configuration) NOEXCEPT
{
	//Set the world grid size.
	_WorldGridSize = configuration._WorldGridSize;

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
	_WorldUniformData._MaximumSkyTextureMipLevel = _SkySystem.GetSkyTexture() ? static_cast<float32>(_SkySystem.GetSkyTexture()->_MipmapLevels) : 0.0f;

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