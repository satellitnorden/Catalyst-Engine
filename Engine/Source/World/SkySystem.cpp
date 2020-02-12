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
	//Initialize the sky textures.
	InitializeSkyTextures();
}

/*
*	Returns the sky texture resolution.
*/
NO_DISCARD uint32 SkySystem::GetSkyTextureResolution() const NOEXCEPT
{
	return SkySystemConstants::SKY_TEXTURE_RESOLUTION;
}

/*
*	Initializes the sky textures.
*/
void SkySystem::InitializeSkyTextures() NOEXCEPT
{
	if (!_SkyTexturesInitialized)
	{
		for (uint32 i{ 0 }; i < CatalystShaderConstants::NUMBER_OF_SKY_TEXTURES; ++i)
		{
			//Calculate the resolution.
			const uint32 resolution{ SkySystemConstants::SKY_TEXTURE_RESOLUTION >> i };

			//Create the sky texture.
			TextureCubeData data;

			data._Resolution = resolution;
			data._Data.UpsizeFast(resolution * resolution * 4 * 6);

			Memory::Set(data._Data.Data(), 0, sizeof(float) * resolution * resolution * 4 * 6);

			RenderingSystem::Instance->CreateTextureCube(data, &_SkyTextures[i]);
		}

		_SkyTexturesInitialized = true;
	}
}