#if defined(CATALYST_ENABLE_RESOURCE_BUILDING)
//Header file.
#include <Resources/Building/CatalystEngineResourceBuilding.h>

//Resources.
#include <Resources/Building/ResourceBuilder.h>

#define BUILD_ENGINE_RESOURCES false

/*
*	Builds resources for the Catalyst Engine.
*/
void CatalystEngineResourceBuilding::BuildResources() NOEXCEPT
{
#if BUILD_ENGINE_RESOURCES
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

		ResourceBuilder::BuildTexture2D(parameters);
	}

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

		ResourceBuilder::BuildTexture2D(parameters);
	}

	ResourceCollectionBuildParameters resourceCollectionBuildParameters;

	resourceCollectionBuildParameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Final\\CatalystEngineResourceCollection";

	resourceCollectionBuildParameters._Resources.EmplaceSlow("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_0_Texture2D.cr");
	resourceCollectionBuildParameters._Resources.EmplaceSlow("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_1_Texture2D.cr");
	resourceCollectionBuildParameters._Resources.EmplaceSlow("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_2_Texture2D.cr");
	resourceCollectionBuildParameters._Resources.EmplaceSlow("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_3_Texture2D.cr");
	resourceCollectionBuildParameters._Resources.EmplaceSlow("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_4_Texture2D.cr");
	resourceCollectionBuildParameters._Resources.EmplaceSlow("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_5_Texture2D.cr");
	resourceCollectionBuildParameters._Resources.EmplaceSlow("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_6_Texture2D.cr");
	resourceCollectionBuildParameters._Resources.EmplaceSlow("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_7_Texture2D.cr");
	resourceCollectionBuildParameters._Resources.EmplaceSlow("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_8_Texture2D.cr");
	resourceCollectionBuildParameters._Resources.EmplaceSlow("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_9_Texture2D.cr");
	resourceCollectionBuildParameters._Resources.EmplaceSlow("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_10_Texture2D.cr");
	resourceCollectionBuildParameters._Resources.EmplaceSlow("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_11_Texture2D.cr");
	resourceCollectionBuildParameters._Resources.EmplaceSlow("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_12_Texture2D.cr");
	resourceCollectionBuildParameters._Resources.EmplaceSlow("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_13_Texture2D.cr");
	resourceCollectionBuildParameters._Resources.EmplaceSlow("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_14_Texture2D.cr");
	resourceCollectionBuildParameters._Resources.EmplaceSlow("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_15_Texture2D.cr");
	resourceCollectionBuildParameters._Resources.EmplaceSlow("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_16_Texture2D.cr");
	resourceCollectionBuildParameters._Resources.EmplaceSlow("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_17_Texture2D.cr");
	resourceCollectionBuildParameters._Resources.EmplaceSlow("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_18_Texture2D.cr");
	resourceCollectionBuildParameters._Resources.EmplaceSlow("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_19_Texture2D.cr");
	resourceCollectionBuildParameters._Resources.EmplaceSlow("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_20_Texture2D.cr");
	resourceCollectionBuildParameters._Resources.EmplaceSlow("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_21_Texture2D.cr");
	resourceCollectionBuildParameters._Resources.EmplaceSlow("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_22_Texture2D.cr");
	resourceCollectionBuildParameters._Resources.EmplaceSlow("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_23_Texture2D.cr");
	resourceCollectionBuildParameters._Resources.EmplaceSlow("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_24_Texture2D.cr");
	resourceCollectionBuildParameters._Resources.EmplaceSlow("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_25_Texture2D.cr");
	resourceCollectionBuildParameters._Resources.EmplaceSlow("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_26_Texture2D.cr");
	resourceCollectionBuildParameters._Resources.EmplaceSlow("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_27_Texture2D.cr");
	resourceCollectionBuildParameters._Resources.EmplaceSlow("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_28_Texture2D.cr");
	resourceCollectionBuildParameters._Resources.EmplaceSlow("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_29_Texture2D.cr");
	resourceCollectionBuildParameters._Resources.EmplaceSlow("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_30_Texture2D.cr");
	resourceCollectionBuildParameters._Resources.EmplaceSlow("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_31_Texture2D.cr");
	resourceCollectionBuildParameters._Resources.EmplaceSlow("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_32_Texture2D.cr");
	resourceCollectionBuildParameters._Resources.EmplaceSlow("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_33_Texture2D.cr");
	resourceCollectionBuildParameters._Resources.EmplaceSlow("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_34_Texture2D.cr");
	resourceCollectionBuildParameters._Resources.EmplaceSlow("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_35_Texture2D.cr");
	resourceCollectionBuildParameters._Resources.EmplaceSlow("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_36_Texture2D.cr");
	resourceCollectionBuildParameters._Resources.EmplaceSlow("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_37_Texture2D.cr");
	resourceCollectionBuildParameters._Resources.EmplaceSlow("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_38_Texture2D.cr");
	resourceCollectionBuildParameters._Resources.EmplaceSlow("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_39_Texture2D.cr");
	resourceCollectionBuildParameters._Resources.EmplaceSlow("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_40_Texture2D.cr");
	resourceCollectionBuildParameters._Resources.EmplaceSlow("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_41_Texture2D.cr");
	resourceCollectionBuildParameters._Resources.EmplaceSlow("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_42_Texture2D.cr");
	resourceCollectionBuildParameters._Resources.EmplaceSlow("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_43_Texture2D.cr");
	resourceCollectionBuildParameters._Resources.EmplaceSlow("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_44_Texture2D.cr");
	resourceCollectionBuildParameters._Resources.EmplaceSlow("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_45_Texture2D.cr");
	resourceCollectionBuildParameters._Resources.EmplaceSlow("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_46_Texture2D.cr");
	resourceCollectionBuildParameters._Resources.EmplaceSlow("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_47_Texture2D.cr");
	resourceCollectionBuildParameters._Resources.EmplaceSlow("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_48_Texture2D.cr");
	resourceCollectionBuildParameters._Resources.EmplaceSlow("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_49_Texture2D.cr");
	resourceCollectionBuildParameters._Resources.EmplaceSlow("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_50_Texture2D.cr");
	resourceCollectionBuildParameters._Resources.EmplaceSlow("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_51_Texture2D.cr");
	resourceCollectionBuildParameters._Resources.EmplaceSlow("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_52_Texture2D.cr");
	resourceCollectionBuildParameters._Resources.EmplaceSlow("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_53_Texture2D.cr");
	resourceCollectionBuildParameters._Resources.EmplaceSlow("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_54_Texture2D.cr");
	resourceCollectionBuildParameters._Resources.EmplaceSlow("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_55_Texture2D.cr");
	resourceCollectionBuildParameters._Resources.EmplaceSlow("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_56_Texture2D.cr");
	resourceCollectionBuildParameters._Resources.EmplaceSlow("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_57_Texture2D.cr");
	resourceCollectionBuildParameters._Resources.EmplaceSlow("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_58_Texture2D.cr");
	resourceCollectionBuildParameters._Resources.EmplaceSlow("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_59_Texture2D.cr");
	resourceCollectionBuildParameters._Resources.EmplaceSlow("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_60_Texture2D.cr");
	resourceCollectionBuildParameters._Resources.EmplaceSlow("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_61_Texture2D.cr");
	resourceCollectionBuildParameters._Resources.EmplaceSlow("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_62_Texture2D.cr");
	resourceCollectionBuildParameters._Resources.EmplaceSlow("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_63_Texture2D.cr");
	resourceCollectionBuildParameters._Resources.EmplaceSlow("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Color_Grading_Lookup_Texture2D.cr");

	ResourceBuilder::BuildResourceCollection(resourceCollectionBuildParameters);
#endif
}
#endif