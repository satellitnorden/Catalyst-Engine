//Header file.
#include <World/SkySystem.h>

//Resources.
#include <Resources/Data/TextureCubeData.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Sky system constants.
namespace SkySystemConstants
{
	constexpr uint32 SKY_TEXTURE_RESOLUTION{ 1024 };
}

/*
*	Post-initializes the sky system.
*/
void SkySystem::PostInitialize() NOEXCEPT
{
	//Initialize the sky texture.
	InitializeSkyTexture();
}

/*
*	Returns the sky texture resolution.
*/
NO_DISCARD uint32 SkySystem::GetSkyTextureResolution() const NOEXCEPT
{
	return SkySystemConstants::SKY_TEXTURE_RESOLUTION;
}

/*
*	Initializes the sky texture.
*/
void SkySystem::InitializeSkyTexture() NOEXCEPT
{
	if (!_SkyTextureInitialized)
	{
		//Create the sky texture.
		TextureCubeData data;

		data._Resolution = SkySystemConstants::SKY_TEXTURE_RESOLUTION;
		data._Data.UpsizeFast(SkySystemConstants::SKY_TEXTURE_RESOLUTION * SkySystemConstants::SKY_TEXTURE_RESOLUTION * 4 * 6);

		Memory::Set(data._Data.Data(), 0, sizeof(float) * SkySystemConstants::SKY_TEXTURE_RESOLUTION * SkySystemConstants::SKY_TEXTURE_RESOLUTION * 4 * 6);

		RenderingSystem::Instance->CreateTextureCube(data, &_SkyTexture);

		_SkyTextureInitialized = true;
	}
}