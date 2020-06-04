#if !defined(CATALYST_CONFIGURATION_FINAL)
//Header file.
#include <Resources/Building/CatalystEngineResourceBuilding.h>

//Core.
#include <Core/Containers/StaticArray.h>
#include <Core/General/DynamicString.h>
#include <Core/General/HashString.h>

//Concurrency.
#include <Concurrency/Task.h>

//File.
#include <File/Core/BinaryFile.h>

//Math.
#include <Math/Core/CatalystRandomMath.h>

//Rendering.
#include <Rendering/Native/Texture2D.h>

//Resources.
#include <Resources/Core/ResourcesCore.h>

//Systems.
#include <Systems/MemorySystem.h>
#include <Systems/ResourceSystem.h>
#include <Systems/TaskSystem.h>

#define BUILD_ENGINE_ALL false

#define BUILD_ENGINE_CLOUD_TEXTURE false
#define BUILD_ENGINE_FONTS false
#define BUILD_ENGINE_OCEAN_TEXTURE false
#define BUILD_ENGINE_BLUE_NOISE_TEXTURES false
#define BUILD_ENGINE_SHADERS false
#define BUILD_ENGINE_DEFAULT_SKY_TEXTURE false
#define BUILD_ENGINE_MATERIALS false
#define BUILD_ENGINE_MODELS false

#define BUILD_ENGINE_RESOURCE_COLLECTION true

/*
*	Builds resources for the Catalyst Engine.
*/
void CatalystEngineResourceBuilding::BuildResources() NOEXCEPT
{
	//Initialize the task system as it will be used to build resources.
	TaskSystem::Instance->Initialize();

	//Keep track of all tasks so that they can be deallocated.
	DynamicArray<Task* RESTRICT> tasks;

#if BUILD_ENGINE_ALL || BUILD_ENGINE_CLOUD_TEXTURE
	BuildCloudTexture();
#endif

#if BUILD_ENGINE_ALL || BUILD_ENGINE_FONTS
	{
		FontBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Catalyst_Engine_Default_Font";
		parameters._ID = "Catalyst_Engine_Default_Font";
		parameters._File = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Fonts\\Timeless.ttf";

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildFont(parameters);
	}
#endif

#if BUILD_ENGINE_ALL || BUILD_ENGINE_OCEAN_TEXTURE
	BuildOceanTexture();
#endif

#if BUILD_ENGINE_ALL || BUILD_ENGINE_BLUE_NOISE_TEXTURES
	{
		Texture2DBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_0_Texture2D";
		parameters._ID = "Blue_Noise_0_Texture2D";
		parameters._DefaultWidth = 0;
		parameters._DefaultHeight = 0;
		parameters._File1 = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Textures\\Blue_Noise_Textures\\HDR_RGBA_0.png";
		parameters._File2 = nullptr;
		parameters._File3 = nullptr;
		parameters._File4 = nullptr;
		parameters._Default = Vector4<float32>(0.0f, 0.0f, 0.0f, 0.0f);
		parameters._Default = Vector4<float32>(0.0f, 0.0f, 0.0f, 0.0f);
		parameters._ChannelMappings[0] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::RED);
		parameters._ChannelMappings[1] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::GREEN);
		parameters._ChannelMappings[2] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::BLUE);
		parameters._ChannelMappings[3] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::ALPHA);
		parameters._ApplyGammaCorrection = false;
		parameters._TransformFunction = nullptr;
		parameters._BaseMipmapLevel = 0;
		parameters._MipmapLevels = 1;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildTexture2D(parameters);
	}

	{
		Texture2DBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_1_Texture2D";
		parameters._ID = "Blue_Noise_1_Texture2D";
		parameters._DefaultWidth = 0;
		parameters._DefaultHeight = 0;
		parameters._File1 = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Textures\\Blue_Noise_Textures\\HDR_RGBA_1.png";
		parameters._File2 = nullptr;
		parameters._File3 = nullptr;
		parameters._File4 = nullptr;
		parameters._Default = Vector4<float32>(0.0f, 0.0f, 0.0f, 0.0f);
		parameters._ChannelMappings[0] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::RED);
		parameters._ChannelMappings[1] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::GREEN);
		parameters._ChannelMappings[2] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::BLUE);
		parameters._ChannelMappings[3] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::ALPHA);
		parameters._ApplyGammaCorrection = false;
		parameters._TransformFunction = nullptr;
		parameters._BaseMipmapLevel = 0;
		parameters._MipmapLevels = 1;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildTexture2D(parameters);
	}

	{
		Texture2DBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_2_Texture2D";
		parameters._ID = "Blue_Noise_2_Texture2D";
		parameters._DefaultWidth = 0;
		parameters._DefaultHeight = 0;
		parameters._File1 = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Textures\\Blue_Noise_Textures\\HDR_RGBA_2.png";
		parameters._File2 = nullptr;
		parameters._File3 = nullptr;
		parameters._File4 = nullptr;
		parameters._Default = Vector4<float32>(0.0f, 0.0f, 0.0f, 0.0f);
		parameters._ChannelMappings[0] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::RED);
		parameters._ChannelMappings[1] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::GREEN);
		parameters._ChannelMappings[2] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::BLUE);
		parameters._ChannelMappings[3] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::ALPHA);
		parameters._ApplyGammaCorrection = false;
		parameters._TransformFunction = nullptr;
		parameters._BaseMipmapLevel = 0;
		parameters._MipmapLevels = 1;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildTexture2D(parameters);
	}

	{
		Texture2DBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_3_Texture2D";
		parameters._ID = "Blue_Noise_3_Texture2D";
		parameters._DefaultWidth = 0;
		parameters._DefaultHeight = 0;
		parameters._File1 = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Textures\\Blue_Noise_Textures\\HDR_RGBA_3.png";
		parameters._File2 = nullptr;
		parameters._File3 = nullptr;
		parameters._File4 = nullptr;
		parameters._Default = Vector4<float32>(0.0f, 0.0f, 0.0f, 0.0f);
		parameters._ChannelMappings[0] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::RED);
		parameters._ChannelMappings[1] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::GREEN);
		parameters._ChannelMappings[2] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::BLUE);
		parameters._ChannelMappings[3] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::ALPHA);
		parameters._ApplyGammaCorrection = false;
		parameters._TransformFunction = nullptr;
		parameters._BaseMipmapLevel = 0;
		parameters._MipmapLevels = 1;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildTexture2D(parameters);
	}

	{
		Texture2DBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_4_Texture2D";
		parameters._ID = "Blue_Noise_4_Texture2D";
		parameters._DefaultWidth = 0;
		parameters._DefaultHeight = 0;
		parameters._File1 = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Textures\\Blue_Noise_Textures\\HDR_RGBA_4.png";
		parameters._File2 = nullptr;
		parameters._File3 = nullptr;
		parameters._File4 = nullptr;
		parameters._Default = Vector4<float32>(0.0f, 0.0f, 0.0f, 0.0f);
		parameters._ChannelMappings[0] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::RED);
		parameters._ChannelMappings[1] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::GREEN);
		parameters._ChannelMappings[2] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::BLUE);
		parameters._ChannelMappings[3] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::ALPHA);
		parameters._ApplyGammaCorrection = false;
		parameters._TransformFunction = nullptr;
		parameters._BaseMipmapLevel = 0;
		parameters._MipmapLevels = 1;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildTexture2D(parameters);
	}

	{
		Texture2DBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_5_Texture2D";
		parameters._ID = "Blue_Noise_5_Texture2D";
		parameters._DefaultWidth = 0;
		parameters._DefaultHeight = 0;
		parameters._File1 = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Textures\\Blue_Noise_Textures\\HDR_RGBA_5.png";
		parameters._File2 = nullptr;
		parameters._File3 = nullptr;
		parameters._File4 = nullptr;
		parameters._Default = Vector4<float32>(0.0f, 0.0f, 0.0f, 0.0f);
		parameters._ChannelMappings[0] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::RED);
		parameters._ChannelMappings[1] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::GREEN);
		parameters._ChannelMappings[2] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::BLUE);
		parameters._ChannelMappings[3] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::ALPHA);
		parameters._ApplyGammaCorrection = false;
		parameters._TransformFunction = nullptr;
		parameters._BaseMipmapLevel = 0;
		parameters._MipmapLevels = 1;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildTexture2D(parameters);
	}

	{
		Texture2DBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_6_Texture2D";
		parameters._ID = "Blue_Noise_6_Texture2D";
		parameters._DefaultWidth = 0;
		parameters._DefaultHeight = 0;
		parameters._File1 = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Textures\\Blue_Noise_Textures\\HDR_RGBA_6.png";
		parameters._File2 = nullptr;
		parameters._File3 = nullptr;
		parameters._File4 = nullptr;
		parameters._Default = Vector4<float32>(0.0f, 0.0f, 0.0f, 0.0f);
		parameters._ChannelMappings[0] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::RED);
		parameters._ChannelMappings[1] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::GREEN);
		parameters._ChannelMappings[2] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::BLUE);
		parameters._ChannelMappings[3] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::ALPHA);
		parameters._ApplyGammaCorrection = false;
		parameters._TransformFunction = nullptr;
		parameters._BaseMipmapLevel = 0;
		parameters._MipmapLevels = 1;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildTexture2D(parameters);
	}

	{
		Texture2DBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_7_Texture2D";
		parameters._ID = "Blue_Noise_7_Texture2D";
		parameters._DefaultWidth = 0;
		parameters._DefaultHeight = 0;
		parameters._File1 = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Textures\\Blue_Noise_Textures\\HDR_RGBA_7.png";
		parameters._File2 = nullptr;
		parameters._File3 = nullptr;
		parameters._File4 = nullptr;
		parameters._Default = Vector4<float32>(0.0f, 0.0f, 0.0f, 0.0f);
		parameters._ChannelMappings[0] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::RED);
		parameters._ChannelMappings[1] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::GREEN);
		parameters._ChannelMappings[2] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::BLUE);
		parameters._ChannelMappings[3] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::ALPHA);
		parameters._ApplyGammaCorrection = false;
		parameters._TransformFunction = nullptr;
		parameters._BaseMipmapLevel = 0;
		parameters._MipmapLevels = 1;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildTexture2D(parameters);
	}

	{
		Texture2DBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_8_Texture2D";
		parameters._ID = "Blue_Noise_8_Texture2D";
		parameters._DefaultWidth = 0;
		parameters._DefaultHeight = 0;
		parameters._File1 = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Textures\\Blue_Noise_Textures\\HDR_RGBA_8.png";
		parameters._File2 = nullptr;
		parameters._File3 = nullptr;
		parameters._File4 = nullptr;
		parameters._Default = Vector4<float32>(0.0f, 0.0f, 0.0f, 0.0f);
		parameters._ChannelMappings[0] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::RED);
		parameters._ChannelMappings[1] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::GREEN);
		parameters._ChannelMappings[2] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::BLUE);
		parameters._ChannelMappings[3] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::ALPHA);
		parameters._ApplyGammaCorrection = false;
		parameters._TransformFunction = nullptr;
		parameters._BaseMipmapLevel = 0;
		parameters._MipmapLevels = 1;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildTexture2D(parameters);
	}

	{
		Texture2DBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_9_Texture2D";
		parameters._ID = "Blue_Noise_9_Texture2D";
		parameters._DefaultWidth = 0;
		parameters._DefaultHeight = 0;
		parameters._File1 = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Textures\\Blue_Noise_Textures\\HDR_RGBA_9.png";
		parameters._File2 = nullptr;
		parameters._File3 = nullptr;
		parameters._File4 = nullptr;
		parameters._Default = Vector4<float32>(0.0f, 0.0f, 0.0f, 0.0f);
		parameters._ChannelMappings[0] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::RED);
		parameters._ChannelMappings[1] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::GREEN);
		parameters._ChannelMappings[2] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::BLUE);
		parameters._ChannelMappings[3] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::ALPHA);
		parameters._ApplyGammaCorrection = false;
		parameters._TransformFunction = nullptr;
		parameters._BaseMipmapLevel = 0;
		parameters._MipmapLevels = 1;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildTexture2D(parameters);
	}

	{
		Texture2DBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_10_Texture2D";
		parameters._ID = "Blue_Noise_10_Texture2D";
		parameters._DefaultWidth = 0;
		parameters._DefaultHeight = 0;
		parameters._File1 = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Textures\\Blue_Noise_Textures\\HDR_RGBA_10.png";
		parameters._File2 = nullptr;
		parameters._File3 = nullptr;
		parameters._File4 = nullptr;
		parameters._Default = Vector4<float32>(0.0f, 0.0f, 0.0f, 0.0f);
		parameters._ChannelMappings[0] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::RED);
		parameters._ChannelMappings[1] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::GREEN);
		parameters._ChannelMappings[2] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::BLUE);
		parameters._ChannelMappings[3] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::ALPHA);
		parameters._ApplyGammaCorrection = false;
		parameters._TransformFunction = nullptr;
		parameters._BaseMipmapLevel = 0;
		parameters._MipmapLevels = 1;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildTexture2D(parameters);
	}

	{
		Texture2DBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_11_Texture2D";
		parameters._ID = "Blue_Noise_11_Texture2D";
		parameters._DefaultWidth = 0;
		parameters._DefaultHeight = 0;
		parameters._File1 = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Textures\\Blue_Noise_Textures\\HDR_RGBA_11.png";
		parameters._File2 = nullptr;
		parameters._File3 = nullptr;
		parameters._File4 = nullptr;
		parameters._Default = Vector4<float32>(0.0f, 0.0f, 0.0f, 0.0f);
		parameters._ChannelMappings[0] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::RED);
		parameters._ChannelMappings[1] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::GREEN);
		parameters._ChannelMappings[2] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::BLUE);
		parameters._ChannelMappings[3] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::ALPHA);
		parameters._ApplyGammaCorrection = false;
		parameters._TransformFunction = nullptr;
		parameters._BaseMipmapLevel = 0;
		parameters._MipmapLevels = 1;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildTexture2D(parameters);
	}

	{
		Texture2DBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_12_Texture2D";
		parameters._ID = "Blue_Noise_12_Texture2D";
		parameters._DefaultWidth = 0;
		parameters._DefaultHeight = 0;
		parameters._File1 = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Textures\\Blue_Noise_Textures\\HDR_RGBA_12.png";
		parameters._File2 = nullptr;
		parameters._File3 = nullptr;
		parameters._File4 = nullptr;
		parameters._Default = Vector4<float32>(0.0f, 0.0f, 0.0f, 0.0f);
		parameters._ChannelMappings[0] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::RED);
		parameters._ChannelMappings[1] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::GREEN);
		parameters._ChannelMappings[2] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::BLUE);
		parameters._ChannelMappings[3] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::ALPHA);
		parameters._ApplyGammaCorrection = false;
		parameters._TransformFunction = nullptr;
		parameters._BaseMipmapLevel = 0;
		parameters._MipmapLevels = 1;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildTexture2D(parameters);
	}

	{
		Texture2DBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_13_Texture2D";
		parameters._ID = "Blue_Noise_13_Texture2D";
		parameters._DefaultWidth = 0;
		parameters._DefaultHeight = 0;
		parameters._File1 = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Textures\\Blue_Noise_Textures\\HDR_RGBA_13.png";
		parameters._File2 = nullptr;
		parameters._File3 = nullptr;
		parameters._File4 = nullptr;
		parameters._Default = Vector4<float32>(0.0f, 0.0f, 0.0f, 0.0f);
		parameters._ChannelMappings[0] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::RED);
		parameters._ChannelMappings[1] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::GREEN);
		parameters._ChannelMappings[2] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::BLUE);
		parameters._ChannelMappings[3] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::ALPHA);
		parameters._ApplyGammaCorrection = false;
		parameters._TransformFunction = nullptr;
		parameters._BaseMipmapLevel = 0;
		parameters._MipmapLevels = 1;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildTexture2D(parameters);
	}

	{
		Texture2DBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_14_Texture2D";
		parameters._ID = "Blue_Noise_14_Texture2D";
		parameters._DefaultWidth = 0;
		parameters._DefaultHeight = 0;
		parameters._File1 = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Textures\\Blue_Noise_Textures\\HDR_RGBA_14.png";
		parameters._File2 = nullptr;
		parameters._File3 = nullptr;
		parameters._File4 = nullptr;
		parameters._Default = Vector4<float32>(0.0f, 0.0f, 0.0f, 0.0f);
		parameters._ChannelMappings[0] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::RED);
		parameters._ChannelMappings[1] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::GREEN);
		parameters._ChannelMappings[2] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::BLUE);
		parameters._ChannelMappings[3] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::ALPHA);
		parameters._ApplyGammaCorrection = false;
		parameters._TransformFunction = nullptr;
		parameters._BaseMipmapLevel = 0;
		parameters._MipmapLevels = 1;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildTexture2D(parameters);
	}

	{
		Texture2DBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_15_Texture2D";
		parameters._ID = "Blue_Noise_15_Texture2D";
		parameters._DefaultWidth = 0;
		parameters._DefaultHeight = 0;
		parameters._File1 = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Textures\\Blue_Noise_Textures\\HDR_RGBA_15.png";
		parameters._File2 = nullptr;
		parameters._File3 = nullptr;
		parameters._File4 = nullptr;
		parameters._Default = Vector4<float32>(0.0f, 0.0f, 0.0f, 0.0f);
		parameters._ChannelMappings[0] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::RED);
		parameters._ChannelMappings[1] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::GREEN);
		parameters._ChannelMappings[2] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::BLUE);
		parameters._ChannelMappings[3] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::ALPHA);
		parameters._ApplyGammaCorrection = false;
		parameters._TransformFunction = nullptr;
		parameters._BaseMipmapLevel = 0;
		parameters._MipmapLevels = 1;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildTexture2D(parameters);
	}

	{
		Texture2DBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_16_Texture2D";
		parameters._ID = "Blue_Noise_16_Texture2D";
		parameters._DefaultWidth = 0;
		parameters._DefaultHeight = 0;
		parameters._File1 = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Textures\\Blue_Noise_Textures\\HDR_RGBA_16.png";
		parameters._File2 = nullptr;
		parameters._File3 = nullptr;
		parameters._File4 = nullptr;
		parameters._Default = Vector4<float32>(0.0f, 0.0f, 0.0f, 0.0f);
		parameters._ChannelMappings[0] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::RED);
		parameters._ChannelMappings[1] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::GREEN);
		parameters._ChannelMappings[2] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::BLUE);
		parameters._ChannelMappings[3] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::ALPHA);
		parameters._ApplyGammaCorrection = false;
		parameters._TransformFunction = nullptr;
		parameters._BaseMipmapLevel = 0;
		parameters._MipmapLevels = 1;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildTexture2D(parameters);
	}

	{
		Texture2DBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_17_Texture2D";
		parameters._ID = "Blue_Noise_17_Texture2D";
		parameters._DefaultWidth = 0;
		parameters._DefaultHeight = 0;
		parameters._File1 = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Textures\\Blue_Noise_Textures\\HDR_RGBA_17.png";
		parameters._File2 = nullptr;
		parameters._File3 = nullptr;
		parameters._File4 = nullptr;
		parameters._Default = Vector4<float32>(0.0f, 0.0f, 0.0f, 0.0f);
		parameters._ChannelMappings[0] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::RED);
		parameters._ChannelMappings[1] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::GREEN);
		parameters._ChannelMappings[2] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::BLUE);
		parameters._ChannelMappings[3] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::ALPHA);
		parameters._ApplyGammaCorrection = false;
		parameters._TransformFunction = nullptr;
		parameters._BaseMipmapLevel = 0;
		parameters._MipmapLevels = 1;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildTexture2D(parameters);
	}

	{
		Texture2DBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_18_Texture2D";
		parameters._ID = "Blue_Noise_18_Texture2D";
		parameters._DefaultWidth = 0;
		parameters._DefaultHeight = 0;
		parameters._File1 = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Textures\\Blue_Noise_Textures\\HDR_RGBA_18.png";
		parameters._File2 = nullptr;
		parameters._File3 = nullptr;
		parameters._File4 = nullptr;
		parameters._Default = Vector4<float32>(0.0f, 0.0f, 0.0f, 0.0f);
		parameters._ChannelMappings[0] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::RED);
		parameters._ChannelMappings[1] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::GREEN);
		parameters._ChannelMappings[2] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::BLUE);
		parameters._ChannelMappings[3] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::ALPHA);
		parameters._ApplyGammaCorrection = false;
		parameters._TransformFunction = nullptr;
		parameters._BaseMipmapLevel = 0;
		parameters._MipmapLevels = 1;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildTexture2D(parameters);
	}

	{
		Texture2DBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_19_Texture2D";
		parameters._ID = "Blue_Noise_19_Texture2D";
		parameters._DefaultWidth = 0;
		parameters._DefaultHeight = 0;
		parameters._File1 = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Textures\\Blue_Noise_Textures\\HDR_RGBA_19.png";
		parameters._File2 = nullptr;
		parameters._File3 = nullptr;
		parameters._File4 = nullptr;
		parameters._Default = Vector4<float32>(0.0f, 0.0f, 0.0f, 0.0f);
		parameters._ChannelMappings[0] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::RED);
		parameters._ChannelMappings[1] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::GREEN);
		parameters._ChannelMappings[2] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::BLUE);
		parameters._ChannelMappings[3] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::ALPHA);
		parameters._ApplyGammaCorrection = false;
		parameters._TransformFunction = nullptr;
		parameters._BaseMipmapLevel = 0;
		parameters._MipmapLevels = 1;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildTexture2D(parameters);
	}

	{
		Texture2DBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_20_Texture2D";
		parameters._ID = "Blue_Noise_20_Texture2D";
		parameters._DefaultWidth = 0;
		parameters._DefaultHeight = 0;
		parameters._File1 = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Textures\\Blue_Noise_Textures\\HDR_RGBA_0.png";
		parameters._File2 = nullptr;
		parameters._File3 = nullptr;
		parameters._File4 = nullptr;
		parameters._Default = Vector4<float32>(0.0f, 0.0f, 0.0f, 0.0f);
		parameters._ChannelMappings[0] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::RED);
		parameters._ChannelMappings[1] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::GREEN);
		parameters._ChannelMappings[2] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::BLUE);
		parameters._ChannelMappings[3] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::ALPHA);
		parameters._ApplyGammaCorrection = false;
		parameters._TransformFunction = nullptr;
		parameters._BaseMipmapLevel = 0;
		parameters._MipmapLevels = 1;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildTexture2D(parameters);
	}

	{
		Texture2DBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_21_Texture2D";
		parameters._ID = "Blue_Noise_21_Texture2D";
		parameters._DefaultWidth = 0;
		parameters._DefaultHeight = 0;
		parameters._File1 = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Textures\\Blue_Noise_Textures\\HDR_RGBA_21.png";
		parameters._File2 = nullptr;
		parameters._File3 = nullptr;
		parameters._File4 = nullptr;
		parameters._Default = Vector4<float32>(0.0f, 0.0f, 0.0f, 0.0f);
		parameters._ChannelMappings[0] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::RED);
		parameters._ChannelMappings[1] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::GREEN);
		parameters._ChannelMappings[2] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::BLUE);
		parameters._ChannelMappings[3] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::ALPHA);
		parameters._ApplyGammaCorrection = false;
		parameters._TransformFunction = nullptr;
		parameters._BaseMipmapLevel = 0;
		parameters._MipmapLevels = 1;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildTexture2D(parameters);
	}

	{
		Texture2DBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_22_Texture2D";
		parameters._ID = "Blue_Noise_22_Texture2D";
		parameters._DefaultWidth = 0;
		parameters._DefaultHeight = 0;
		parameters._File1 = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Textures\\Blue_Noise_Textures\\HDR_RGBA_22.png";
		parameters._File2 = nullptr;
		parameters._File3 = nullptr;
		parameters._File4 = nullptr;
		parameters._Default = Vector4<float32>(0.0f, 0.0f, 0.0f, 0.0f);
		parameters._ChannelMappings[0] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::RED);
		parameters._ChannelMappings[1] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::GREEN);
		parameters._ChannelMappings[2] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::BLUE);
		parameters._ChannelMappings[3] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::ALPHA);
		parameters._ApplyGammaCorrection = false;
		parameters._TransformFunction = nullptr;
		parameters._BaseMipmapLevel = 0;
		parameters._MipmapLevels = 1;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildTexture2D(parameters);
	}

	{
		Texture2DBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_23_Texture2D";
		parameters._ID = "Blue_Noise_23_Texture2D";
		parameters._DefaultWidth = 0;
		parameters._DefaultHeight = 0;
		parameters._File1 = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Textures\\Blue_Noise_Textures\\HDR_RGBA_23.png";
		parameters._File2 = nullptr;
		parameters._File3 = nullptr;
		parameters._File4 = nullptr;
		parameters._Default = Vector4<float32>(0.0f, 0.0f, 0.0f, 0.0f);
		parameters._ChannelMappings[0] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::RED);
		parameters._ChannelMappings[1] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::GREEN);
		parameters._ChannelMappings[2] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::BLUE);
		parameters._ChannelMappings[3] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::ALPHA);
		parameters._ApplyGammaCorrection = false;
		parameters._TransformFunction = nullptr;
		parameters._BaseMipmapLevel = 0;
		parameters._MipmapLevels = 1;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildTexture2D(parameters);
	}

	{
		Texture2DBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_24_Texture2D";
		parameters._ID = "Blue_Noise_24_Texture2D";
		parameters._DefaultWidth = 0;
		parameters._DefaultHeight = 0;
		parameters._File1 = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Textures\\Blue_Noise_Textures\\HDR_RGBA_24.png";
		parameters._File2 = nullptr;
		parameters._File3 = nullptr;
		parameters._File4 = nullptr;
		parameters._Default = Vector4<float32>(0.0f, 0.0f, 0.0f, 0.0f);
		parameters._ChannelMappings[0] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::RED);
		parameters._ChannelMappings[1] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::GREEN);
		parameters._ChannelMappings[2] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::BLUE);
		parameters._ChannelMappings[3] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::ALPHA);
		parameters._ApplyGammaCorrection = false;
		parameters._TransformFunction = nullptr;
		parameters._BaseMipmapLevel = 0;
		parameters._MipmapLevels = 1;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildTexture2D(parameters);
	}

	{
		Texture2DBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_25_Texture2D";
		parameters._ID = "Blue_Noise_25_Texture2D";
		parameters._DefaultWidth = 0;
		parameters._DefaultHeight = 0;
		parameters._File1 = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Textures\\Blue_Noise_Textures\\HDR_RGBA_25.png";
		parameters._File2 = nullptr;
		parameters._File3 = nullptr;
		parameters._File4 = nullptr;
		parameters._Default = Vector4<float32>(0.0f, 0.0f, 0.0f, 0.0f);
		parameters._ChannelMappings[0] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::RED);
		parameters._ChannelMappings[1] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::GREEN);
		parameters._ChannelMappings[2] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::BLUE);
		parameters._ChannelMappings[3] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::ALPHA);
		parameters._ApplyGammaCorrection = false;
		parameters._TransformFunction = nullptr;
		parameters._BaseMipmapLevel = 0;
		parameters._MipmapLevels = 1;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildTexture2D(parameters);
	}

	{
		Texture2DBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_26_Texture2D";
		parameters._ID = "Blue_Noise_26_Texture2D";
		parameters._DefaultWidth = 0;
		parameters._DefaultHeight = 0;
		parameters._File1 = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Textures\\Blue_Noise_Textures\\HDR_RGBA_26.png";
		parameters._File2 = nullptr;
		parameters._File3 = nullptr;
		parameters._File4 = nullptr;
		parameters._Default = Vector4<float32>(0.0f, 0.0f, 0.0f, 0.0f);
		parameters._ChannelMappings[0] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::RED);
		parameters._ChannelMappings[1] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::GREEN);
		parameters._ChannelMappings[2] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::BLUE);
		parameters._ChannelMappings[3] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::ALPHA);
		parameters._ApplyGammaCorrection = false;
		parameters._TransformFunction = nullptr;
		parameters._BaseMipmapLevel = 0;
		parameters._MipmapLevels = 1;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildTexture2D(parameters);
	}

	{
		Texture2DBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_27_Texture2D";
		parameters._ID = "Blue_Noise_27_Texture2D";
		parameters._DefaultWidth = 0;
		parameters._DefaultHeight = 0;
		parameters._File1 = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Textures\\Blue_Noise_Textures\\HDR_RGBA_27.png";
		parameters._File2 = nullptr;
		parameters._File3 = nullptr;
		parameters._File4 = nullptr;
		parameters._Default = Vector4<float32>(0.0f, 0.0f, 0.0f, 0.0f);
		parameters._ChannelMappings[0] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::RED);
		parameters._ChannelMappings[1] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::GREEN);
		parameters._ChannelMappings[2] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::BLUE);
		parameters._ChannelMappings[3] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::ALPHA);
		parameters._ApplyGammaCorrection = false;
		parameters._TransformFunction = nullptr;
		parameters._BaseMipmapLevel = 0;
		parameters._MipmapLevels = 1;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildTexture2D(parameters);
	}

	{
		Texture2DBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_28_Texture2D";
		parameters._ID = "Blue_Noise_28_Texture2D";
		parameters._DefaultWidth = 0;
		parameters._DefaultHeight = 0;
		parameters._File1 = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Textures\\Blue_Noise_Textures\\HDR_RGBA_28.png";
		parameters._File2 = nullptr;
		parameters._File3 = nullptr;
		parameters._File4 = nullptr;
		parameters._Default = Vector4<float32>(0.0f, 0.0f, 0.0f, 0.0f);
		parameters._ChannelMappings[0] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::RED);
		parameters._ChannelMappings[1] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::GREEN);
		parameters._ChannelMappings[2] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::BLUE);
		parameters._ChannelMappings[3] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::ALPHA);
		parameters._ApplyGammaCorrection = false;
		parameters._TransformFunction = nullptr;
		parameters._BaseMipmapLevel = 0;
		parameters._MipmapLevels = 1;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildTexture2D(parameters);
	}

	{
		Texture2DBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_29_Texture2D";
		parameters._ID = "Blue_Noise_29_Texture2D";
		parameters._DefaultWidth = 0;
		parameters._DefaultHeight = 0;
		parameters._File1 = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Textures\\Blue_Noise_Textures\\HDR_RGBA_29.png";
		parameters._File2 = nullptr;
		parameters._File3 = nullptr;
		parameters._File4 = nullptr;
		parameters._Default = Vector4<float32>(0.0f, 0.0f, 0.0f, 0.0f);
		parameters._ChannelMappings[0] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::RED);
		parameters._ChannelMappings[1] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::GREEN);
		parameters._ChannelMappings[2] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::BLUE);
		parameters._ChannelMappings[3] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::ALPHA);
		parameters._ApplyGammaCorrection = false;
		parameters._TransformFunction = nullptr;
		parameters._BaseMipmapLevel = 0;
		parameters._MipmapLevels = 1;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildTexture2D(parameters);
	}

	{
		Texture2DBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_30_Texture2D";
		parameters._ID = "Blue_Noise_30_Texture2D";
		parameters._DefaultWidth = 0;
		parameters._DefaultHeight = 0;
		parameters._File1 = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Textures\\Blue_Noise_Textures\\HDR_RGBA_30.png";
		parameters._File2 = nullptr;
		parameters._File3 = nullptr;
		parameters._File4 = nullptr;
		parameters._Default = Vector4<float32>(0.0f, 0.0f, 0.0f, 0.0f);
		parameters._ChannelMappings[0] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::RED);
		parameters._ChannelMappings[1] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::GREEN);
		parameters._ChannelMappings[2] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::BLUE);
		parameters._ChannelMappings[3] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::ALPHA);
		parameters._ApplyGammaCorrection = false;
		parameters._TransformFunction = nullptr;
		parameters._BaseMipmapLevel = 0;
		parameters._MipmapLevels = 1;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildTexture2D(parameters);
	}

	{
		Texture2DBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Blue_Noise_31_Texture2D";
		parameters._ID = "Blue_Noise_31_Texture2D";
		parameters._DefaultWidth = 0;
		parameters._DefaultHeight = 0;
		parameters._File1 = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Textures\\Blue_Noise_Textures\\HDR_RGBA_31.png";
		parameters._File2 = nullptr;
		parameters._File3 = nullptr;
		parameters._File4 = nullptr;
		parameters._Default = Vector4<float32>(0.0f, 0.0f, 0.0f, 0.0f);
		parameters._ChannelMappings[0] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::RED);
		parameters._ChannelMappings[1] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::GREEN);
		parameters._ChannelMappings[2] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::BLUE);
		parameters._ChannelMappings[3] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::ALPHA);
		parameters._ApplyGammaCorrection = false;
		parameters._TransformFunction = nullptr;
		parameters._BaseMipmapLevel = 0;
		parameters._MipmapLevels = 1;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildTexture2D(parameters);
	}
#endif

#if BUILD_ENGINE_ALL || BUILD_ENGINE_SHADERS
	{
		tasks.Emplace(new (MemorySystem::Instance->TypeAllocate<Task>()) Task());
		Task &task{ *tasks.Back() };

		task._Function = [](void* const RESTRICT)
		{
			ShaderBuildParameters parameters;

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\AmbientOcclusionApplicationFragmentShader";
			parameters._ID = "AmbientOcclusionApplicationFragmentShader";
			parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\AmbientOcclusionApplicationFragmentShader.frag";
			parameters._Stage = ShaderStage::FRAGMENT;

			ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
		};
		task._Arguments = nullptr;
		task._ExecutableOnSameThread = false;

		TaskSystem::Instance->ExecuteTask(&task);
	}

	{
		tasks.Emplace(new (MemorySystem::Instance->TypeAllocate<Task>()) Task());
		Task &task{ *tasks.Back() };

		task._Function = [](void* const RESTRICT)
		{
			ShaderBuildParameters parameters;

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\AmbientOcclusionSpatialDenoisingFragmentShader";
			parameters._ID = "AmbientOcclusionSpatialDenoisingFragmentShader";
			parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\AmbientOcclusionSpatialDenoisingFragmentShader.frag";
			parameters._Stage = ShaderStage::FRAGMENT;

			ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
		};
		task._Arguments = nullptr;
		task._ExecutableOnSameThread = false;

		TaskSystem::Instance->ExecuteTask(&task);
	}

	{
		tasks.Emplace(new (MemorySystem::Instance->TypeAllocate<Task>()) Task());
		Task &task{ *tasks.Back() };

		task._Function = [](void* const RESTRICT)
		{
			ShaderBuildParameters parameters;

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\AmbientOcclusionTemporalDenoisingFragmentShader";
			parameters._ID = "AmbientOcclusionTemporalDenoisingFragmentShader";
			parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\AmbientOcclusionTemporalDenoisingFragmentShader.frag";
			parameters._Stage = ShaderStage::FRAGMENT;

			ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
		};
		task._Arguments = nullptr;
		task._ExecutableOnSameThread = false;

		TaskSystem::Instance->ExecuteTask(&task);
	}

	{
		tasks.Emplace(new (MemorySystem::Instance->TypeAllocate<Task>()) Task());
		Task &task{ *tasks.Back() };

		task._Function = [](void* const RESTRICT)
		{
			ShaderBuildParameters parameters;

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\AnimatedModelSceneFeaturesFragmentShader";
			parameters._ID = "AnimatedModelSceneFeaturesFragmentShader";
			parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\AnimatedModelSceneFeaturesFragmentShader.frag";
			parameters._Stage = ShaderStage::FRAGMENT;

			ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
		};
		task._Arguments = nullptr;
		task._ExecutableOnSameThread = false;

		TaskSystem::Instance->ExecuteTask(&task);
	}

	{
		tasks.Emplace(new (MemorySystem::Instance->TypeAllocate<Task>()) Task());
		Task &task{ *tasks.Back() };

		task._Function = [](void* const RESTRICT)
		{
			ShaderBuildParameters parameters;

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\AnimatedModelSceneFeaturesVertexShader";
			parameters._ID = "AnimatedModelSceneFeaturesVertexShader";
			parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\AnimatedModelSceneFeaturesVertexShader.vert";
			parameters._Stage = ShaderStage::VERTEX;

			ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
		};
		task._Arguments = nullptr;
		task._ExecutableOnSameThread = false;

		TaskSystem::Instance->ExecuteTask(&task);
	}

	{
		tasks.Emplace(new (MemorySystem::Instance->TypeAllocate<Task>()) Task());
		Task &task{ *tasks.Back() };

		task._Function = [](void* const RESTRICT)
		{
			ShaderBuildParameters parameters;

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\AxisAlignedBoundingBox3DVertexShader";
			parameters._ID = "AxisAlignedBoundingBox3DVertexShader";
			parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\AxisAlignedBoundingBox3DVertexShader.vert";
			parameters._Stage = ShaderStage::VERTEX;

			ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
		};
		task._Arguments = nullptr;
		task._ExecutableOnSameThread = false;

		TaskSystem::Instance->ExecuteTask(&task);
	}

	{
		tasks.Emplace(new (MemorySystem::Instance->TypeAllocate<Task>()) Task());
		Task &task{ *tasks.Back() };

		task._Function = [](void* const RESTRICT)
		{
			ShaderBuildParameters parameters;

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\BloomIsolationFragmentShader";
			parameters._ID = "BloomIsolationFragmentShader";
			parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\BloomIsolationFragmentShader.frag";
			parameters._Stage = ShaderStage::FRAGMENT;

			ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
		};
		task._Arguments = nullptr;
		task._ExecutableOnSameThread = false;

		TaskSystem::Instance->ExecuteTask(&task);
	}

	{
		tasks.Emplace(new (MemorySystem::Instance->TypeAllocate<Task>()) Task());
		Task &task{ *tasks.Back() };

		task._Function = [](void* const RESTRICT)
		{
			ShaderBuildParameters parameters;

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\DebugRenderFragmentShader";
			parameters._ID = "DebugRenderFragmentShader";
			parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\DebugRenderFragmentShader.frag";
			parameters._Stage = ShaderStage::FRAGMENT;

			ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
		};
		task._Arguments = nullptr;
		task._ExecutableOnSameThread = false;

		TaskSystem::Instance->ExecuteTask(&task);
	}

	{
		tasks.Emplace(new (MemorySystem::Instance->TypeAllocate<Task>()) Task());
		Task &task{ *tasks.Back() };

		task._Function = [](void* const RESTRICT)
		{
			ShaderBuildParameters parameters;

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\DepthOfFieldApplicationFragmentShader";
			parameters._ID = "DepthOfFieldApplicationFragmentShader";
			parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\DepthOfFieldApplicationFragmentShader.frag";
			parameters._Stage = ShaderStage::FRAGMENT;

			ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
		};
		task._Arguments = nullptr;
		task._ExecutableOnSameThread = false;

		TaskSystem::Instance->ExecuteTask(&task);
	}

	{
		tasks.Emplace(new (MemorySystem::Instance->TypeAllocate<Task>()) Task());
		Task &task{ *tasks.Back() };

		task._Function = [](void* const RESTRICT)
		{
			ShaderBuildParameters parameters;

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\DepthOfFieldBokehBlurFragmentShader";
			parameters._ID = "DepthOfFieldBokehBlurFragmentShader";
			parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\DepthOfFieldBokehBlurFragmentShader.frag";
			parameters._Stage = ShaderStage::FRAGMENT;

			ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
		};
		task._Arguments = nullptr;
		task._ExecutableOnSameThread = false;

		TaskSystem::Instance->ExecuteTask(&task);
	}

	{
		tasks.Emplace(new (MemorySystem::Instance->TypeAllocate<Task>()) Task());
		Task &task{ *tasks.Back() };

		task._Function = [](void* const RESTRICT)
		{
			ShaderBuildParameters parameters;

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\DirectLightingFragmentShader";
			parameters._ID = "DirectLightingFragmentShader";
			parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\DirectLightingFragmentShader.frag";
			parameters._Stage = ShaderStage::FRAGMENT;

			ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
		};
		task._Arguments = nullptr;
		task._ExecutableOnSameThread = false;

		TaskSystem::Instance->ExecuteTask(&task);
	}

	{
		tasks.Emplace(new (MemorySystem::Instance->TypeAllocate<Task>()) Task());
		Task &task{ *tasks.Back() };

		task._Function = [](void* const RESTRICT)
		{
			ShaderBuildParameters parameters;

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\DynamicModelRayClosestHitShader";
			parameters._ID = "DynamicModelRayClosestHitShader";
			parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\DynamicModelRayClosestHitShader.rchit";
			parameters._Stage = ShaderStage::RAY_CLOSEST_HIT;

			ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
		};
		task._Arguments = nullptr;
		task._ExecutableOnSameThread = false;

		TaskSystem::Instance->ExecuteTask(&task);
	}

	{
		tasks.Emplace(new (MemorySystem::Instance->TypeAllocate<Task>()) Task());
		Task &task{ *tasks.Back() };

		task._Function = [](void* const RESTRICT)
		{
			ShaderBuildParameters parameters;

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\EditorUserInterfaceFragmentShader";
			parameters._ID = "EditorUserInterfaceFragmentShader";
			parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\EditorUserInterfaceFragmentShader.frag";
			parameters._Stage = ShaderStage::FRAGMENT;

			ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
		};
		task._Arguments = nullptr;
		task._ExecutableOnSameThread = false;

		TaskSystem::Instance->ExecuteTask(&task);
	}

	{
		tasks.Emplace(new (MemorySystem::Instance->TypeAllocate<Task>()) Task());
		Task &task{ *tasks.Back() };

		task._Function = [](void* const RESTRICT)
		{
			ShaderBuildParameters parameters;

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\EditorUserInterfaceVertexShader";
			parameters._ID = "EditorUserInterfaceVertexShader";
			parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\EditorUserInterfaceVertexShader.vert";
			parameters._Stage = ShaderStage::VERTEX;

			ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
		};
		task._Arguments = nullptr;
		task._ExecutableOnSameThread = false;

		TaskSystem::Instance->ExecuteTask(&task);
	}

	{
		tasks.Emplace(new (MemorySystem::Instance->TypeAllocate<Task>()) Task());
		Task &task{ *tasks.Back() };

		task._Function = [](void* const RESTRICT)
		{
			ShaderBuildParameters parameters;

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\FastApproximateAntiAliasingFragmentShader";
			parameters._ID = "FastApproximateAntiAliasingFragmentShader";
			parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\FastApproximateAntiAliasingFragmentShader.frag";
			parameters._Stage = ShaderStage::FRAGMENT;

			ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
		};
		task._Arguments = nullptr;
		task._ExecutableOnSameThread = false;

		TaskSystem::Instance->ExecuteTask(&task);
	}

	{
		tasks.Emplace(new (MemorySystem::Instance->TypeAllocate<Task>()) Task());
		Task &task{ *tasks.Back() };

		task._Function = [](void* const RESTRICT)
		{
			ShaderBuildParameters parameters;

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\IndirectLightingApplicationFragmentShader";
			parameters._ID = "IndirectLightingApplicationFragmentShader";
			parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\IndirectLightingApplicationFragmentShader.frag";
			parameters._Stage = ShaderStage::FRAGMENT;

			ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
		};
		task._Arguments = nullptr;
		task._ExecutableOnSameThread = false;

		TaskSystem::Instance->ExecuteTask(&task);
	}

	{
		tasks.Emplace(new (MemorySystem::Instance->TypeAllocate<Task>()) Task());
		Task &task{ *tasks.Back() };

		task._Function = [](void* const RESTRICT)
		{
			ShaderBuildParameters parameters;

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\IndirectLightingRayGenerationShader";
			parameters._ID = "IndirectLightingRayGenerationShader";
			parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\IndirectLightingRayGenerationShader.rgen";
			parameters._Stage = ShaderStage::RAY_GENERATION;

			ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
		};
		task._Arguments = nullptr;
		task._ExecutableOnSameThread = false;

		TaskSystem::Instance->ExecuteTask(&task);
	}

	{
		tasks.Emplace(new (MemorySystem::Instance->TypeAllocate<Task>()) Task());
		Task &task{ *tasks.Back() };

		task._Function = [](void* const RESTRICT)
		{
			ShaderBuildParameters parameters;

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\IndirectLightingSpatialDenoisingFragmentShader";
			parameters._ID = "IndirectLightingSpatialDenoisingFragmentShader";
			parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\IndirectLightingSpatialDenoisingFragmentShader.frag";
			parameters._Stage = ShaderStage::FRAGMENT;

			ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
		};
		task._Arguments = nullptr;
		task._ExecutableOnSameThread = false;

		TaskSystem::Instance->ExecuteTask(&task);
	}

	{
		tasks.Emplace(new (MemorySystem::Instance->TypeAllocate<Task>()) Task());
		Task &task{ *tasks.Back() };

		task._Function = [](void* const RESTRICT)
		{
			ShaderBuildParameters parameters;

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\IndirectLightingTemporalDenoisingFragmentShader";
			parameters._ID = "IndirectLightingTemporalDenoisingFragmentShader";
			parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\IndirectLightingTemporalDenoisingFragmentShader.frag";
			parameters._Stage = ShaderStage::FRAGMENT;

			ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
		};
		task._Arguments = nullptr;
		task._ExecutableOnSameThread = false;

		TaskSystem::Instance->ExecuteTask(&task);
	}

	{
		tasks.Emplace(new (MemorySystem::Instance->TypeAllocate<Task>()) Task());
		Task &task{ *tasks.Back() };

		task._Function = [](void* const RESTRICT)
		{
			ShaderBuildParameters parameters;

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\ModelSceneFeaturesFragmentShader";
			parameters._ID = "ModelSceneFeaturesFragmentShader";
			parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\ModelSceneFeaturesFragmentShader.frag";
			parameters._Stage = ShaderStage::FRAGMENT;

			ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
		};
		task._Arguments = nullptr;
		task._ExecutableOnSameThread = false;

		TaskSystem::Instance->ExecuteTask(&task);
	}

	{
		tasks.Emplace(new (MemorySystem::Instance->TypeAllocate<Task>()) Task());
		Task &task{ *tasks.Back() };

		task._Function = [](void* const RESTRICT)
		{
			ShaderBuildParameters parameters;

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\ModelSceneFeaturesVertexShader";
			parameters._ID = "ModelSceneFeaturesVertexShader";
			parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\ModelSceneFeaturesVertexShader.vert";
			parameters._Stage = ShaderStage::VERTEX;

			ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
		};
		task._Arguments = nullptr;
		task._ExecutableOnSameThread = false;

		TaskSystem::Instance->ExecuteTask(&task);
	}

	{
		tasks.Emplace(new (MemorySystem::Instance->TypeAllocate<Task>()) Task());
		Task &task{ *tasks.Back() };

		task._Function = [](void* const RESTRICT)
		{
			ShaderBuildParameters parameters;

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\ModelShadowMapFragmentShader";
			parameters._ID = "ModelShadowMapFragmentShader";
			parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\ModelShadowMapFragmentShader.frag";
			parameters._Stage = ShaderStage::FRAGMENT;

			ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
		};
		task._Arguments = nullptr;
		task._ExecutableOnSameThread = false;

		TaskSystem::Instance->ExecuteTask(&task);
	}

	{
		tasks.Emplace(new (MemorySystem::Instance->TypeAllocate<Task>()) Task());
		Task &task{ *tasks.Back() };

		task._Function = [](void* const RESTRICT)
		{
			ShaderBuildParameters parameters;

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\ModelShadowMapVertexShader";
			parameters._ID = "ModelShadowMapVertexShader";
			parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\ModelShadowMapVertexShader.vert";
			parameters._Stage = ShaderStage::VERTEX;

			ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
		};
		task._Arguments = nullptr;
		task._ExecutableOnSameThread = false;

		TaskSystem::Instance->ExecuteTask(&task);
	}

	{
		tasks.Emplace(new (MemorySystem::Instance->TypeAllocate<Task>()) Task());
		Task &task{ *tasks.Back() };

		task._Function = [](void* const RESTRICT)
		{
			ShaderBuildParameters parameters;

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\MotionBlurFragmentShader";
			parameters._ID = "MotionBlurFragmentShader";
			parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\MotionBlurFragmentShader.frag";
			parameters._Stage = ShaderStage::FRAGMENT;

			ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
		};
		task._Arguments = nullptr;
		task._ExecutableOnSameThread = false;

		TaskSystem::Instance->ExecuteTask(&task);
	}

	{
		tasks.Emplace(new (MemorySystem::Instance->TypeAllocate<Task>()) Task());
		Task &task{ *tasks.Back() };

		task._Function = [](void* const RESTRICT)
		{
			ShaderBuildParameters parameters;

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\OceanSceneFeaturesFragmentShader";
			parameters._ID = "OceanSceneFeaturesFragmentShader";
			parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\OceanSceneFeaturesFragmentShader.frag";
			parameters._Stage = ShaderStage::FRAGMENT;

			ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
		};
		task._Arguments = nullptr;
		task._ExecutableOnSameThread = false;

		TaskSystem::Instance->ExecuteTask(&task);
	}

	{
		tasks.Emplace(new (MemorySystem::Instance->TypeAllocate<Task>()) Task());
		Task &task{ *tasks.Back() };

		task._Function = [](void* const RESTRICT)
		{
			ShaderBuildParameters parameters;

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\OceanSceneFeaturesVertexShader";
			parameters._ID = "OceanSceneFeaturesVertexShader";
			parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\OceanSceneFeaturesVertexShader.vert";
			parameters._Stage = ShaderStage::VERTEX;

			ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
		};
		task._Arguments = nullptr;
		task._ExecutableOnSameThread = false;

		TaskSystem::Instance->ExecuteTask(&task);
	}

	{
		tasks.Emplace(new (MemorySystem::Instance->TypeAllocate<Task>()) Task());
		Task &task{ *tasks.Back() };

		task._Function = [](void* const RESTRICT)
		{
			ShaderBuildParameters parameters;

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\ParticleSystemComputeShader";
			parameters._ID = "ParticleSystemComputeShader";
			parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\ParticleSystemComputeShader.comp";
			parameters._Stage = ShaderStage::COMPUTE;

			ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
		};
		task._Arguments = nullptr;
		task._ExecutableOnSameThread = false;

		TaskSystem::Instance->ExecuteTask(&task);
	}

	{
		tasks.Emplace(new (MemorySystem::Instance->TypeAllocate<Task>()) Task());
		Task &task{ *tasks.Back() };

		task._Function = [](void* const RESTRICT)
		{
			ShaderBuildParameters parameters;

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\ParticleSystemMaskedColorSceneFeaturesFragmentShader";
			parameters._ID = "ParticleSystemMaskedColorSceneFeaturesFragmentShader";
			parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\ParticleSystemMaskedColorSceneFeaturesFragmentShader.frag";
			parameters._Stage = ShaderStage::FRAGMENT;

			ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
		};
		task._Arguments = nullptr;
		task._ExecutableOnSameThread = false;

		TaskSystem::Instance->ExecuteTask(&task);
	}

	{
		tasks.Emplace(new (MemorySystem::Instance->TypeAllocate<Task>()) Task());
		Task &task{ *tasks.Back() };

		task._Function = [](void* const RESTRICT)
		{
			ShaderBuildParameters parameters;

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\ParticleSystemMaskedColorSceneFeaturesGeometryShader";
			parameters._ID = "ParticleSystemMaskedColorSceneFeaturesGeometryShader";
			parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\ParticleSystemMaskedColorSceneFeaturesGeometryShader.geom";
			parameters._Stage = ShaderStage::GEOMETRY;

			ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
		};
		task._Arguments = nullptr;
		task._ExecutableOnSameThread = false;

		TaskSystem::Instance->ExecuteTask(&task);
	}

	{
		tasks.Emplace(new (MemorySystem::Instance->TypeAllocate<Task>()) Task());
		Task &task{ *tasks.Back() };

		task._Function = [](void* const RESTRICT)
		{
			ShaderBuildParameters parameters;

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\ParticleSystemMaskedColorSceneFeaturesVertexShader";
			parameters._ID = "ParticleSystemMaskedColorSceneFeaturesVertexShader";
			parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\ParticleSystemMaskedColorSceneFeaturesVertexShader.vert";
			parameters._Stage = ShaderStage::VERTEX;

			ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
		};
		task._Arguments = nullptr;
		task._ExecutableOnSameThread = false;

		TaskSystem::Instance->ExecuteTask(&task);
	}

	{
		tasks.Emplace(new (MemorySystem::Instance->TypeAllocate<Task>()) Task());
		Task &task{ *tasks.Back() };

		task._Function = [](void* const RESTRICT)
		{
			ShaderBuildParameters parameters;

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\ParticleSystemMaskedDepthSceneFeaturesFragmentShader";
			parameters._ID = "ParticleSystemMaskedDepthSceneFeaturesFragmentShader";
			parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\ParticleSystemMaskedDepthSceneFeaturesFragmentShader.frag";
			parameters._Stage = ShaderStage::FRAGMENT;

			ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
		};
		task._Arguments = nullptr;
		task._ExecutableOnSameThread = false;

		TaskSystem::Instance->ExecuteTask(&task);
	}

	{
		tasks.Emplace(new (MemorySystem::Instance->TypeAllocate<Task>()) Task());
		Task &task{ *tasks.Back() };

		task._Function = [](void* const RESTRICT)
		{
			ShaderBuildParameters parameters;

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\ParticleSystemMaskedDepthSceneFeaturesGeometryShader";
			parameters._ID = "ParticleSystemMaskedDepthSceneFeaturesGeometryShader";
			parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\ParticleSystemMaskedDepthSceneFeaturesGeometryShader.geom";
			parameters._Stage = ShaderStage::GEOMETRY;

			ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
		};
		task._Arguments = nullptr;
		task._ExecutableOnSameThread = false;

		TaskSystem::Instance->ExecuteTask(&task);
	}

	{
		tasks.Emplace(new (MemorySystem::Instance->TypeAllocate<Task>()) Task());
		Task &task{ *tasks.Back() };

		task._Function = [](void* const RESTRICT)
		{
			ShaderBuildParameters parameters;

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\ParticleSystemMaskedDepthSceneFeaturesVertexShader";
			parameters._ID = "ParticleSystemMaskedDepthSceneFeaturesVertexShader";
			parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\ParticleSystemMaskedDepthSceneFeaturesVertexShader.vert";
			parameters._Stage = ShaderStage::VERTEX;

			ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
		};
		task._Arguments = nullptr;
		task._ExecutableOnSameThread = false;

		TaskSystem::Instance->ExecuteTask(&task);
	}

	{
		tasks.Emplace(new (MemorySystem::Instance->TypeAllocate<Task>()) Task());
		Task &task{ *tasks.Back() };

		task._Function = [](void* const RESTRICT)
		{
			ShaderBuildParameters parameters;

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\PassthroughFragmentShader";
			parameters._ID = "PassthroughFragmentShader";
			parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\PassthroughFragmentShader.frag";
			parameters._Stage = ShaderStage::FRAGMENT;

			ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
		};
		task._Arguments = nullptr;
		task._ExecutableOnSameThread = false;

		TaskSystem::Instance->ExecuteTask(&task);
	}

	{
		tasks.Emplace(new (MemorySystem::Instance->TypeAllocate<Task>()) Task());
		Task &task{ *tasks.Back() };

		task._Function = [](void* const RESTRICT)
		{
			ShaderBuildParameters parameters;

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\PathTracingRayGenerationShader";
			parameters._ID = "PathTracingRayGenerationShader";
			parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\PathTracingRayGenerationShader.rgen";
			parameters._Stage = ShaderStage::RAY_GENERATION;

			ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
		};
		task._Arguments = nullptr;
		task._ExecutableOnSameThread = false;

		TaskSystem::Instance->ExecuteTask(&task);
	}

	{
		tasks.Emplace(new (MemorySystem::Instance->TypeAllocate<Task>()) Task());
		Task &task{ *tasks.Back() };

		task._Function = [](void* const RESTRICT)
		{
			ShaderBuildParameters parameters;

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\PathTracingRayMissShader";
			parameters._ID = "PathTracingRayMissShader";
			parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\PathTracingRayMissShader.rmiss";
			parameters._Stage = ShaderStage::RAY_MISS;

			ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
		};
		task._Arguments = nullptr;
		task._ExecutableOnSameThread = false;

		TaskSystem::Instance->ExecuteTask(&task);
	}

	{
		tasks.Emplace(new (MemorySystem::Instance->TypeAllocate<Task>()) Task());
		Task &task{ *tasks.Back() };

		task._Function = [](void* const RESTRICT)
		{
			ShaderBuildParameters parameters;

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\PostProcessingFragmentShader";
			parameters._ID = "PostProcessingFragmentShader";
			parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\PostProcessingFragmentShader.frag";
			parameters._Stage = ShaderStage::FRAGMENT;

			ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
		};
		task._Arguments = nullptr;
		task._ExecutableOnSameThread = false;

		TaskSystem::Instance->ExecuteTask(&task);
	}

	{
		tasks.Emplace(new (MemorySystem::Instance->TypeAllocate<Task>()) Task());
		Task &task{ *tasks.Back() };

		task._Function = [](void* const RESTRICT)
		{
			ShaderBuildParameters parameters;

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\RasterizedShadowsFragmentShader";
			parameters._ID = "RasterizedShadowsFragmentShader";
			parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\RasterizedShadowsFragmentShader.frag";
			parameters._Stage = ShaderStage::FRAGMENT;

			ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
		};
		task._Arguments = nullptr;
		task._ExecutableOnSameThread = false;

		TaskSystem::Instance->ExecuteTask(&task);
	}

	{
		tasks.Emplace(new (MemorySystem::Instance->TypeAllocate<Task>()) Task());
		Task &task{ *tasks.Back() };

		task._Function = [](void* const RESTRICT)
		{
			ShaderBuildParameters parameters;

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\RayTracedAmbientOcclusionRayGenerationShader";
			parameters._ID = "RayTracedAmbientOcclusionRayGenerationShader";
			parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\RayTracedAmbientOcclusionRayGenerationShader.rgen";
			parameters._Stage = ShaderStage::RAY_GENERATION;

			ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
		};
		task._Arguments = nullptr;
		task._ExecutableOnSameThread = false;

		TaskSystem::Instance->ExecuteTask(&task);
	}

	{
		tasks.Emplace(new (MemorySystem::Instance->TypeAllocate<Task>()) Task());
		Task &task{ *tasks.Back() };

		task._Function = [](void* const RESTRICT)
		{
			ShaderBuildParameters parameters;

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\RayTracedShadowsRayGenerationShader";
			parameters._ID = "RayTracedShadowsRayGenerationShader";
			parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\RayTracedShadowsRayGenerationShader.rgen";
			parameters._Stage = ShaderStage::RAY_GENERATION;

			ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
		};
		task._Arguments = nullptr;
		task._ExecutableOnSameThread = false;

		TaskSystem::Instance->ExecuteTask(&task);
	}

	{
		tasks.Emplace(new (MemorySystem::Instance->TypeAllocate<Task>()) Task());
		Task &task{ *tasks.Back() };

		task._Function = [](void* const RESTRICT)
		{
			ShaderBuildParameters parameters;

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\RenderingReferenceFragmentShader";
			parameters._ID = "RenderingReferenceFragmentShader";
			parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\RenderingReferenceFragmentShader.frag";
			parameters._Stage = ShaderStage::FRAGMENT;

			ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
		};
		task._Arguments = nullptr;
		task._ExecutableOnSameThread = false;

		TaskSystem::Instance->ExecuteTask(&task);
	}

	{
		tasks.Emplace(new (MemorySystem::Instance->TypeAllocate<Task>()) Task());
		Task &task{ *tasks.Back() };

		task._Function = [](void* const RESTRICT)
		{
			ShaderBuildParameters parameters;

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\ResampleFragmentShader";
			parameters._ID = "ResampleFragmentShader";
			parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\ResampleFragmentShader.frag";
			parameters._Stage = ShaderStage::FRAGMENT;

			ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
		};
		task._Arguments = nullptr;
		task._ExecutableOnSameThread = false;

		TaskSystem::Instance->ExecuteTask(&task);
	}

	{
		tasks.Emplace(new (MemorySystem::Instance->TypeAllocate<Task>()) Task());
		Task &task{ *tasks.Back() };

		task._Function = [](void* const RESTRICT)
		{
			ShaderBuildParameters parameters;

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\SceneFeaturesDownsampleFragmentShader";
			parameters._ID = "SceneFeaturesDownsampleFragmentShader";
			parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\SceneFeaturesDownsampleFragmentShader.frag";
			parameters._Stage = ShaderStage::FRAGMENT;

			ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
		};
		task._Arguments = nullptr;
		task._ExecutableOnSameThread = false;

		TaskSystem::Instance->ExecuteTask(&task);
	}

	{
		tasks.Emplace(new (MemorySystem::Instance->TypeAllocate<Task>()) Task());
		Task &task{ *tasks.Back() };

		task._Function = [](void* const RESTRICT)
		{
			ShaderBuildParameters parameters;

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\ScreenSpaceAmbientOcclusionFragmentShader";
			parameters._ID = "ScreenSpaceAmbientOcclusionFragmentShader";
			parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\ScreenSpaceAmbientOcclusionFragmentShader.frag";
			parameters._Stage = ShaderStage::FRAGMENT;

			ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
		};
		task._Arguments = nullptr;
		task._ExecutableOnSameThread = false;

		TaskSystem::Instance->ExecuteTask(&task);
	}

	{
		tasks.Emplace(new (MemorySystem::Instance->TypeAllocate<Task>()) Task());
		Task &task{ *tasks.Back() };

		task._Function = [](void* const RESTRICT)
		{
			ShaderBuildParameters parameters;

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\ScreenSpaceIndirectLightingFragmentShader";
			parameters._ID = "ScreenSpaceIndirectLightingFragmentShader";
			parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\ScreenSpaceIndirectLightingFragmentShader.frag";
			parameters._Stage = ShaderStage::FRAGMENT;

			ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
		};
		task._Arguments = nullptr;
		task._ExecutableOnSameThread = false;

		TaskSystem::Instance->ExecuteTask(&task);
	}

	{
		tasks.Emplace(new (MemorySystem::Instance->TypeAllocate<Task>()) Task());
		Task &task{ *tasks.Back() };

		task._Function = [](void* const RESTRICT)
		{
			ShaderBuildParameters parameters;

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\ShadowsSpatialDenoisingFragmentShader";
			parameters._ID = "ShadowsSpatialDenoisingFragmentShader";
			parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\ShadowsSpatialDenoisingFragmentShader.frag";
			parameters._Stage = ShaderStage::FRAGMENT;

			ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
		};
		task._Arguments = nullptr;
		task._ExecutableOnSameThread = false;

		TaskSystem::Instance->ExecuteTask(&task);
	}

	{
		tasks.Emplace(new (MemorySystem::Instance->TypeAllocate<Task>()) Task());
		Task &task{ *tasks.Back() };

		task._Function = [](void* const RESTRICT)
		{
			ShaderBuildParameters parameters;

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\SkyFragmentShader";
			parameters._ID = "SkyFragmentShader";
			parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\SkyFragmentShader.frag";
			parameters._Stage = ShaderStage::FRAGMENT;

			ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
		};
		task._Arguments = nullptr;
		task._ExecutableOnSameThread = false;

		TaskSystem::Instance->ExecuteTask(&task);
	}

	{
		tasks.Emplace(new (MemorySystem::Instance->TypeAllocate<Task>()) Task());
		Task &task{ *tasks.Back() };

		task._Function = [](void* const RESTRICT)
		{
			ShaderBuildParameters parameters;

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\SphereVertexShader";
			parameters._ID = "SphereVertexShader";
			parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\SphereVertexShader.vert";
			parameters._Stage = ShaderStage::VERTEX;

			ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
		};
		task._Arguments = nullptr;
		task._ExecutableOnSameThread = false;

		TaskSystem::Instance->ExecuteTask(&task);
	}

	{
		tasks.Emplace(new (MemorySystem::Instance->TypeAllocate<Task>()) Task());
		Task &task{ *tasks.Back() };

		task._Function = [](void* const RESTRICT)
		{
			ShaderBuildParameters parameters;

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\StaticModelRayClosestHitShader";
			parameters._ID = "StaticModelRayClosestHitShader";
			parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\StaticModelRayClosestHitShader.rchit";
			parameters._Stage = ShaderStage::RAY_CLOSEST_HIT;

			ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
		};
		task._Arguments = nullptr;
		task._ExecutableOnSameThread = false;

		TaskSystem::Instance->ExecuteTask(&task);
	}

	{
		tasks.Emplace(new (MemorySystem::Instance->TypeAllocate<Task>()) Task());
		Task &task{ *tasks.Back() };

		task._Function = [](void* const RESTRICT)
		{
			ShaderBuildParameters parameters;

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\TemporalAntiAliasingFragmentShader";
			parameters._ID = "TemporalAntiAliasingFragmentShader";
			parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\TemporalAntiAliasingFragmentShader.frag";
			parameters._Stage = ShaderStage::FRAGMENT;

			ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
		};
		task._Arguments = nullptr;
		task._ExecutableOnSameThread = false;

		TaskSystem::Instance->ExecuteTask(&task);
	}

	{
		tasks.Emplace(new (MemorySystem::Instance->TypeAllocate<Task>()) Task());
		Task &task{ *tasks.Back() };

		task._Function = [](void* const RESTRICT)
		{
			ShaderBuildParameters parameters;

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\TerrainRayClosestHitShader";
			parameters._ID = "TerrainRayClosestHitShader";
			parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\TerrainRayClosestHitShader.rchit";
			parameters._Stage = ShaderStage::RAY_CLOSEST_HIT;

			ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
		};
		task._Arguments = nullptr;
		task._ExecutableOnSameThread = false;

		TaskSystem::Instance->ExecuteTask(&task);
	}

	{
		tasks.Emplace(new (MemorySystem::Instance->TypeAllocate<Task>()) Task());
		Task &task{ *tasks.Back() };

		task._Function = [](void* const RESTRICT)
		{
			ShaderBuildParameters parameters;

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\TerrainSceneFeaturesFragmentShader";
			parameters._ID = "TerrainSceneFeaturesFragmentShader";
			parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\TerrainSceneFeaturesFragmentShader.frag";
			parameters._Stage = ShaderStage::FRAGMENT;

			ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
		};
		task._Arguments = nullptr;
		task._ExecutableOnSameThread = false;

		TaskSystem::Instance->ExecuteTask(&task);
	}

	{
		tasks.Emplace(new (MemorySystem::Instance->TypeAllocate<Task>()) Task());
		Task &task{ *tasks.Back() };

		task._Function = [](void* const RESTRICT)
		{
			ShaderBuildParameters parameters;

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\TerrainSceneFeaturesVertexShader";
			parameters._ID = "TerrainSceneFeaturesVertexShader";
			parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\TerrainSceneFeaturesVertexShader.vert";
			parameters._Stage = ShaderStage::VERTEX;

			ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
		};
		task._Arguments = nullptr;
		task._ExecutableOnSameThread = false;

		TaskSystem::Instance->ExecuteTask(&task);
	}

	{
		tasks.Emplace(new (MemorySystem::Instance->TypeAllocate<Task>()) Task());
		Task &task{ *tasks.Back() };

		task._Function = [](void* const RESTRICT)
		{
			ShaderBuildParameters parameters;

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\ToneMappingFragmentShader";
			parameters._ID = "ToneMappingFragmentShader";
			parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\ToneMappingFragmentShader.frag";
			parameters._Stage = ShaderStage::FRAGMENT;

			ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
		};
		task._Arguments = nullptr;
		task._ExecutableOnSameThread = false;

		TaskSystem::Instance->ExecuteTask(&task);
	}

	{
		tasks.Emplace(new (MemorySystem::Instance->TypeAllocate<Task>()) Task());
		Task &task{ *tasks.Back() };

		task._Function = [](void* const RESTRICT)
		{
			ShaderBuildParameters parameters;

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\UserInterfaceFragmentShader";
			parameters._ID = "UserInterfaceFragmentShader";
			parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\UserInterfaceFragmentShader.frag";
			parameters._Stage = ShaderStage::FRAGMENT;

			ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
		};
		task._Arguments = nullptr;
		task._ExecutableOnSameThread = false;

		TaskSystem::Instance->ExecuteTask(&task);
	}

	{
		tasks.Emplace(new (MemorySystem::Instance->TypeAllocate<Task>()) Task());
		Task &task{ *tasks.Back() };

		task._Function = [](void* const RESTRICT)
		{
			ShaderBuildParameters parameters;

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\UserInterfaceVertexShader";
			parameters._ID = "UserInterfaceVertexShader";
			parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\UserInterfaceVertexShader.vert";
			parameters._Stage = ShaderStage::VERTEX;

			ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
		};
		task._Arguments = nullptr;
		task._ExecutableOnSameThread = false;

		TaskSystem::Instance->ExecuteTask(&task);
	}

	{
		tasks.Emplace(new (MemorySystem::Instance->TypeAllocate<Task>()) Task());
		Task &task{ *tasks.Back() };

		task._Function = [](void* const RESTRICT)
		{
			ShaderBuildParameters parameters;

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\VegetationColorSceneFeaturesFragmentShader";
			parameters._ID = "VegetationColorSceneFeaturesFragmentShader";
			parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\VegetationColorSceneFeaturesFragmentShader.frag";
			parameters._Stage = ShaderStage::FRAGMENT;

			ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
		};
		task._Arguments = nullptr;
		task._ExecutableOnSameThread = false;

		TaskSystem::Instance->ExecuteTask(&task);
	}

	{
		tasks.Emplace(new (MemorySystem::Instance->TypeAllocate<Task>()) Task());
		Task &task{ *tasks.Back() };

		task._Function = [](void* const RESTRICT)
		{
			ShaderBuildParameters parameters;

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\VegetationColorSceneFeaturesVertexShader";
			parameters._ID = "VegetationColorSceneFeaturesVertexShader";
			parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\VegetationColorSceneFeaturesVertexShader.vert";
			parameters._Stage = ShaderStage::VERTEX;

			ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
		};
		task._Arguments = nullptr;
		task._ExecutableOnSameThread = false;

		TaskSystem::Instance->ExecuteTask(&task);
	}

	{
		tasks.Emplace(new (MemorySystem::Instance->TypeAllocate<Task>()) Task());
		Task &task{ *tasks.Back() };

		task._Function = [](void* const RESTRICT)
		{
			ShaderBuildParameters parameters;

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\VegetationDepthSceneFeaturesFragmentShader";
			parameters._ID = "VegetationDepthSceneFeaturesFragmentShader";
			parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\VegetationDepthSceneFeaturesFragmentShader.frag";
			parameters._Stage = ShaderStage::FRAGMENT;

			ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
		};
		task._Arguments = nullptr;
		task._ExecutableOnSameThread = false;

		TaskSystem::Instance->ExecuteTask(&task);
	}

	{
		tasks.Emplace(new (MemorySystem::Instance->TypeAllocate<Task>()) Task());
		Task &task{ *tasks.Back() };

		task._Function = [](void* const RESTRICT)
		{
			ShaderBuildParameters parameters;

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\VegetationDepthSceneFeaturesVertexShader";
			parameters._ID = "VegetationDepthSceneFeaturesVertexShader";
			parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\VegetationDepthSceneFeaturesVertexShader.vert";
			parameters._Stage = ShaderStage::VERTEX;

			ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
		};
		task._Arguments = nullptr;
		task._ExecutableOnSameThread = false;

		TaskSystem::Instance->ExecuteTask(&task);
	}

	{
		tasks.Emplace(new (MemorySystem::Instance->TypeAllocate<Task>()) Task());
		Task &task{ *tasks.Back() };

		task._Function = [](void* const RESTRICT)
		{
			ShaderBuildParameters parameters;

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\VegetationImpostorColorSceneFeaturesFragmentShader";
			parameters._ID = "VegetationImpostorColorSceneFeaturesFragmentShader";
			parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\VegetationImpostorColorSceneFeaturesFragmentShader.frag";
			parameters._Stage = ShaderStage::FRAGMENT;

			ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
		};
		task._Arguments = nullptr;
		task._ExecutableOnSameThread = false;

		TaskSystem::Instance->ExecuteTask(&task);
	}

	{
		tasks.Emplace(new (MemorySystem::Instance->TypeAllocate<Task>()) Task());
		Task &task{ *tasks.Back() };

		task._Function = [](void* const RESTRICT)
		{
			ShaderBuildParameters parameters;

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\VegetationImpostorColorSceneFeaturesGeometryShader";
			parameters._ID = "VegetationImpostorColorSceneFeaturesGeometryShader";
			parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\VegetationImpostorColorSceneFeaturesGeometryShader.geom";
			parameters._Stage = ShaderStage::GEOMETRY;

			ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
		};
		task._Arguments = nullptr;
		task._ExecutableOnSameThread = false;

		TaskSystem::Instance->ExecuteTask(&task);
	}

	{
		tasks.Emplace(new (MemorySystem::Instance->TypeAllocate<Task>()) Task());
		Task &task{ *tasks.Back() };

		task._Function = [](void* const RESTRICT)
		{
			ShaderBuildParameters parameters;

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\VegetationImpostorColorSceneFeaturesVertexShader";
			parameters._ID = "VegetationImpostorColorSceneFeaturesVertexShader";
			parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\VegetationImpostorColorSceneFeaturesVertexShader.vert";
			parameters._Stage = ShaderStage::VERTEX;

			ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
		};
		task._Arguments = nullptr;
		task._ExecutableOnSameThread = false;

		TaskSystem::Instance->ExecuteTask(&task);
	}

	{
		tasks.Emplace(new (MemorySystem::Instance->TypeAllocate<Task>()) Task());
		Task &task{ *tasks.Back() };

		task._Function = [](void* const RESTRICT)
		{
			ShaderBuildParameters parameters;

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\VegetationImpostorDepthSceneFeaturesFragmentShader";
			parameters._ID = "VegetationImpostorDepthSceneFeaturesFragmentShader";
			parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\VegetationImpostorDepthSceneFeaturesFragmentShader.frag";
			parameters._Stage = ShaderStage::FRAGMENT;

			ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
		};
		task._Arguments = nullptr;
		task._ExecutableOnSameThread = false;

		TaskSystem::Instance->ExecuteTask(&task);
	}

	{
		tasks.Emplace(new (MemorySystem::Instance->TypeAllocate<Task>()) Task());
		Task &task{ *tasks.Back() };

		task._Function = [](void* const RESTRICT)
		{
			ShaderBuildParameters parameters;

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\VegetationImpostorDepthSceneFeaturesGeometryShader";
			parameters._ID = "VegetationImpostorDepthSceneFeaturesGeometryShader";
			parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\VegetationImpostorDepthSceneFeaturesGeometryShader.geom";
			parameters._Stage = ShaderStage::GEOMETRY;

			ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
		};
		task._Arguments = nullptr;
		task._ExecutableOnSameThread = false;

		TaskSystem::Instance->ExecuteTask(&task);
	}

	{
		tasks.Emplace(new (MemorySystem::Instance->TypeAllocate<Task>()) Task());
		Task &task{ *tasks.Back() };

		task._Function = [](void* const RESTRICT)
		{
			ShaderBuildParameters parameters;

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\VegetationImpostorDepthSceneFeaturesVertexShader";
			parameters._ID = "VegetationImpostorDepthSceneFeaturesVertexShader";
			parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\VegetationImpostorDepthSceneFeaturesVertexShader.vert";
			parameters._Stage = ShaderStage::VERTEX;

			ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
		};
		task._Arguments = nullptr;
		task._ExecutableOnSameThread = false;

		TaskSystem::Instance->ExecuteTask(&task);
	}

	{
		tasks.Emplace(new (MemorySystem::Instance->TypeAllocate<Task>()) Task());
		Task &task{ *tasks.Back() };

		task._Function = [](void* const RESTRICT)
		{
			ShaderBuildParameters parameters;

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\VegetationOpaqueSceneFeaturesFragmentShader";
			parameters._ID = "VegetationOpaqueSceneFeaturesFragmentShader";
			parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\VegetationOpaqueSceneFeaturesFragmentShader.frag";
			parameters._Stage = ShaderStage::FRAGMENT;

			ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
		};
		task._Arguments = nullptr;
		task._ExecutableOnSameThread = false;

		TaskSystem::Instance->ExecuteTask(&task);
	}

	{
		tasks.Emplace(new (MemorySystem::Instance->TypeAllocate<Task>()) Task());
		Task &task{ *tasks.Back() };

		task._Function = [](void* const RESTRICT)
		{
			ShaderBuildParameters parameters;

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\VegetationOpaqueSceneFeaturesVertexShader";
			parameters._ID = "VegetationOpaqueSceneFeaturesVertexShader";
			parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\VegetationOpaqueSceneFeaturesVertexShader.vert";
			parameters._Stage = ShaderStage::VERTEX;

			ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
		};
		task._Arguments = nullptr;
		task._ExecutableOnSameThread = false;

		TaskSystem::Instance->ExecuteTask(&task);
	}

	{
		tasks.Emplace(new (MemorySystem::Instance->TypeAllocate<Task>()) Task());
		Task &task{ *tasks.Back() };

		task._Function = [](void* const RESTRICT)
		{
			ShaderBuildParameters parameters;

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\VelocityFragmentShader";
			parameters._ID = "VelocityFragmentShader";
			parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\VelocityFragmentShader.frag";
			parameters._Stage = ShaderStage::FRAGMENT;

			ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
		};
		task._Arguments = nullptr;
		task._ExecutableOnSameThread = false;

		TaskSystem::Instance->ExecuteTask(&task);
	}

	{
		tasks.Emplace(new (MemorySystem::Instance->TypeAllocate<Task>()) Task());
		Task &task{ *tasks.Back() };

		task._Function = [](void* const RESTRICT)
		{
			ShaderBuildParameters parameters;

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\ViewportVertexShader";
			parameters._ID = "ViewportVertexShader";
			parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\ViewportVertexShader.vert";
			parameters._Stage = ShaderStage::VERTEX;

			ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
		};
		task._Arguments = nullptr;
		task._ExecutableOnSameThread = false;

		TaskSystem::Instance->ExecuteTask(&task);
	}

	{
		tasks.Emplace(new (MemorySystem::Instance->TypeAllocate<Task>()) Task());
		Task &task{ *tasks.Back() };

		task._Function = [](void* const RESTRICT)
		{
			ShaderBuildParameters parameters;

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\VisibilityRayMissShader";
			parameters._ID = "VisibilityRayMissShader";
			parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\VisibilityRayMissShader.rmiss";
			parameters._Stage = ShaderStage::RAY_MISS;

			ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
		};
		task._Arguments = nullptr;
		task._ExecutableOnSameThread = false;

		TaskSystem::Instance->ExecuteTask(&task);
	}

	{
		tasks.Emplace(new (MemorySystem::Instance->TypeAllocate<Task>()) Task());
		Task &task{ *tasks.Back() };

		task._Function = [](void* const RESTRICT)
		{
			ShaderBuildParameters parameters;

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\VolumetricLightingApplicationFragmentShader";
			parameters._ID = "VolumetricLightingApplicationFragmentShader";
			parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\VolumetricLightingApplicationFragmentShader.frag";
			parameters._Stage = ShaderStage::FRAGMENT;

			ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
		};
		task._Arguments = nullptr;
		task._ExecutableOnSameThread = false;

		TaskSystem::Instance->ExecuteTask(&task);
	}

	{
		tasks.Emplace(new (MemorySystem::Instance->TypeAllocate<Task>()) Task());
		Task &task{ *tasks.Back() };

		task._Function = [](void* const RESTRICT)
		{
			ShaderBuildParameters parameters;

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\VolumetricLightingFragmentShader";
			parameters._ID = "VolumetricLightingFragmentShader";
			parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\VolumetricLightingFragmentShader.frag";
			parameters._Stage = ShaderStage::FRAGMENT;

			ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
		};
		task._Arguments = nullptr;
		task._ExecutableOnSameThread = false;

		TaskSystem::Instance->ExecuteTask(&task);
	}

	{
		tasks.Emplace(new (MemorySystem::Instance->TypeAllocate<Task>()) Task());
		Task &task{ *tasks.Back() };

		task._Function = [](void* const RESTRICT)
		{
			ShaderBuildParameters parameters;

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\VolumetricLightingRayGenerationShader";
			parameters._ID = "VolumetricLightingRayGenerationShader";
			parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\VolumetricLightingRayGenerationShader.rgen";
			parameters._Stage = ShaderStage::RAY_GENERATION;

			ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
		};
		task._Arguments = nullptr;
		task._ExecutableOnSameThread = false;

		TaskSystem::Instance->ExecuteTask(&task);
	}

	{
		tasks.Emplace(new (MemorySystem::Instance->TypeAllocate<Task>()) Task());
		Task &task{ *tasks.Back() };

		task._Function = [](void* const RESTRICT)
		{
			ShaderBuildParameters parameters;

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\VolumetricLightingSpatialDenoisingFragmentShader";
			parameters._ID = "VolumetricLightingSpatialDenoisingFragmentShader";
			parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\VolumetricLightingSpatialDenoisingFragmentShader.frag";
			parameters._Stage = ShaderStage::FRAGMENT;

			ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
		};
		task._Arguments = nullptr;
		task._ExecutableOnSameThread = false;

		TaskSystem::Instance->ExecuteTask(&task);
	}

	{
		tasks.Emplace(new (MemorySystem::Instance->TypeAllocate<Task>()) Task());
		Task &task{ *tasks.Back() };

		task._Function = [](void* const RESTRICT)
		{
			ShaderBuildParameters parameters;

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\VolumetricLightingTemporalDenoisingFragmentShader";
			parameters._ID = "VolumetricLightingTemporalDenoisingFragmentShader";
			parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\VolumetricLightingTemporalDenoisingFragmentShader.frag";
			parameters._Stage = ShaderStage::FRAGMENT;

			ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
		};
		task._Arguments = nullptr;
		task._ExecutableOnSameThread = false;

		TaskSystem::Instance->ExecuteTask(&task);
	}
#endif

#if BUILD_ENGINE_ALL || BUILD_ENGINE_DEFAULT_SKY_TEXTURE
	BuildDefaultSkyTexture();
#endif

#if BUILD_ENGINE_ALL || BUILD_ENGINE_MATERIALS
	{
		tasks.Emplace(new (MemorySystem::Instance->TypeAllocate<Task>()) Task());
		Task &task{ *tasks.Back() };

		task._Function = [](void* const RESTRICT)
		{
			//Build the material.
			MaterialBuildParameters parameters;

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Catalyst_Engine_Default_Material";
			parameters._ID = "Catalyst_Engine_Default_Material";
			parameters._Type = MaterialResource::Type::OPAQUE;
			parameters._AlbedoThicknessComponent._Type = MaterialResource::MaterialResourceComponent::Type::COLOR;
			parameters._AlbedoThicknessComponent._Color = Color(Vector4<float32>(0.25f, 0.25f, 0.25f, 1.0f));
			parameters._AlbedoThicknessComponent._Color.ApplyGammaCorrection();
			parameters._NormalMapDisplacementComponent._Type = MaterialResource::MaterialResourceComponent::Type::COLOR;
			parameters._NormalMapDisplacementComponent._Color = Color(Vector4<float32>(0.5f, 0.5f, 1.0f, 0.5f));
			parameters._MaterialPropertiesComponent._Type = MaterialResource::MaterialResourceComponent::Type::COLOR;
			parameters._MaterialPropertiesComponent._Color = Color(Vector4<float32>(0.25f, 0.75f, 1.0f, 0.0f));
			parameters._OpacityComponent._Type = MaterialResource::MaterialResourceComponent::Type::COLOR;
			parameters._OpacityComponent._Color = Color(Vector4<float32>(1.0f, 1.0f, 1.0f, 1.0f));
			parameters._EmissiveMultiplier = 0.0f;

			ResourceSystem::Instance->GetResourceBuildingSystem()->BuildMaterial(parameters);
		};
		task._Arguments = nullptr;
		task._ExecutableOnSameThread = false;

		TaskSystem::Instance->ExecuteTask(&task);
	}
#endif

#if BUILD_ENGINE_ALL || BUILD_ENGINE_MODELS
	{
		tasks.Emplace(new (MemorySystem::Instance->TypeAllocate<Task>()) Task());
		Task &task{ *tasks.Back() };

		task._Function = [](void* const RESTRICT)
		{
			//Build the model.
			ModelBuildParameters parameters;

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Catalyst_Engine_Default_Model";
			parameters._ID = "Catalyst_Engine_Default_Model";
			parameters._LevelOfDetails.Emplace("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Models\\Catalyst_Engine_Default_Model.fbx");
			parameters._Transformation = Matrix4x4(VectorConstants::ZERO, Vector3<float32>(-CatalystBaseMathConstants::HALF_PI, 0.0f, 0.0f), VectorConstants::ONE);
			parameters._TextureCoordinateMultiplier = 1.0f;
			parameters._TexturCoordinateRotation = 0.0f;

			ResourceSystem::Instance->GetResourceBuildingSystem()->BuildModel(parameters);
		};
		task._Arguments = nullptr;
		task._ExecutableOnSameThread = false;

		TaskSystem::Instance->ExecuteTask(&task);
	}

	{
		tasks.Emplace(new (MemorySystem::Instance->TypeAllocate<Task>()) Task());
		Task &task{ *tasks.Back() };

		task._Function = [](void* const RESTRICT)
		{
			//Build the model.
			ModelBuildParameters parameters;

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Editor_Arrow_Model";
			parameters._ID = "Editor_Arrow_Model";
			parameters._LevelOfDetails.Emplace("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Models\\Editor_Arrow.fbx");
			parameters._Transformation = Matrix4x4(VectorConstants::ZERO, Vector3<float32>(-CatalystBaseMathConstants::HALF_PI, 0.0f, 0.0f), VectorConstants::ONE);
			parameters._TextureCoordinateMultiplier = 1.0f;
			parameters._TexturCoordinateRotation = 0.0f;

			ResourceSystem::Instance->GetResourceBuildingSystem()->BuildModel(parameters);
		};
		task._Arguments = nullptr;
		task._ExecutableOnSameThread = false;

		TaskSystem::Instance->ExecuteTask(&task);
	}
#endif

	//Wait for all tasks to finish.
	TaskSystem::Instance->WaitForAllTasksToFinish();

	//Free all tasks.
	for (Task *const RESTRICT task : tasks)
	{
		MemorySystem::Instance->TypeFree<Task>(task);
	}

#if BUILD_ENGINE_ALL || BUILD_ENGINE_CLOUD_TEXTURE || BUILD_ENGINE_FONTS || BUILD_ENGINE_OCEAN_TEXTURE || BUILD_ENGINE_BLUE_NOISE_TEXTURES || BUILD_ENGINE_SHADERS || BUILD_ENGINE_DEFAULT_SKY_TEXTURE || BUILD_ENGINE_MODELS || BUILD_ENGINE_MATERIALS || BUILD_ENGINE_RESOURCE_COLLECTION
	{
		ResourceCollectionBuildParameters resourceCollectionBuildParameters;

		resourceCollectionBuildParameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Final\\CatalystEngineResourceCollection";
		resourceCollectionBuildParameters._Folder = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate";

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildResourceCollection(resourceCollectionBuildParameters);
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
			points[i].Emplace(	CatalystRandomMath::RandomFloatInRange(0.0f, 1.0f),
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
						points[i].Emplace(points[i][j] + offset);
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

	ResourceSystem::Instance->GetResourceBuildingSystem()->BuildTexture3D(parameters);
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
			points[i].Emplace(	CatalystRandomMath::RandomFloatInRange(0.0f, 1.0f),
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
					points[i].Emplace(points[i][j] + offset);
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

	//Write the resource header to the file.
	const ResourceHeader header{ ResourceConstants::TEXTURE_2D_TYPE_IDENTIFIER, HashString("Ocean_Texture2D"), "Ocean_Texture2D" };
	file.Write(&header, sizeof(ResourceHeader));

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

/*
*	Builds the default sky texture.
*/
void CatalystEngineResourceBuilding::BuildDefaultSkyTexture()
{
	//What should the file be called?
	DynamicString file_name{ "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Default_Sky_TextureCube" };
	file_name += ".cr";

	//Open the file to be written to.
	BinaryFile<IOMode::Out> file{ file_name.Data() };

	//Write the resource header to the file.
	const ResourceHeader header{ ResourceConstants::TEXTURE_CUBE_TYPE_IDENTIFIER, HashString("Default_Sky_TextureCube"), "Default_Sky_TextureCube" };
	file.Write(&header, sizeof(ResourceHeader));

	//Write the resolution to the file.
	constexpr uint32 RESOLUTION{ 2 };
	file.Write(&RESOLUTION, sizeof(uint32));

	//Write the number of mipmap levels to the file.
	constexpr uint8 MIPMAP_LEVELS{ 1 };
	file.Write(&MIPMAP_LEVELS, sizeof(uint8));

	//Write the texture data to the file.
	DynamicArray<float32> data;
	data.Upsize<false>(RESOLUTION * RESOLUTION * 4 * 6);

	for (float32 &value : data)
	{
		value = 0.01f;
	}

	file.Write(data.Data(), RESOLUTION * RESOLUTION * 4 * 6 * sizeof(float32));

	//Cloe the file.
	file.Close();
}
#endif