#if defined(CATALYST_ENABLE_RESOURCE_BUILDING)
//Header file.
#include <Resources/Building/CatalystEngineResourceBuilding.h>

//Core.
#include <Core/Containers/StaticArray.h>
#include <Core/General/DynamicString.h>
#include <Core/General/HashString.h>

//File.
#include <File/Core/BinaryFile.h>

//Math.
#include <Math/Core/CatalystRandomMath.h>

//Rendering.
#include <Rendering/Native/Texture2D.h>

//Resources.
#include <Resources/Core/ResourcesCore.h>
#include <Resources/Building/ResourceBuilder.h>

#define BUILD_ENGINE_CLOUD_TEXTURE false
#define BUILD_ENGINE_FONTS false
#define BUILD_ENGINE_OCEAN_TEXTURE false
#define BUILD_ENGINE_NOISE_TEXTURES false
#define BUILD_ENGINE_COLOR_GRADING_TEXTURES false

/*
*	Builds resources for the Catalyst Engine.
*/
void CatalystEngineResourceBuilding::BuildResources() NOEXCEPT
{

#if BUILD_ENGINE_CLOUD_TEXTURE
	BuildCloudTexture();
#endif

#if BUILD_ENGINE_FONTS
	{
		FontBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Catalyst_Engine_Default_Font";
		parameters._ID = "Catalyst_Engine_Default_Font";
		parameters._File = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Fonts\\Timeless.ttf";
		parameters._MaximumFontResolution = 128;
		parameters._MipmapLevels = 1;

		ResourceBuilder::BuildFont(parameters);
	}
#endif

#if BUILD_ENGINE_OCEAN_TEXTURE
	BuildOceanTexture();
#endif

#if BUILD_ENGINE_NOISE_TEXTURES
	{
		Texture2DBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_0_Texture2D";
		parameters._ID = "Blue_Noise_0_Texture2D";
		parameters._MipmapLevels = 1;
		parameters._Mode = Texture2DBuildParameters::Mode::RToRGBA;
		parameters._FileR = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Textures\\Blue_Noise_Textures\\HDR_RGBA_0.png";
		parameters._FileG = nullptr;
		parameters._FileB = nullptr;
		parameters._FileA = nullptr;
		parameters._ApplyGammaCorrection = false;

		ResourceBuilder::BuildTexture2D(parameters);
	}

	{
		Texture2DBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_1_Texture2D";
		parameters._ID = "Blue_Noise_1_Texture2D";
		parameters._MipmapLevels = 1;
		parameters._Mode = Texture2DBuildParameters::Mode::RToRGBA;
		parameters._FileR = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Textures\\Blue_Noise_Textures\\HDR_RGBA_1.png";
		parameters._FileG = nullptr;
		parameters._FileB = nullptr;
		parameters._FileA = nullptr;
		parameters._ApplyGammaCorrection = false;

		ResourceBuilder::BuildTexture2D(parameters);
	}

	{
		Texture2DBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_2_Texture2D";
		parameters._ID = "Blue_Noise_2_Texture2D";
		parameters._MipmapLevels = 1;
		parameters._Mode = Texture2DBuildParameters::Mode::RToRGBA;
		parameters._FileR = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Textures\\Blue_Noise_Textures\\HDR_RGBA_2.png";
		parameters._FileG = nullptr;
		parameters._FileB = nullptr;
		parameters._FileA = nullptr;
		parameters._ApplyGammaCorrection = false;

		ResourceBuilder::BuildTexture2D(parameters);
	}

	{
		Texture2DBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_3_Texture2D";
		parameters._ID = "Blue_Noise_3_Texture2D";
		parameters._MipmapLevels = 1;
		parameters._Mode = Texture2DBuildParameters::Mode::RToRGBA;
		parameters._FileR = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Textures\\Blue_Noise_Textures\\HDR_RGBA_3.png";
		parameters._FileG = nullptr;
		parameters._FileB = nullptr;
		parameters._FileA = nullptr;
		parameters._ApplyGammaCorrection = false;

		ResourceBuilder::BuildTexture2D(parameters);
	}

	{
		Texture2DBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_4_Texture2D";
		parameters._ID = "Blue_Noise_4_Texture2D";
		parameters._MipmapLevels = 1;
		parameters._Mode = Texture2DBuildParameters::Mode::RToRGBA;
		parameters._FileR = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Textures\\Blue_Noise_Textures\\HDR_RGBA_4.png";
		parameters._FileG = nullptr;
		parameters._FileB = nullptr;
		parameters._FileA = nullptr;
		parameters._ApplyGammaCorrection = false;

		ResourceBuilder::BuildTexture2D(parameters);
	}

	{
		Texture2DBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_5_Texture2D";
		parameters._ID = "Blue_Noise_5_Texture2D";
		parameters._MipmapLevels = 1;
		parameters._Mode = Texture2DBuildParameters::Mode::RToRGBA;
		parameters._FileR = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Textures\\Blue_Noise_Textures\\HDR_RGBA_5.png";
		parameters._FileG = nullptr;
		parameters._FileB = nullptr;
		parameters._FileA = nullptr;
		parameters._ApplyGammaCorrection = false;

		ResourceBuilder::BuildTexture2D(parameters);
	}

	{
		Texture2DBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_6_Texture2D";
		parameters._ID = "Blue_Noise_6_Texture2D";
		parameters._MipmapLevels = 1;
		parameters._Mode = Texture2DBuildParameters::Mode::RToRGBA;
		parameters._FileR = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Textures\\Blue_Noise_Textures\\HDR_RGBA_6.png";
		parameters._FileG = nullptr;
		parameters._FileB = nullptr;
		parameters._FileA = nullptr;
		parameters._ApplyGammaCorrection = false;

		ResourceBuilder::BuildTexture2D(parameters);
	}

	{
		Texture2DBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_7_Texture2D";
		parameters._ID = "Blue_Noise_7_Texture2D";
		parameters._MipmapLevels = 1;
		parameters._Mode = Texture2DBuildParameters::Mode::RToRGBA;
		parameters._FileR = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Textures\\Blue_Noise_Textures\\HDR_RGBA_7.png";
		parameters._FileG = nullptr;
		parameters._FileB = nullptr;
		parameters._FileA = nullptr;
		parameters._ApplyGammaCorrection = false;

		ResourceBuilder::BuildTexture2D(parameters);
	}

	{
		Texture2DBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_8_Texture2D";
		parameters._ID = "Blue_Noise_8_Texture2D";
		parameters._MipmapLevels = 1;
		parameters._Mode = Texture2DBuildParameters::Mode::RToRGBA;
		parameters._FileR = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Textures\\Blue_Noise_Textures\\HDR_RGBA_8.png";
		parameters._FileG = nullptr;
		parameters._FileB = nullptr;
		parameters._FileA = nullptr;
		parameters._ApplyGammaCorrection = false;

		ResourceBuilder::BuildTexture2D(parameters);
	}

	{
		Texture2DBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_9_Texture2D";
		parameters._ID = "Blue_Noise_9_Texture2D";
		parameters._MipmapLevels = 1;
		parameters._Mode = Texture2DBuildParameters::Mode::RToRGBA;
		parameters._FileR = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Textures\\Blue_Noise_Textures\\HDR_RGBA_9.png";
		parameters._FileG = nullptr;
		parameters._FileB = nullptr;
		parameters._FileA = nullptr;
		parameters._ApplyGammaCorrection = false;

		ResourceBuilder::BuildTexture2D(parameters);
	}

	{
		Texture2DBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_10_Texture2D";
		parameters._ID = "Blue_Noise_10_Texture2D";
		parameters._MipmapLevels = 1;
		parameters._Mode = Texture2DBuildParameters::Mode::RToRGBA;
		parameters._FileR = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Textures\\Blue_Noise_Textures\\HDR_RGBA_10.png";
		parameters._FileG = nullptr;
		parameters._FileB = nullptr;
		parameters._FileA = nullptr;
		parameters._ApplyGammaCorrection = false;

		ResourceBuilder::BuildTexture2D(parameters);
	}

	{
		Texture2DBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_11_Texture2D";
		parameters._ID = "Blue_Noise_11_Texture2D";
		parameters._MipmapLevels = 1;
		parameters._Mode = Texture2DBuildParameters::Mode::RToRGBA;
		parameters._FileR = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Textures\\Blue_Noise_Textures\\HDR_RGBA_11.png";
		parameters._FileG = nullptr;
		parameters._FileB = nullptr;
		parameters._FileA = nullptr;
		parameters._ApplyGammaCorrection = false;

		ResourceBuilder::BuildTexture2D(parameters);
	}

	{
		Texture2DBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_12_Texture2D";
		parameters._ID = "Blue_Noise_12_Texture2D";
		parameters._MipmapLevels = 1;
		parameters._Mode = Texture2DBuildParameters::Mode::RToRGBA;
		parameters._FileR = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Textures\\Blue_Noise_Textures\\HDR_RGBA_12.png";
		parameters._FileG = nullptr;
		parameters._FileB = nullptr;
		parameters._FileA = nullptr;
		parameters._ApplyGammaCorrection = false;

		ResourceBuilder::BuildTexture2D(parameters);
	}

	{
		Texture2DBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_13_Texture2D";
		parameters._ID = "Blue_Noise_13_Texture2D";
		parameters._MipmapLevels = 1;
		parameters._Mode = Texture2DBuildParameters::Mode::RToRGBA;
		parameters._FileR = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Textures\\Blue_Noise_Textures\\HDR_RGBA_13.png";
		parameters._FileG = nullptr;
		parameters._FileB = nullptr;
		parameters._FileA = nullptr;
		parameters._ApplyGammaCorrection = false;

		ResourceBuilder::BuildTexture2D(parameters);
	}

	{
		Texture2DBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_14_Texture2D";
		parameters._ID = "Blue_Noise_14_Texture2D";
		parameters._MipmapLevels = 1;
		parameters._Mode = Texture2DBuildParameters::Mode::RToRGBA;
		parameters._FileR = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Textures\\Blue_Noise_Textures\\HDR_RGBA_14.png";
		parameters._FileG = nullptr;
		parameters._FileB = nullptr;
		parameters._FileA = nullptr;
		parameters._ApplyGammaCorrection = false;

		ResourceBuilder::BuildTexture2D(parameters);
	}

	{
		Texture2DBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_15_Texture2D";
		parameters._ID = "Blue_Noise_15_Texture2D";
		parameters._MipmapLevels = 1;
		parameters._Mode = Texture2DBuildParameters::Mode::RToRGBA;
		parameters._FileR = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Textures\\Blue_Noise_Textures\\HDR_RGBA_15.png";
		parameters._FileG = nullptr;
		parameters._FileB = nullptr;
		parameters._FileA = nullptr;
		parameters._ApplyGammaCorrection = false;

		ResourceBuilder::BuildTexture2D(parameters);
	}

	{
		Texture2DBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_16_Texture2D";
		parameters._ID = "Blue_Noise_16_Texture2D";
		parameters._MipmapLevels = 1;
		parameters._Mode = Texture2DBuildParameters::Mode::RToRGBA;
		parameters._FileR = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Textures\\Blue_Noise_Textures\\HDR_RGBA_16.png";
		parameters._FileG = nullptr;
		parameters._FileB = nullptr;
		parameters._FileA = nullptr;
		parameters._ApplyGammaCorrection = false;

		ResourceBuilder::BuildTexture2D(parameters);
	}

	{
		Texture2DBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_17_Texture2D";
		parameters._ID = "Blue_Noise_17_Texture2D";
		parameters._MipmapLevels = 1;
		parameters._Mode = Texture2DBuildParameters::Mode::RToRGBA;
		parameters._FileR = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Textures\\Blue_Noise_Textures\\HDR_RGBA_17.png";
		parameters._FileG = nullptr;
		parameters._FileB = nullptr;
		parameters._FileA = nullptr;
		parameters._ApplyGammaCorrection = false;

		ResourceBuilder::BuildTexture2D(parameters);
	}

	{
		Texture2DBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_18_Texture2D";
		parameters._ID = "Blue_Noise_18_Texture2D";
		parameters._MipmapLevels = 1;
		parameters._Mode = Texture2DBuildParameters::Mode::RToRGBA;
		parameters._FileR = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Textures\\Blue_Noise_Textures\\HDR_RGBA_18.png";
		parameters._FileG = nullptr;
		parameters._FileB = nullptr;
		parameters._FileA = nullptr;
		parameters._ApplyGammaCorrection = false;

		ResourceBuilder::BuildTexture2D(parameters);
	}

	{
		Texture2DBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_19_Texture2D";
		parameters._ID = "Blue_Noise_19_Texture2D";
		parameters._MipmapLevels = 1;
		parameters._Mode = Texture2DBuildParameters::Mode::RToRGBA;
		parameters._FileR = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Textures\\Blue_Noise_Textures\\HDR_RGBA_19.png";
		parameters._FileG = nullptr;
		parameters._FileB = nullptr;
		parameters._FileA = nullptr;
		parameters._ApplyGammaCorrection = false;

		ResourceBuilder::BuildTexture2D(parameters);
	}

	{
		Texture2DBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_20_Texture2D";
		parameters._ID = "Blue_Noise_20_Texture2D";
		parameters._MipmapLevels = 1;
		parameters._Mode = Texture2DBuildParameters::Mode::RToRGBA;
		parameters._FileR = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Textures\\Blue_Noise_Textures\\HDR_RGBA_0.png";
		parameters._FileG = nullptr;
		parameters._FileB = nullptr;
		parameters._FileA = nullptr;
		parameters._ApplyGammaCorrection = false;

		ResourceBuilder::BuildTexture2D(parameters);
	}

	{
		Texture2DBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_21_Texture2D";
		parameters._ID = "Blue_Noise_21_Texture2D";
		parameters._MipmapLevels = 1;
		parameters._Mode = Texture2DBuildParameters::Mode::RToRGBA;
		parameters._FileR = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Textures\\Blue_Noise_Textures\\HDR_RGBA_21.png";
		parameters._FileG = nullptr;
		parameters._FileB = nullptr;
		parameters._FileA = nullptr;
		parameters._ApplyGammaCorrection = false;

		ResourceBuilder::BuildTexture2D(parameters);
	}

	{
		Texture2DBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_22_Texture2D";
		parameters._ID = "Blue_Noise_22_Texture2D";
		parameters._MipmapLevels = 1;
		parameters._Mode = Texture2DBuildParameters::Mode::RToRGBA;
		parameters._FileR = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Textures\\Blue_Noise_Textures\\HDR_RGBA_22.png";
		parameters._FileG = nullptr;
		parameters._FileB = nullptr;
		parameters._FileA = nullptr;
		parameters._ApplyGammaCorrection = false;

		ResourceBuilder::BuildTexture2D(parameters);
	}

	{
		Texture2DBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_23_Texture2D";
		parameters._ID = "Blue_Noise_23_Texture2D";
		parameters._MipmapLevels = 1;
		parameters._Mode = Texture2DBuildParameters::Mode::RToRGBA;
		parameters._FileR = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Textures\\Blue_Noise_Textures\\HDR_RGBA_23.png";
		parameters._FileG = nullptr;
		parameters._FileB = nullptr;
		parameters._FileA = nullptr;
		parameters._ApplyGammaCorrection = false;

		ResourceBuilder::BuildTexture2D(parameters);
	}

	{
		Texture2DBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_24_Texture2D";
		parameters._ID = "Blue_Noise_24_Texture2D";
		parameters._MipmapLevels = 1;
		parameters._Mode = Texture2DBuildParameters::Mode::RToRGBA;
		parameters._FileR = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Textures\\Blue_Noise_Textures\\HDR_RGBA_24.png";
		parameters._FileG = nullptr;
		parameters._FileB = nullptr;
		parameters._FileA = nullptr;
		parameters._ApplyGammaCorrection = false;

		ResourceBuilder::BuildTexture2D(parameters);
	}

	{
		Texture2DBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_25_Texture2D";
		parameters._ID = "Blue_Noise_25_Texture2D";
		parameters._MipmapLevels = 1;
		parameters._Mode = Texture2DBuildParameters::Mode::RToRGBA;
		parameters._FileR = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Textures\\Blue_Noise_Textures\\HDR_RGBA_25.png";
		parameters._FileG = nullptr;
		parameters._FileB = nullptr;
		parameters._FileA = nullptr;
		parameters._ApplyGammaCorrection = false;

		ResourceBuilder::BuildTexture2D(parameters);
	}

	{
		Texture2DBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_26_Texture2D";
		parameters._ID = "Blue_Noise_26_Texture2D";
		parameters._MipmapLevels = 1;
		parameters._Mode = Texture2DBuildParameters::Mode::RToRGBA;
		parameters._FileR = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Textures\\Blue_Noise_Textures\\HDR_RGBA_26.png";
		parameters._FileG = nullptr;
		parameters._FileB = nullptr;
		parameters._FileA = nullptr;
		parameters._ApplyGammaCorrection = false;

		ResourceBuilder::BuildTexture2D(parameters);
	}

	{
		Texture2DBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_27_Texture2D";
		parameters._ID = "Blue_Noise_27_Texture2D";
		parameters._MipmapLevels = 1;
		parameters._Mode = Texture2DBuildParameters::Mode::RToRGBA;
		parameters._FileR = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Textures\\Blue_Noise_Textures\\HDR_RGBA_27.png";
		parameters._FileG = nullptr;
		parameters._FileB = nullptr;
		parameters._FileA = nullptr;
		parameters._ApplyGammaCorrection = false;

		ResourceBuilder::BuildTexture2D(parameters);
	}

	{
		Texture2DBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_28_Texture2D";
		parameters._ID = "Blue_Noise_28_Texture2D";
		parameters._MipmapLevels = 1;
		parameters._Mode = Texture2DBuildParameters::Mode::RToRGBA;
		parameters._FileR = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Textures\\Blue_Noise_Textures\\HDR_RGBA_28.png";
		parameters._FileG = nullptr;
		parameters._FileB = nullptr;
		parameters._FileA = nullptr;
		parameters._ApplyGammaCorrection = false;

		ResourceBuilder::BuildTexture2D(parameters);
	}

	{
		Texture2DBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_29_Texture2D";
		parameters._ID = "Blue_Noise_29_Texture2D";
		parameters._MipmapLevels = 1;
		parameters._Mode = Texture2DBuildParameters::Mode::RToRGBA;
		parameters._FileR = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Textures\\Blue_Noise_Textures\\HDR_RGBA_29.png";
		parameters._FileG = nullptr;
		parameters._FileB = nullptr;
		parameters._FileA = nullptr;
		parameters._ApplyGammaCorrection = false;

		ResourceBuilder::BuildTexture2D(parameters);
	}

	{
		Texture2DBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_30_Texture2D";
		parameters._ID = "Blue_Noise_30_Texture2D";
		parameters._MipmapLevels = 1;
		parameters._Mode = Texture2DBuildParameters::Mode::RToRGBA;
		parameters._FileR = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Textures\\Blue_Noise_Textures\\HDR_RGBA_30.png";
		parameters._FileG = nullptr;
		parameters._FileB = nullptr;
		parameters._FileA = nullptr;
		parameters._ApplyGammaCorrection = false;

		ResourceBuilder::BuildTexture2D(parameters);
	}

	{
		Texture2DBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_31_Texture2D";
		parameters._ID = "Blue_Noise_31_Texture2D";
		parameters._MipmapLevels = 1;
		parameters._Mode = Texture2DBuildParameters::Mode::RToRGBA;
		parameters._FileR = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Textures\\Blue_Noise_Textures\\HDR_RGBA_31.png";
		parameters._FileG = nullptr;
		parameters._FileB = nullptr;
		parameters._FileA = nullptr;
		parameters._ApplyGammaCorrection = false;

		ResourceBuilder::BuildTexture2D(parameters);
	}

	{
		Texture2DBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_32_Texture2D";
		parameters._ID = "Blue_Noise_32_Texture2D";
		parameters._MipmapLevels = 1;
		parameters._Mode = Texture2DBuildParameters::Mode::RToRGBA;
		parameters._FileR = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Textures\\Blue_Noise_Textures\\HDR_RGBA_32.png";
		parameters._FileG = nullptr;
		parameters._FileB = nullptr;
		parameters._FileA = nullptr;
		parameters._ApplyGammaCorrection = false;

		ResourceBuilder::BuildTexture2D(parameters);
	}

	{
		Texture2DBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_33_Texture2D";
		parameters._ID = "Blue_Noise_33_Texture2D";
		parameters._MipmapLevels = 1;
		parameters._Mode = Texture2DBuildParameters::Mode::RToRGBA;
		parameters._FileR = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Textures\\Blue_Noise_Textures\\HDR_RGBA_33.png";
		parameters._FileG = nullptr;
		parameters._FileB = nullptr;
		parameters._FileA = nullptr;
		parameters._ApplyGammaCorrection = false;

		ResourceBuilder::BuildTexture2D(parameters);
	}

	{
		Texture2DBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_34_Texture2D";
		parameters._ID = "Blue_Noise_34_Texture2D";
		parameters._MipmapLevels = 1;
		parameters._Mode = Texture2DBuildParameters::Mode::RToRGBA;
		parameters._FileR = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Textures\\Blue_Noise_Textures\\HDR_RGBA_34.png";
		parameters._FileG = nullptr;
		parameters._FileB = nullptr;
		parameters._FileA = nullptr;
		parameters._ApplyGammaCorrection = false;

		ResourceBuilder::BuildTexture2D(parameters);
	}

	{
		Texture2DBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_35_Texture2D";
		parameters._ID = "Blue_Noise_35_Texture2D";
		parameters._MipmapLevels = 1;
		parameters._Mode = Texture2DBuildParameters::Mode::RToRGBA;
		parameters._FileR = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Textures\\Blue_Noise_Textures\\HDR_RGBA_35.png";
		parameters._FileG = nullptr;
		parameters._FileB = nullptr;
		parameters._FileA = nullptr;
		parameters._ApplyGammaCorrection = false;

		ResourceBuilder::BuildTexture2D(parameters);
	}

	{
		Texture2DBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_36_Texture2D";
		parameters._ID = "Blue_Noise_36_Texture2D";
		parameters._MipmapLevels = 1;
		parameters._Mode = Texture2DBuildParameters::Mode::RToRGBA;
		parameters._FileR = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Textures\\Blue_Noise_Textures\\HDR_RGBA_36.png";
		parameters._FileG = nullptr;
		parameters._FileB = nullptr;
		parameters._FileA = nullptr;
		parameters._ApplyGammaCorrection = false;

		ResourceBuilder::BuildTexture2D(parameters);
	}

	{
		Texture2DBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_37_Texture2D";
		parameters._ID = "Blue_Noise_37_Texture2D";
		parameters._MipmapLevels = 1;
		parameters._Mode = Texture2DBuildParameters::Mode::RToRGBA;
		parameters._FileR = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Textures\\Blue_Noise_Textures\\HDR_RGBA_37.png";
		parameters._FileG = nullptr;
		parameters._FileB = nullptr;
		parameters._FileA = nullptr;
		parameters._ApplyGammaCorrection = false;

		ResourceBuilder::BuildTexture2D(parameters);
	}

	{
		Texture2DBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_38_Texture2D";
		parameters._ID = "Blue_Noise_38_Texture2D";
		parameters._MipmapLevels = 1;
		parameters._Mode = Texture2DBuildParameters::Mode::RToRGBA;
		parameters._FileR = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Textures\\Blue_Noise_Textures\\HDR_RGBA_38.png";
		parameters._FileG = nullptr;
		parameters._FileB = nullptr;
		parameters._FileA = nullptr;
		parameters._ApplyGammaCorrection = false;

		ResourceBuilder::BuildTexture2D(parameters);
	}

	{
		Texture2DBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_39_Texture2D";
		parameters._ID = "Blue_Noise_39_Texture2D";
		parameters._MipmapLevels = 1;
		parameters._Mode = Texture2DBuildParameters::Mode::RToRGBA;
		parameters._FileR = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Textures\\Blue_Noise_Textures\\HDR_RGBA_39.png";
		parameters._FileG = nullptr;
		parameters._FileB = nullptr;
		parameters._FileA = nullptr;
		parameters._ApplyGammaCorrection = false;

		ResourceBuilder::BuildTexture2D(parameters);
	}

	{
		Texture2DBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_40_Texture2D";
		parameters._ID = "Blue_Noise_40_Texture2D";
		parameters._MipmapLevels = 1;
		parameters._Mode = Texture2DBuildParameters::Mode::RToRGBA;
		parameters._FileR = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Textures\\Blue_Noise_Textures\\HDR_RGBA_40.png";
		parameters._FileG = nullptr;
		parameters._FileB = nullptr;
		parameters._FileA = nullptr;
		parameters._ApplyGammaCorrection = false;

		ResourceBuilder::BuildTexture2D(parameters);
	}

	{
		Texture2DBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_41_Texture2D";
		parameters._ID = "Blue_Noise_41_Texture2D";
		parameters._MipmapLevels = 1;
		parameters._Mode = Texture2DBuildParameters::Mode::RToRGBA;
		parameters._FileR = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Textures\\Blue_Noise_Textures\\HDR_RGBA_41.png";
		parameters._FileG = nullptr;
		parameters._FileB = nullptr;
		parameters._FileA = nullptr;
		parameters._ApplyGammaCorrection = false;

		ResourceBuilder::BuildTexture2D(parameters);
	}

	{
		Texture2DBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_42_Texture2D";
		parameters._ID = "Blue_Noise_42_Texture2D";
		parameters._MipmapLevels = 1;
		parameters._Mode = Texture2DBuildParameters::Mode::RToRGBA;
		parameters._FileR = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Textures\\Blue_Noise_Textures\\HDR_RGBA_42.png";
		parameters._FileG = nullptr;
		parameters._FileB = nullptr;
		parameters._FileA = nullptr;
		parameters._ApplyGammaCorrection = false;

		ResourceBuilder::BuildTexture2D(parameters);
	}

	{
		Texture2DBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_43_Texture2D";
		parameters._ID = "Blue_Noise_43_Texture2D";
		parameters._MipmapLevels = 1;
		parameters._Mode = Texture2DBuildParameters::Mode::RToRGBA;
		parameters._FileR = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Textures\\Blue_Noise_Textures\\HDR_RGBA_43.png";
		parameters._FileG = nullptr;
		parameters._FileB = nullptr;
		parameters._FileA = nullptr;
		parameters._ApplyGammaCorrection = false;

		ResourceBuilder::BuildTexture2D(parameters);
	}

	{
		Texture2DBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_44_Texture2D";
		parameters._ID = "Blue_Noise_44_Texture2D";
		parameters._MipmapLevels = 1;
		parameters._Mode = Texture2DBuildParameters::Mode::RToRGBA;
		parameters._FileR = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Textures\\Blue_Noise_Textures\\HDR_RGBA_44.png";
		parameters._FileG = nullptr;
		parameters._FileB = nullptr;
		parameters._FileA = nullptr;
		parameters._ApplyGammaCorrection = false;

		ResourceBuilder::BuildTexture2D(parameters);
	}

	{
		Texture2DBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_45_Texture2D";
		parameters._ID = "Blue_Noise_45_Texture2D";
		parameters._MipmapLevels = 1;
		parameters._Mode = Texture2DBuildParameters::Mode::RToRGBA;
		parameters._FileR = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Textures\\Blue_Noise_Textures\\HDR_RGBA_45.png";
		parameters._FileG = nullptr;
		parameters._FileB = nullptr;
		parameters._FileA = nullptr;
		parameters._ApplyGammaCorrection = false;

		ResourceBuilder::BuildTexture2D(parameters);
	}

	{
		Texture2DBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_46_Texture2D";
		parameters._ID = "Blue_Noise_46_Texture2D";
		parameters._MipmapLevels = 1;
		parameters._Mode = Texture2DBuildParameters::Mode::RToRGBA;
		parameters._FileR = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Textures\\Blue_Noise_Textures\\HDR_RGBA_46.png";
		parameters._FileG = nullptr;
		parameters._FileB = nullptr;
		parameters._FileA = nullptr;
		parameters._ApplyGammaCorrection = false;

		ResourceBuilder::BuildTexture2D(parameters);
	}

	{
		Texture2DBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_47_Texture2D";
		parameters._ID = "Blue_Noise_47_Texture2D";
		parameters._MipmapLevels = 1;
		parameters._Mode = Texture2DBuildParameters::Mode::RToRGBA;
		parameters._FileR = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Textures\\Blue_Noise_Textures\\HDR_RGBA_47.png";
		parameters._FileG = nullptr;
		parameters._FileB = nullptr;
		parameters._FileA = nullptr;
		parameters._ApplyGammaCorrection = false;

		ResourceBuilder::BuildTexture2D(parameters);
	}

	{
		Texture2DBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_48_Texture2D";
		parameters._ID = "Blue_Noise_48_Texture2D";
		parameters._MipmapLevels = 1;
		parameters._Mode = Texture2DBuildParameters::Mode::RToRGBA;
		parameters._FileR = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Textures\\Blue_Noise_Textures\\HDR_RGBA_48.png";
		parameters._FileG = nullptr;
		parameters._FileB = nullptr;
		parameters._FileA = nullptr;
		parameters._ApplyGammaCorrection = false;

		ResourceBuilder::BuildTexture2D(parameters);
	}

	{
		Texture2DBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_49_Texture2D";
		parameters._ID = "Blue_Noise_49_Texture2D";
		parameters._MipmapLevels = 1;
		parameters._Mode = Texture2DBuildParameters::Mode::RToRGBA;
		parameters._FileR = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Textures\\Blue_Noise_Textures\\HDR_RGBA_49.png";
		parameters._FileG = nullptr;
		parameters._FileB = nullptr;
		parameters._FileA = nullptr;
		parameters._ApplyGammaCorrection = false;

		ResourceBuilder::BuildTexture2D(parameters);
	}

	{
		Texture2DBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_50_Texture2D";
		parameters._ID = "Blue_Noise_50_Texture2D";
		parameters._MipmapLevels = 1;
		parameters._Mode = Texture2DBuildParameters::Mode::RToRGBA;
		parameters._FileR = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Textures\\Blue_Noise_Textures\\HDR_RGBA_50.png";
		parameters._FileG = nullptr;
		parameters._FileB = nullptr;
		parameters._FileA = nullptr;
		parameters._ApplyGammaCorrection = false;

		ResourceBuilder::BuildTexture2D(parameters);
	}

	{
		Texture2DBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_51_Texture2D";
		parameters._ID = "Blue_Noise_51_Texture2D";
		parameters._MipmapLevels = 1;
		parameters._Mode = Texture2DBuildParameters::Mode::RToRGBA;
		parameters._FileR = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Textures\\Blue_Noise_Textures\\HDR_RGBA_51.png";
		parameters._FileG = nullptr;
		parameters._FileB = nullptr;
		parameters._FileA = nullptr;
		parameters._ApplyGammaCorrection = false;

		ResourceBuilder::BuildTexture2D(parameters);
	}

	{
		Texture2DBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_52_Texture2D";
		parameters._ID = "Blue_Noise_52_Texture2D";
		parameters._MipmapLevels = 1;
		parameters._Mode = Texture2DBuildParameters::Mode::RToRGBA;
		parameters._FileR = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Textures\\Blue_Noise_Textures\\HDR_RGBA_52.png";
		parameters._FileG = nullptr;
		parameters._FileB = nullptr;
		parameters._FileA = nullptr;
		parameters._ApplyGammaCorrection = false;

		ResourceBuilder::BuildTexture2D(parameters);
	}

	{
		Texture2DBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_53_Texture2D";
		parameters._ID = "Blue_Noise_53_Texture2D";
		parameters._MipmapLevels = 1;
		parameters._Mode = Texture2DBuildParameters::Mode::RToRGBA;
		parameters._FileR = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Textures\\Blue_Noise_Textures\\HDR_RGBA_53.png";
		parameters._FileG = nullptr;
		parameters._FileB = nullptr;
		parameters._FileA = nullptr;
		parameters._ApplyGammaCorrection = false;

		ResourceBuilder::BuildTexture2D(parameters);
	}

	{
		Texture2DBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_54_Texture2D";
		parameters._ID = "Blue_Noise_54_Texture2D";
		parameters._MipmapLevels = 1;
		parameters._Mode = Texture2DBuildParameters::Mode::RToRGBA;
		parameters._FileR = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Textures\\Blue_Noise_Textures\\HDR_RGBA_54.png";
		parameters._FileG = nullptr;
		parameters._FileB = nullptr;
		parameters._FileA = nullptr;
		parameters._ApplyGammaCorrection = false;

		ResourceBuilder::BuildTexture2D(parameters);
	}

	{
		Texture2DBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_55_Texture2D";
		parameters._ID = "Blue_Noise_55_Texture2D";
		parameters._MipmapLevels = 1;
		parameters._Mode = Texture2DBuildParameters::Mode::RToRGBA;
		parameters._FileR = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Textures\\Blue_Noise_Textures\\HDR_RGBA_55.png";
		parameters._FileG = nullptr;
		parameters._FileB = nullptr;
		parameters._FileA = nullptr;
		parameters._ApplyGammaCorrection = false;

		ResourceBuilder::BuildTexture2D(parameters);
	}

	{
		Texture2DBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_56_Texture2D";
		parameters._ID = "Blue_Noise_56_Texture2D";
		parameters._MipmapLevels = 1;
		parameters._Mode = Texture2DBuildParameters::Mode::RToRGBA;
		parameters._FileR = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Textures\\Blue_Noise_Textures\\HDR_RGBA_56.png";
		parameters._FileG = nullptr;
		parameters._FileB = nullptr;
		parameters._FileA = nullptr;
		parameters._ApplyGammaCorrection = false;

		ResourceBuilder::BuildTexture2D(parameters);
	}

	{
		Texture2DBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_57_Texture2D";
		parameters._ID = "Blue_Noise_57_Texture2D";
		parameters._MipmapLevels = 1;
		parameters._Mode = Texture2DBuildParameters::Mode::RToRGBA;
		parameters._FileR = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Textures\\Blue_Noise_Textures\\HDR_RGBA_57.png";
		parameters._FileG = nullptr;
		parameters._FileB = nullptr;
		parameters._FileA = nullptr;
		parameters._ApplyGammaCorrection = false;

		ResourceBuilder::BuildTexture2D(parameters);
	}

	{
		Texture2DBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_58_Texture2D";
		parameters._ID = "Blue_Noise_58_Texture2D";
		parameters._MipmapLevels = 1;
		parameters._Mode = Texture2DBuildParameters::Mode::RToRGBA;
		parameters._FileR = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Textures\\Blue_Noise_Textures\\HDR_RGBA_58.png";
		parameters._FileG = nullptr;
		parameters._FileB = nullptr;
		parameters._FileA = nullptr;
		parameters._ApplyGammaCorrection = false;

		ResourceBuilder::BuildTexture2D(parameters);
	}

	{
		Texture2DBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_59_Texture2D";
		parameters._ID = "Blue_Noise_59_Texture2D";
		parameters._MipmapLevels = 1;
		parameters._Mode = Texture2DBuildParameters::Mode::RToRGBA;
		parameters._FileR = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Textures\\Blue_Noise_Textures\\HDR_RGBA_59.png";
		parameters._FileG = nullptr;
		parameters._FileB = nullptr;
		parameters._FileA = nullptr;
		parameters._ApplyGammaCorrection = false;

		ResourceBuilder::BuildTexture2D(parameters);
	}

	{
		Texture2DBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_60_Texture2D";
		parameters._ID = "Blue_Noise_60_Texture2D";
		parameters._MipmapLevels = 1;
		parameters._Mode = Texture2DBuildParameters::Mode::RToRGBA;
		parameters._FileR = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Textures\\Blue_Noise_Textures\\HDR_RGBA_60.png";
		parameters._FileG = nullptr;
		parameters._FileB = nullptr;
		parameters._FileA = nullptr;
		parameters._ApplyGammaCorrection = false;

		ResourceBuilder::BuildTexture2D(parameters);
	}

	{
		Texture2DBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_61_Texture2D";
		parameters._ID = "Blue_Noise_61_Texture2D";
		parameters._MipmapLevels = 1;
		parameters._Mode = Texture2DBuildParameters::Mode::RToRGBA;
		parameters._FileR = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Textures\\Blue_Noise_Textures\\HDR_RGBA_61.png";
		parameters._FileG = nullptr;
		parameters._FileB = nullptr;
		parameters._FileA = nullptr;
		parameters._ApplyGammaCorrection = false;

		ResourceBuilder::BuildTexture2D(parameters);
	}

	{
		Texture2DBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_62_Texture2D";
		parameters._ID = "Blue_Noise_62_Texture2D";
		parameters._MipmapLevels = 1;
		parameters._Mode = Texture2DBuildParameters::Mode::RToRGBA;
		parameters._FileR = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Textures\\Blue_Noise_Textures\\HDR_RGBA_62.png";
		parameters._FileG = nullptr;
		parameters._FileB = nullptr;
		parameters._FileA = nullptr;
		parameters._ApplyGammaCorrection = false;

		ResourceBuilder::BuildTexture2D(parameters);
	}

	{
		Texture2DBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_63_Texture2D";
		parameters._ID = "Blue_Noise_63_Texture2D";
		parameters._MipmapLevels = 1;
		parameters._Mode = Texture2DBuildParameters::Mode::RToRGBA;
		parameters._FileR = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Textures\\Blue_Noise_Textures\\HDR_RGBA_63.png";
		parameters._FileG = nullptr;
		parameters._FileB = nullptr;
		parameters._FileA = nullptr;
		parameters._ApplyGammaCorrection = false;

		ResourceBuilder::BuildTexture2D(parameters);
	}
#endif

#if BUILD_ENGINE_COLOR_GRADING_TEXTURES
	{
		Texture2DBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Color_Grading_Lookup_Texture2D";
		parameters._ID = "Color_Grading_Lookup_Texture2D";
		parameters._MipmapLevels = 1;
		parameters._Mode = Texture2DBuildParameters::Mode::RToRGBA;
		parameters._FileR = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Textures\\Color_Grading_Lookup_Textures\\LUT_Filmic7.png";
		parameters._FileG = nullptr;
		parameters._FileB = nullptr;
		parameters._FileA = nullptr;
		parameters._ApplyGammaCorrection = false;

		ResourceBuilder::BuildTexture2D(parameters);
	}
#endif

#if BUILD_ENGINE_CLOUD_TEXTURE || BUILD_ENGINE_FONTS || BUILD_ENGINE_OCEAN_TEXTURE || BUILD_ENGINE_NOISE_TEXTURES ||BUILD_ENGINE_COLOR_GRADING_TEXTURES
	{
		ResourceCollectionBuildParameters resourceCollectionBuildParameters;

		resourceCollectionBuildParameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Final\\CatalystEngineResourceCollection";
		resourceCollectionBuildParameters._Folder = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate";

		ResourceBuilder::BuildResourceCollection(resourceCollectionBuildParameters);
	}
#endif
}

/*
*	Builds the cloud texture.
*/
void CatalystEngineResourceBuilding::BuildCloudTexture()
{
	//Defone constants.
	constexpr uint32 CLOUD_TEXTURE_RESOLUTION{ 64 };
	constexpr uint32 CLOUD_TEXTURE_LAYER_0_POINTS{ 64 };

	//Generate the points for the layers.
	StaticArray<DynamicArray<Vector3<float>>, 4> points;

	for (uint8 i{ 0 }; i < 4; ++i)
	{
		const uint32 cloud_layer_points{ CLOUD_TEXTURE_LAYER_0_POINTS << i };

		points[i].Reserve(cloud_layer_points * 27);

		for (uint32 j{ 0 }; j < cloud_layer_points; ++j)
		{
			points[i].EmplaceSlow(	CatalystRandomMath::RandomFloatInRange(0.0f, 1.0f),
									CatalystRandomMath::RandomFloatInRange(0.0f, 1.0f),
									CatalystRandomMath::RandomFloatInRange(0.0f, 1.0f));
		}
	}

	//Copy the first N points to the sides of the cube.
	for (int8 X{ -1 }; X <= 1; ++X)
	{
		for (int8 Y{ -1 }; Y <= 1; ++Y)
		{
			for (int8 Z{ -1 }; Z <= 1; ++Z)
			{
				if (X == 0 && Y == 0 && Z == 0)
				{
					continue;
				}

				for (uint8 i{ 0 }; i < 4; ++i)
				{
					const uint32 cloud_layer_points{ CLOUD_TEXTURE_LAYER_0_POINTS << i };

					for (uint32 j{ 0 }; j < cloud_layer_points; ++j)
					{
						const Vector3<float> offset{ static_cast<float>(X), static_cast<float>(Y), static_cast<float>(Z) };
						points[i].EmplaceFast(points[i][j] + offset);
					}
				}
			}
		}
	}

	//Create the temporary texture.
	Texture3D<Vector4<float>> temporary_texture{ CLOUD_TEXTURE_RESOLUTION };

	//Keep track of the longest distances.
	StaticArray<float, 4> longest_distances{ -FLOAT_MAXIMUM, -FLOAT_MAXIMUM, -FLOAT_MAXIMUM, -FLOAT_MAXIMUM };

	for (uint32 X{ 0 }; X < CLOUD_TEXTURE_RESOLUTION; ++X)
	{
		for (uint32 Y{ 0 }; Y < CLOUD_TEXTURE_RESOLUTION; ++Y)
		{
			for (uint32 Z{ 0 }; Z < CLOUD_TEXTURE_RESOLUTION; ++Z)
			{
				//Calcualte the position in the texture.
				const Vector3<float> position{	static_cast<float>(X) / static_cast<float>(CLOUD_TEXTURE_RESOLUTION),
												static_cast<float>(Y) / static_cast<float>(CLOUD_TEXTURE_RESOLUTION),
												static_cast<float>(Z) / static_cast<float>(CLOUD_TEXTURE_RESOLUTION) };

				for (uint8 i{ 0 }; i < 4; ++i)
				{
					//Find the closest distance.
					float closest_distance{ FLOAT_MAXIMUM };

					for (const Vector3<float>& point : points[i])
					{
						const float distance{ Vector3<float>::Length(position - point) };
						closest_distance = CatalystBaseMath::Minimum<float>(closest_distance, distance);
					}

					//Write to the texture.
					temporary_texture.At(X, Y, Z)[i] = closest_distance;

					//Update the longest distance.
					longest_distances[i] = CatalystBaseMath::Maximum<float>(longest_distances[i], closest_distance);
				}
			}
		}
	}

	//Create the final texture.
	Texture3D<Vector4<byte>> final_texture{ CLOUD_TEXTURE_RESOLUTION };

	for (uint32 X{ 0 }; X < CLOUD_TEXTURE_RESOLUTION; ++X)
	{
		for (uint32 Y{ 0 }; Y < CLOUD_TEXTURE_RESOLUTION; ++Y)
		{
			for (uint32 Z{ 0 }; Z < CLOUD_TEXTURE_RESOLUTION; ++Z)
			{
				//Get the distances at the current position.
				Vector4<float> distances{ temporary_texture.At(X, Y, Z) };

				//Normalize the distances.
				for (uint8 i{ 0 }; i < 4; ++i)
				{
					distances[i] /= longest_distances[i];
				}

				//Invert the distance.
				for (uint8 i{ 0 }; i < 4; ++i)
				{
					distances[i] = 1.0f - distances[i];
				}

				//Convert it into byte.
				final_texture.At(X, Y, Z) = Vector4<byte>(static_cast<byte>(distances[0] * UINT8_MAXIMUM), static_cast<byte>(distances[1] * UINT8_MAXIMUM), static_cast<byte>(distances[2] * UINT8_MAXIMUM), static_cast<byte>(distances[3] * UINT8_MAXIMUM));
			}
		}
	}

	Texture3DBuildParameters parameters;

	parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Cloud_Texture3D";
	parameters._ID = "Cloud_Texture3D";
	parameters._Texture = &final_texture;

	ResourceBuilder::BuildTexture3D(parameters);
}

/*
*	Builds the ocean texture.
*/
void CatalystEngineResourceBuilding::BuildOceanTexture()
{
	//Defone constants.
	constexpr uint32 OCEAN_TEXTURE_RESOLUTION{ 64 };
	constexpr uint32 OCEAN_TEXTURE_LAYER_0_POINTS{ 64 };

	//Generate the points for the layers.
	StaticArray<DynamicArray<Vector2<float>>, 4> points;

	for (uint8 i{ 0 }; i < 4; ++i)
	{
		const uint32 cloud_layer_points{ OCEAN_TEXTURE_LAYER_0_POINTS << i };

		points[i].Reserve(cloud_layer_points * 27);

		for (uint32 j{ 0 }; j < cloud_layer_points; ++j)
		{
			points[i].EmplaceSlow(	CatalystRandomMath::RandomFloatInRange(0.0f, 1.0f),
									CatalystRandomMath::RandomFloatInRange(0.0f, 1.0f));
		}
	}

	//Copy the first N points to the sides of the cube.
	for (int8 X{ -1 }; X <= 1; ++X)
	{
		for (int8 Y{ -1 }; Y <= 1; ++Y)
		{
			if (X == 0 && Y == 0)
			{
				continue;
			}

			for (uint8 i{ 0 }; i < 4; ++i)
			{
				const uint32 cloud_layer_points{ OCEAN_TEXTURE_LAYER_0_POINTS << i };

				for (uint32 j{ 0 }; j < cloud_layer_points; ++j)
				{
					const Vector2<float> offset{ static_cast<float>(X), static_cast<float>(Y) };
					points[i].EmplaceFast(points[i][j] + offset);
				}
			}
		}
	}

	//Create the temporary texture.
	Texture2D<Vector4<float>> temporary_texture{ OCEAN_TEXTURE_RESOLUTION };

	//Keep track of the longest distances.
	StaticArray<float, 4> longest_distances{ -FLOAT_MAXIMUM, -FLOAT_MAXIMUM, -FLOAT_MAXIMUM, -FLOAT_MAXIMUM };

	for (uint32 X{ 0 }; X < OCEAN_TEXTURE_RESOLUTION; ++X)
	{
		for (uint32 Y{ 0 }; Y < OCEAN_TEXTURE_RESOLUTION; ++Y)
		{
			//Calcualte the position in the texture.
			const Vector2<float> position{	static_cast<float>(X) / static_cast<float>(OCEAN_TEXTURE_RESOLUTION),
											static_cast<float>(Y) / static_cast<float>(OCEAN_TEXTURE_RESOLUTION) };

			for (uint8 i{ 0 }; i < 4; ++i)
			{
				//Find the closest distance.
				float closest_distance{ FLOAT_MAXIMUM };

				for (const Vector2<float>& point : points[i])
				{
					const float distance{ Vector2<float>::Length(position - point) };
					closest_distance = CatalystBaseMath::Minimum<float>(closest_distance, distance);
				}

				//Write to the texture.
				temporary_texture.At(X, Y)[i] = closest_distance;

				//Update the longest distance.
				longest_distances[i] = CatalystBaseMath::Maximum<float>(longest_distances[i], closest_distance);
			}
		}
	}

	//Create the final texture.
	Texture2D<Vector4<byte>> final_texture{ OCEAN_TEXTURE_RESOLUTION };

	for (uint32 X{ 0 }; X < OCEAN_TEXTURE_RESOLUTION; ++X)
	{
		for (uint32 Y{ 0 }; Y < OCEAN_TEXTURE_RESOLUTION; ++Y)
		{
			//Get the distances at the current position.
			Vector4<float> distances{ temporary_texture.At(X, Y) };

			//Normalize the distances.
			for (uint8 i{ 0 }; i < 4; ++i)
			{
				distances[i] /= longest_distances[i];
			}

			//Convert it into byte.
			final_texture.At(X, Y) = Vector4<byte>(static_cast<byte>(distances[0] * UINT8_MAXIMUM), static_cast<byte>(distances[1] * UINT8_MAXIMUM), static_cast<byte>(distances[2] * UINT8_MAXIMUM), static_cast<byte>(distances[3] * UINT8_MAXIMUM));
		}
	}

	//What should the file be called?
	DynamicString file_name{ "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Ocean_Texture2D" };
	file_name += ".cr";

	//Open the file to be written to.
	BinaryFile<IOMode::Out> file{ file_name.Data() };

	//Write the resource type to the file.
	constexpr ResourceType RESOURCE_TYPE{ ResourceType::Texture2D };
	file.Write(&RESOURCE_TYPE, sizeof(ResourceType));

	//Write the resource ID to the file.
	constexpr HashString RESOURCE_ID{ "Ocean_Texture2D" };
	file.Write(&RESOURCE_ID, sizeof(HashString));

	//Write the number of mipmap levels to the file.
	constexpr uint8 MIPMAP_LEVELS{ 1 };
	file.Write(&MIPMAP_LEVELS, sizeof(uint8));

	//Write the width and height of the texture to the file.
	file.Write(&OCEAN_TEXTURE_RESOLUTION, sizeof(uint32));
	file.Write(&OCEAN_TEXTURE_RESOLUTION, sizeof(uint32));

	//Write the texture data to the file.
	file.Write(final_texture.Data(), OCEAN_TEXTURE_RESOLUTION * OCEAN_TEXTURE_RESOLUTION * 4);

	//Cloe the file.
	file.Close();
}
#endif