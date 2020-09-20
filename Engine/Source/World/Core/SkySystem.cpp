//Header file.
#include <World/Core/SkySystem.h>

//Resources.
#include <Resources/Loading/TextureCubeData.h>

//Systems.
#include <Systems/RenderingSystem.h>
#include <Systems/ResourceSystem.h>
#include <Systems/WorldSystem.h>

//Sky system constants.
namespace SkySystemConstants
{
	constexpr uint8 SKY_GRADIENT_LOOKUP_SIZE{ 8 };
	constexpr StaticArray<SkyGradient, SKY_GRADIENT_LOOKUP_SIZE> SKY_GRADIENT_LOOKUP
	{
		//00.00.
		SkyGradient(Vector3<float32>(0.0f, 0.25f, 1.0f) * 0.125f * 0.125f * 0.5f, Vector3<float32>(0.0f, 0.25f, 1.0f) * 0.125f * 0.125f * 0.25f),

		//03.00.
		SkyGradient(Vector3<float32>(0.0f, 0.5f, 1.0f) * 0.125f * 0.25f, Vector3<float32>(1.0f, 1.0f, 0.75f) * 0.125f * 0.25f),

		//06.00.
		SkyGradient(Vector3<float32>(0.0f, 0.5f, 1.0f) * 0.25f, Vector3<float32>(1.0f, 0.5f, 0.25f) * 0.25f),

		//09.00.
		SkyGradient(Vector3<float32>(0.0f, 0.75f, 1.0f) * 0.5f, Vector3<float32>(1.0f, 0.75f, 0.5f) * 0.5f),

		//12.00.
		SkyGradient(Vector3<float32>(0.0f, 0.5f, 1.0f), Vector3<float32>(1.0f, 1.0f, 1.0f)),

		//15.00.
		SkyGradient(Vector3<float32>(0.0f, 0.5f, 1.0f), Vector3<float32>(1.0f, 0.75f, 0.5f)),

		//18.00.
		SkyGradient(Vector3<float32>(0.0f, 0.25f, 1.0f), Vector3<float32>(1.0f, 0.25f, 0.0625f)),

		//21.00.
		SkyGradient(Vector3<float32>(0.0f, 0.25f, 1.0f) * 0.125f * 0.5f, Vector3<float32>(0.25f, 0.5f, 0.75f) * 0.125f * 0.25f)
	};

	constexpr uint8 STAR_INTENSITY_LOOKUP_SIZE{ 8 };
	constexpr StaticArray<float32, STAR_INTENSITY_LOOKUP_SIZE> STAR_STRENGTH_LOOKUP
	{
		1.0f, //00.00
		0.5f, //03.00
		0.0f, //06.00
		0.0f, //09.00
		0.0f, //12.00
		0.0f, //15.00
		0.0f, //18.00
		0.5f  //32.00
	};
}

/*
*	Post-initializes the sky system.
*/
void SkySystem::PostInitialize() NOEXCEPT
{
	//Retrieve the default sky texture.
	_SkyTexture = ResourceSystem::Instance->GetTextureCubeResource(HashString("Default_Sky_TextureCube"));
}