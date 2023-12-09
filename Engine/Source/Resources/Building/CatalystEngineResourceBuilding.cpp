#if defined(CATALYST_ENABLE_RESOURCE_BUILDING)
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
#include <File/Writers/PNGWriter.h>

//Math.
#include <Math/Core/CatalystGeometryMath.h>
#include <Math/Core/CatalystRandomMath.h>
#include <Math/Geometry/Sphere.h>

//Rendering.
#include <Rendering/Native/RenderingCompiler.h>
#include <Rendering/Native/Texture2D.h>

//Resources.
#include <Resources/Core/ResourcesCore.h>

//Systems.
#include <Systems/MemorySystem.h>
#include <Systems/ResourceSystem.h>
#include <Systems/TaskSystem.h>

#define BUILD_ENGINE_ALL (0)

#define BUILD_ENGINE_BASE (0)
#define BUILD_ENGINE_CLOUD_TEXTURE (0)
#define BUILD_ENGINE_FONTS (0)
#define BUILD_ENGINE_OCEAN_TEXTURE (0)
#define BUILD_ENGINE_BLUE_NOISE_TEXTURES (0)
#define BUILD_ENGINE_SHADERS (0)
#define BUILD_ENGINE_DEFAULT_SKY_TEXTURE (0)
#define BUILD_ENGINE_DEFAULT_TEXTURE_2D (0)
#define BUILD_ENGINE_DEFAULT_TEXTURE_3D (0)
#define BUILD_ENGINE_MATERIALS (0)
#define BUILD_ENGINE_MODELS (0)
#define BUILD_ENGINE_STAR_TEXTURE (0)
#define BUILD_ENGINE_MISCELLANEOUS (0)

#define BUILD_ENGINE_RESOURCE_COLLECTIONS (0)

/*
*	Builds resources for the Catalyst Engine.
*/
void CatalystEngineResourceBuilding::BuildResources(const CatalystProjectConfiguration &configuration) NOEXCEPT
{
	//Initialize the task system as it will be used to build resources.
	CatalystProjectConcurrencyConfiguration concurrency_configuration;

	concurrency_configuration._NumberOfTaskExecutors = Concurrency::NumberOfHardwareThreads();

	TaskSystem::Instance->Initialize(concurrency_configuration);

	//Run the rendering compiler.
	const bool new_rendering_data_was_compiled{ RenderingCompiler::Instance->Run() };

	//Keep track of all tasks so that they can be deallocated.
	DynamicArray<Task *RESTRICT> tasks;

#if BUILD_ENGINE_BASE || BUILD_ENGINE_ALL
	{
		tasks.Emplace(new (MemorySystem::Instance->TypeAllocate<Task>()) Task());
		Task &task{ *tasks.Back() };

		task._Function = [](void* const RESTRICT)
		{
			//Gather the data.
			DynamicArray<byte> data;

			BinaryFile<BinaryFileMode::IN> data_file{ "..\\..\\..\\..\\Catalyst-Engine\\Tools\\Specular Bias Lookup Texture Generator\\Visual Studio Solution\\Specular Bias Lookup Texture Generator\\Specular Bias Lookup Texture Generator\\SpecularBiasLookupTextureData" };

			if (data_file)
			{
				data.Upsize<false>(data_file.Size());
				data_file.Read(data.Data(), data_file.Size());

				data_file.Close();
			}

			else
			{
				ASSERT(false, "Couldn't open the specular bias lookup texture data file!");
			}

			//Build the raw data.
			RawDataBuildParameters parameters;

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\Specular_Bias_Lookup_Texture_Raw_Data";
			parameters._ResourceIdentifier = "Specular_Bias_Lookup_Texture_Raw_Data";
			parameters._Data = &data;

			ResourceSystem::Instance->GetResourceBuildingSystem()->BuildRawData(parameters);
		};
		task._Arguments = nullptr;
		task._ExecutableOnSameThread = false;

		TaskSystem::Instance->ExecuteTask(&task);
	}
#endif

#if BUILD_ENGINE_ALL || BUILD_ENGINE_CLOUD_TEXTURE
	BuildCloudTexture();
#endif

#if BUILD_ENGINE_ALL || BUILD_ENGINE_FONTS
	{
		FontBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\Base\\Catalyst_Engine_Default_Font";
		parameters._ID = "Catalyst_Engine_Default_Font";
		parameters._File = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Fonts\\Kiona-Regular.ttf";

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildFont(parameters);
	}
#endif

#if BUILD_ENGINE_ALL || BUILD_ENGINE_OCEAN_TEXTURE
	BuildOceanTexture();
#endif

#if BUILD_ENGINE_ALL || BUILD_ENGINE_DEFAULT_SKY_TEXTURE
	BuildDefaultSkyTexture();
#endif

#if BUILD_ENGINE_ALL || BUILD_ENGINE_DEFAULT_TEXTURE_2D
	BuildDefaultTexture2D();
#endif

#if BUILD_ENGINE_ALL || BUILD_ENGINE_DEFAULT_TEXTURE_3D
	BuildDefaultTexture3D();
#endif

#if BUILD_ENGINE_ALL || BUILD_ENGINE_BLUE_NOISE_TEXTURES
	{
		Texture2DBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\Base\\Blue_Noise_0_Texture2D";
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

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\Base\\Blue_Noise_1_Texture2D";
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

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\Base\\Blue_Noise_2_Texture2D";
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

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\Blue_Noise_3_Texture2D";
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

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\Blue_Noise_4_Texture2D";
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

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\Blue_Noise_5_Texture2D";
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

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\Blue_Noise_6_Texture2D";
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

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\Blue_Noise_7_Texture2D";
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

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\Blue_Noise_8_Texture2D";
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

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\Blue_Noise_9_Texture2D";
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

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\Blue_Noise_10_Texture2D";
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

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\Blue_Noise_11_Texture2D";
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

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\Blue_Noise_12_Texture2D";
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

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\Blue_Noise_13_Texture2D";
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

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\Blue_Noise_14_Texture2D";
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

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\Blue_Noise_15_Texture2D";
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

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\Blue_Noise_16_Texture2D";
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

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\Blue_Noise_17_Texture2D";
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

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\Blue_Noise_18_Texture2D";
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

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\Blue_Noise_19_Texture2D";
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

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\Blue_Noise_20_Texture2D";
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

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\Blue_Noise_21_Texture2D";
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

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\Blue_Noise_22_Texture2D";
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

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\Blue_Noise_23_Texture2D";
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

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\Blue_Noise_24_Texture2D";
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

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\Blue_Noise_25_Texture2D";
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

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\Blue_Noise_26_Texture2D";
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

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\Blue_Noise_27_Texture2D";
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

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\Blue_Noise_28_Texture2D";
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

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\Blue_Noise_29_Texture2D";
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

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\Blue_Noise_30_Texture2D";
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

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\Blue_Noise_31_Texture2D";
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
		ShaderBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\AmbientOcclusionApplicationFragmentShader";
		parameters._ID = "AmbientOcclusionApplicationFragmentShader";
		parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\AmbientOcclusionApplicationFragmentShader.frag";
		parameters._Stage = ShaderStage::FRAGMENT;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
	}

	{
		ShaderBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\AmbientOcclusionSpatialDenoisingFragmentShader";
		parameters._ID = "AmbientOcclusionSpatialDenoisingFragmentShader";
		parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\AmbientOcclusionSpatialDenoisingFragmentShader.frag";
		parameters._Stage = ShaderStage::FRAGMENT;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
	}

	{
		ShaderBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\AmbientOcclusionTemporalDenoisingFragmentShader";
		parameters._ID = "AmbientOcclusionTemporalDenoisingFragmentShader";
		parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\AmbientOcclusionTemporalDenoisingFragmentShader.frag";
		parameters._Stage = ShaderStage::FRAGMENT;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
	}

	{
		ShaderBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\AnimatedModelSceneFeaturesFragmentShader";
		parameters._ID = "AnimatedModelSceneFeaturesFragmentShader";
		parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\AnimatedModelSceneFeaturesFragmentShader.frag";
		parameters._Stage = ShaderStage::FRAGMENT;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
	}

	{
		ShaderBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\AnimatedModelSceneFeaturesVertexShader";
		parameters._ID = "AnimatedModelSceneFeaturesVertexShader";
		parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\AnimatedModelSceneFeaturesVertexShader.vert";
		parameters._Stage = ShaderStage::VERTEX;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
	}

	{
		ShaderBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\AxisAlignedBoundingBox3DVertexShader";
		parameters._ID = "AxisAlignedBoundingBox3DVertexShader";
		parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\AxisAlignedBoundingBox3DVertexShader.vert";
		parameters._Stage = ShaderStage::VERTEX;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
	}

	{
		ShaderBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\BloomIsolationFragmentShader";
		parameters._ID = "BloomIsolationFragmentShader";
		parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\BloomIsolationFragmentShader.frag";
		parameters._Stage = ShaderStage::FRAGMENT;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
	}

	{
		ShaderBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\DebugFragmentShader";
		parameters._ID = "DebugFragmentShader";
		parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\DebugFragmentShader.frag";
		parameters._Stage = ShaderStage::FRAGMENT;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
	}

	{
		ShaderBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\DebugRenderFragmentShader";
		parameters._ID = "DebugRenderFragmentShader";
		parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\DebugRenderFragmentShader.frag";
		parameters._Stage = ShaderStage::FRAGMENT;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
	}

	{
		ShaderBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\DepthDownsampleFragmentShader";
		parameters._ID = "DepthDownsampleFragmentShader";
		parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\DepthDownsampleFragmentShader.frag";
		parameters._Stage = ShaderStage::FRAGMENT;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
	}

	{
		ShaderBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\DepthOfFieldApplicationFragmentShader";
		parameters._ID = "DepthOfFieldApplicationFragmentShader";
		parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\DepthOfFieldApplicationFragmentShader.frag";
		parameters._Stage = ShaderStage::FRAGMENT;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
	}

	{
		ShaderBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\DepthOfFieldBokehBlurFragmentShader";
		parameters._ID = "DepthOfFieldBokehBlurFragmentShader";
		parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\DepthOfFieldBokehBlurFragmentShader.frag";
		parameters._Stage = ShaderStage::FRAGMENT;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
	}

	{
		ShaderBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\DepthOfFieldFloodfillBlurFragmentShader";
		parameters._ID = "DepthOfFieldFloodfillBlurFragmentShader";
		parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\DepthOfFieldFloodfillBlurFragmentShader.frag";
		parameters._Stage = ShaderStage::FRAGMENT;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
	}

	{
		ShaderBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\DirectLightingFragmentShader";
		parameters._ID = "DirectLightingFragmentShader";
		parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\DirectLightingFragmentShader.frag";
		parameters._Stage = ShaderStage::FRAGMENT;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
	}

	{
		ShaderBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\DynamicModelRayClosestHitShader";
		parameters._ID = "DynamicModelRayClosestHitShader";
		parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\ModelRayClosestHitShader.rchit";
		parameters._Defines.Emplace("DYNAMIC_MODELS");
		parameters._Stage = ShaderStage::RAY_CLOSEST_HIT;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
	}

	{
		ShaderBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\EditorSelectedModelFragmentShader";
		parameters._ID = "EditorSelectedModelFragmentShader";
		parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\EditorSelectedModelFragmentShader.frag";
		parameters._Stage = ShaderStage::FRAGMENT;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
	}

	{
		ShaderBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\EditorSelectedModelVertexShader";
		parameters._ID = "EditorSelectedModelVertexShader";
		parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\EditorSelectedModelVertexShader.vert";
		parameters._Stage = ShaderStage::VERTEX;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
	}

	{
		ShaderBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\EditorUserInterfaceFragmentShader";
		parameters._ID = "EditorUserInterfaceFragmentShader";
		parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\EditorUserInterfaceFragmentShader.frag";
		parameters._Stage = ShaderStage::FRAGMENT;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
	}

	{
		ShaderBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\EditorUserInterfaceVertexShader";
		parameters._ID = "EditorUserInterfaceVertexShader";
		parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\EditorUserInterfaceVertexShader.vert";
		parameters._Stage = ShaderStage::VERTEX;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
	}

	{
		ShaderBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\FastApproximateAntiAliasingFragmentShader";
		parameters._ID = "FastApproximateAntiAliasingFragmentShader";
		parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\FastApproximateAntiAliasingFragmentShader.frag";
		parameters._Stage = ShaderStage::FRAGMENT;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
	}

	{
		ShaderBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\IndirectLightingApplicationFragmentShader";
		parameters._ID = "IndirectLightingApplicationFragmentShader";
		parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\IndirectLightingApplicationFragmentShader.frag";
		parameters._Stage = ShaderStage::FRAGMENT;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
	}

	{
		ShaderBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\IndirectLightingTemporalDenoisingFragmentShader";
		parameters._ID = "IndirectLightingTemporalDenoisingFragmentShader";
		parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\IndirectLightingTemporalDenoisingFragmentShader.frag";
		parameters._Stage = ShaderStage::FRAGMENT;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
	}

	{
		ShaderBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\InstancedImpostorColorSceneFeaturesFragmentShader";
		parameters._ID = "InstancedImpostorColorSceneFeaturesFragmentShader";
		parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\InstancedImpostorColorSceneFeaturesFragmentShader.frag";
		parameters._Stage = ShaderStage::FRAGMENT;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
	}

	{
		ShaderBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\InstancedImpostorColorSceneFeaturesVertexShader";
		parameters._ID = "InstancedImpostorColorSceneFeaturesVertexShader";
		parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\InstancedImpostorColorSceneFeaturesVertexShader.vert";
		parameters._Stage = ShaderStage::VERTEX;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
	}

	{
		ShaderBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\InstancedImpostorDepthSceneFeaturesFragmentShader";
		parameters._ID = "InstancedImpostorDepthSceneFeaturesFragmentShader";
		parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\InstancedImpostorDepthSceneFeaturesFragmentShader.frag";
		parameters._Stage = ShaderStage::FRAGMENT;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
	}

	{
		ShaderBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\InstancedImpostorDepthSceneFeaturesVertexShader";
		parameters._ID = "InstancedImpostorDepthSceneFeaturesVertexShader";
		parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\InstancedImpostorDepthSceneFeaturesVertexShader.vert";
		parameters._Stage = ShaderStage::VERTEX;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
	}

	{
		ShaderBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\InstancedOpaqueModelShadowsFragmentShader";
		parameters._ID = "InstancedOpaqueModelShadowsFragmentShader";
		parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\InstancedOpaqueModelShadowsFragmentShader.frag";
		parameters._Stage = ShaderStage::FRAGMENT;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
	}

	{
		ShaderBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\InstancedOpaqueModelShadowsVertexShader";
		parameters._ID = "InstancedOpaqueModelShadowsVertexShader";
		parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\InstancedOpaqueModelShadowsVertexShader.vert";
		parameters._Stage = ShaderStage::VERTEX;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
	}

	{
		ShaderBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\InstancedStaticModelColorSceneFeaturesFragmentShader";
		parameters._ID = "InstancedStaticModelColorSceneFeaturesFragmentShader";
		parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\InstancedStaticModelColorSceneFeaturesFragmentShader.frag";
		parameters._Stage = ShaderStage::FRAGMENT;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
	}

	{
		ShaderBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\InstancedStaticModelColorSceneFeaturesVertexShader";
		parameters._ID = "InstancedStaticModelColorSceneFeaturesVertexShader";
		parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\InstancedStaticModelColorSceneFeaturesVertexShader.vert";
		parameters._Stage = ShaderStage::VERTEX;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
	}

	{
		ShaderBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\InstancedStaticModelDepthSceneFeaturesFragmentShader";
		parameters._ID = "InstancedStaticModelDepthSceneFeaturesFragmentShader";
		parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\InstancedStaticModelDepthSceneFeaturesFragmentShader.frag";
		parameters._Stage = ShaderStage::FRAGMENT;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
	}

	{
		ShaderBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\InstancedStaticModelDepthSceneFeaturesVertexShader";
		parameters._ID = "InstancedStaticModelDepthSceneFeaturesVertexShader";
		parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\InstancedStaticModelDepthSceneFeaturesVertexShader.vert";
		parameters._Stage = ShaderStage::VERTEX;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
	}

	{
		ShaderBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\MaskedModelColorSceneFeaturesFragmentShader";
		parameters._ID = "MaskedModelColorSceneFeaturesFragmentShader";
		parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\MaskedModelColorSceneFeaturesFragmentShader.frag";
		parameters._Stage = ShaderStage::FRAGMENT;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
	}

	{
		ShaderBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\MaskedModelColorSceneFeaturesVertexShader";
		parameters._ID = "MaskedModelColorSceneFeaturesVertexShader";
		parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\MaskedModelColorSceneFeaturesVertexShader.vert";
		parameters._Stage = ShaderStage::VERTEX;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
	}

	{
		ShaderBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\MaskedModelDepthSceneFeaturesFragmentShader";
		parameters._ID = "MaskedModelDepthSceneFeaturesFragmentShader";
		parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\MaskedModelDepthSceneFeaturesFragmentShader.frag";
		parameters._Stage = ShaderStage::FRAGMENT;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
	}

	{
		ShaderBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\MaskedModelDepthSceneFeaturesVertexShader";
		parameters._ID = "MaskedModelDepthSceneFeaturesVertexShader";
		parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\MaskedModelDepthSceneFeaturesVertexShader.vert";
		parameters._Stage = ShaderStage::VERTEX;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
	}

	{
		ShaderBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\MobileFragmentShader";
		parameters._ID = "MobileFragmentShader";
		parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\MobileFragmentShader.frag";
		parameters._Stage = ShaderStage::FRAGMENT;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
	}

	{
		ShaderBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\MobileVertexShader";
		parameters._ID = "MobileVertexShader";
		parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\MobileVertexShader.vert";
		parameters._Stage = ShaderStage::VERTEX;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
	}

	{
		ShaderBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\ModelShadowMapFragmentShader";
		parameters._ID = "ModelShadowMapFragmentShader";
		parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\ModelShadowMapFragmentShader.frag";
		parameters._Stage = ShaderStage::FRAGMENT;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
	}

	{
		ShaderBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\ModelShadowMapVertexShader";
		parameters._ID = "ModelShadowMapVertexShader";
		parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\ModelShadowMapVertexShader.vert";
		parameters._Stage = ShaderStage::VERTEX;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
	}

	{
		ShaderBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\MotionBlurFragmentShader";
		parameters._ID = "MotionBlurFragmentShader";
		parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\MotionBlurFragmentShader.frag";
		parameters._Stage = ShaderStage::FRAGMENT;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
	}

	{
		ShaderBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\OpaqueModelSceneFeaturesFragmentShader";
		parameters._ID = "OpaqueModelSceneFeaturesFragmentShader";
		parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\OpaqueModelSceneFeaturesFragmentShader.frag";
		parameters._Stage = ShaderStage::FRAGMENT;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
	}

	{
		ShaderBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\OpaqueModelSceneFeaturesVertexShader";
		parameters._ID = "OpaqueModelSceneFeaturesVertexShader";
		parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\OpaqueModelSceneFeaturesVertexShader.vert";
		parameters._Stage = ShaderStage::VERTEX;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
	}

	{
		ShaderBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\ParticleSystemComputeShader";
		parameters._ID = "ParticleSystemComputeShader";
		parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\ParticleSystemComputeShader.comp";
		parameters._Stage = ShaderStage::COMPUTE;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
	}

	{
		ShaderBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\ParticleSystemMaskedColorSceneFeaturesFragmentShader";
		parameters._ID = "ParticleSystemMaskedColorSceneFeaturesFragmentShader";
		parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\ParticleSystemMaskedColorSceneFeaturesFragmentShader.frag";
		parameters._Stage = ShaderStage::FRAGMENT;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
	}

	{
		ShaderBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\ParticleSystemMaskedColorSceneFeaturesGeometryShader";
		parameters._ID = "ParticleSystemMaskedColorSceneFeaturesGeometryShader";
		parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\ParticleSystemMaskedColorSceneFeaturesGeometryShader.geom";
		parameters._Stage = ShaderStage::GEOMETRY;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
	}

	{
		ShaderBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\ParticleSystemMaskedColorSceneFeaturesVertexShader";
		parameters._ID = "ParticleSystemMaskedColorSceneFeaturesVertexShader";
		parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\ParticleSystemMaskedColorSceneFeaturesVertexShader.vert";
		parameters._Stage = ShaderStage::VERTEX;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
	}

	{
		ShaderBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\ParticleSystemMaskedDepthSceneFeaturesFragmentShader";
		parameters._ID = "ParticleSystemMaskedDepthSceneFeaturesFragmentShader";
		parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\ParticleSystemMaskedDepthSceneFeaturesFragmentShader.frag";
		parameters._Stage = ShaderStage::FRAGMENT;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
	}

	{
		ShaderBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\ParticleSystemMaskedDepthSceneFeaturesGeometryShader";
		parameters._ID = "ParticleSystemMaskedDepthSceneFeaturesGeometryShader";
		parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\ParticleSystemMaskedDepthSceneFeaturesGeometryShader.geom";
		parameters._Stage = ShaderStage::GEOMETRY;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
	}

	{
		ShaderBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\ParticleSystemMaskedDepthSceneFeaturesVertexShader";
		parameters._ID = "ParticleSystemMaskedDepthSceneFeaturesVertexShader";
		parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\ParticleSystemMaskedDepthSceneFeaturesVertexShader.vert";
		parameters._Stage = ShaderStage::VERTEX;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
	}

	{
		ShaderBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\PassthroughFragmentShader";
		parameters._ID = "PassthroughFragmentShader";
		parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\PassthroughFragmentShader.frag";
		parameters._Stage = ShaderStage::FRAGMENT;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
	}

	{
		ShaderBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\PathTracingRayGenerationShader";
		parameters._ID = "PathTracingRayGenerationShader";
		parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\PathTracingRayGenerationShader.rgen";
		parameters._Stage = ShaderStage::RAY_GENERATION;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
	}

	{
		ShaderBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\PathTracingRayMissShader";
		parameters._ID = "PathTracingRayMissShader";
		parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\PathTracingRayMissShader.rmiss";
		parameters._Stage = ShaderStage::RAY_MISS;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
	}

	{
		ShaderBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\PostProcessingFragmentShader";
		parameters._ID = "PostProcessingFragmentShader";
		parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\PostProcessingFragmentShader.frag";
		parameters._Stage = ShaderStage::FRAGMENT;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
	}

	{
		ShaderBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\RasterizedShadowsFragmentShader";
		parameters._ID = "RasterizedShadowsFragmentShader";
		parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\RasterizedShadowsFragmentShader.frag";
		parameters._Stage = ShaderStage::FRAGMENT;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
	}

	{
		ShaderBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\RayTracedAmbientOcclusionRayGenerationShader";
		parameters._ID = "RayTracedAmbientOcclusionRayGenerationShader";
		parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\RayTracedAmbientOcclusionRayGenerationShader.rgen";
		parameters._Stage = ShaderStage::RAY_GENERATION;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
	}

	{
		ShaderBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\RayTracedIndirectLightingRayGenerationShader";
		parameters._ID = "RayTracedIndirectLightingRayGenerationShader";
		parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\RayTracedIndirectLightingRayGenerationShader.rgen";
		parameters._Stage = ShaderStage::RAY_GENERATION;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
	}

	{
		ShaderBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\RayTracedShadowsRayGenerationShader";
		parameters._ID = "RayTracedShadowsRayGenerationShader";
		parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\RayTracedShadowsRayGenerationShader.rgen";
		parameters._Stage = ShaderStage::RAY_GENERATION;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
	}

	{
		ShaderBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\RayTracedVolumetricLightingRayGenerationShader";
		parameters._ID = "RayTracedVolumetricLightingRayGenerationShader";
		parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\RayTracedVolumetricLightingRayGenerationShader.rgen";
		parameters._Stage = ShaderStage::RAY_GENERATION;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
	}

	{
		ShaderBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\RenderingReferenceFragmentShader";
		parameters._ID = "RenderingReferenceFragmentShader";
		parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\RenderingReferenceFragmentShader.frag";
		parameters._Stage = ShaderStage::FRAGMENT;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
	}

	{
		ShaderBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\ResampleFragmentShader";
		parameters._ID = "ResampleFragmentShader";
		parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\ResampleFragmentShader.frag";
		parameters._Stage = ShaderStage::FRAGMENT;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
	}

	{
		ShaderBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\SceneFeaturesDownsampleFragmentShader";
		parameters._ID = "SceneFeaturesDownsampleFragmentShader";
		parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\SceneFeaturesDownsampleFragmentShader.frag";
		parameters._Stage = ShaderStage::FRAGMENT;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
	}

	{
		ShaderBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\ScreenSpaceAmbientOcclusionFragmentShader";
		parameters._ID = "ScreenSpaceAmbientOcclusionFragmentShader";
		parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\ScreenSpaceAmbientOcclusionFragmentShader.frag";
		parameters._Stage = ShaderStage::FRAGMENT;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
	}

	{
		ShaderBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\ScreenSpaceIndirectLightingFragmentShader";
		parameters._ID = "ScreenSpaceIndirectLightingFragmentShader";
		parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\ScreenSpaceIndirectLightingFragmentShader.frag";
		parameters._Stage = ShaderStage::FRAGMENT;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
	}

	{
		ShaderBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\ScreenSpaceIndirectLightingResolveFragmentShader";
		parameters._ID = "ScreenSpaceIndirectLightingResolveFragmentShader";
		parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\ScreenSpaceIndirectLightingResolveFragmentShader.frag";
		parameters._Stage = ShaderStage::FRAGMENT;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
	}

	{
		ShaderBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\ShadowsSpatialDenoisingFragmentShader";
		parameters._ID = "ShadowsSpatialDenoisingFragmentShader";
		parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\ShadowsSpatialDenoisingFragmentShader.frag";
		parameters._Stage = ShaderStage::FRAGMENT;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
	}

	{
		ShaderBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\SkyFragmentShader";
		parameters._ID = "SkyFragmentShader";
		parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\SkyFragmentShader.frag";
		parameters._Stage = ShaderStage::FRAGMENT;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
	}

	{
		ShaderBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\SphereVertexShader";
		parameters._ID = "SphereVertexShader";
		parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\SphereVertexShader.vert";
		parameters._Stage = ShaderStage::VERTEX;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
	}

	{
		ShaderBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\StaticModelRayClosestHitShader";
		parameters._ID = "StaticModelRayClosestHitShader";
		parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\ModelRayClosestHitShader.rchit";
		parameters._Defines.Emplace("STATIC_MODELS");
		parameters._Stage = ShaderStage::RAY_CLOSEST_HIT;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
	}

	{
		ShaderBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\TemporalAntiAliasingFragmentShader";
		parameters._ID = "TemporalAntiAliasingFragmentShader";
		parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\TemporalAntiAliasingFragmentShader.frag";
		parameters._Stage = ShaderStage::FRAGMENT;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
	}

	{
		ShaderBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\TerrainRayClosestHitShader";
		parameters._ID = "TerrainRayClosestHitShader";
		parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\TerrainRayClosestHitShader.rchit";
		parameters._Stage = ShaderStage::RAY_CLOSEST_HIT;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
	}

	{
		ShaderBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\TerrainSceneFeaturesFragmentShader";
		parameters._ID = "TerrainSceneFeaturesFragmentShader";
		parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\TerrainSceneFeaturesFragmentShader.frag";
		parameters._Stage = ShaderStage::FRAGMENT;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
	}

	{
		ShaderBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\TerrainSceneFeaturesVertexShader";
		parameters._ID = "TerrainSceneFeaturesVertexShader";
		parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\TerrainSceneFeaturesVertexShader.vert";
		parameters._Stage = ShaderStage::VERTEX;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
	}

	{
		ShaderBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\TerrainShadowFragmentShader";
		parameters._ID = "TerrainShadowFragmentShader";
		parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\TerrainShadowFragmentShader.frag";
		parameters._Stage = ShaderStage::FRAGMENT;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
	}

	{
		ShaderBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\TerrainShadowVertexShader";
		parameters._ID = "TerrainShadowVertexShader";
		parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\TerrainShadowVertexShader.vert";
		parameters._Stage = ShaderStage::VERTEX;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
	}

	{
		ShaderBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\ThreeDimensionalUserInterfaceFragmentShader";
		parameters._ID = "ThreeDimensionalUserInterfaceFragmentShader";
		parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\UserInterfaceFragmentShader.frag";
		parameters._Defines.Emplace("THREE_DIMENSIONAL_USER_INTERFACE");
		parameters._Stage = ShaderStage::FRAGMENT;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
	}

	{
		ShaderBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\ThreeDimensionalUserInterfaceVertexShader";
		parameters._ID = "ThreeDimensionalUserInterfaceVertexShader";
		parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\UserInterfaceVertexShader.vert";
		parameters._Defines.Emplace("THREE_DIMENSIONAL_USER_INTERFACE");
		parameters._Stage = ShaderStage::VERTEX;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
	}

	{
		ShaderBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\ToneMappingFragmentShader";
		parameters._ID = "ToneMappingFragmentShader";
		parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\ToneMappingFragmentShader.frag";
		parameters._Stage = ShaderStage::FRAGMENT;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
	}

	{
		ShaderBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\UserInterfaceFragmentShader";
		parameters._ID = "UserInterfaceFragmentShader";
		parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\UserInterfaceFragmentShader.frag";
		parameters._Stage = ShaderStage::FRAGMENT;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
	}

	{
		ShaderBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\UserInterfaceVertexShader";
		parameters._ID = "UserInterfaceVertexShader";
		parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\UserInterfaceVertexShader.vert";
		parameters._Stage = ShaderStage::VERTEX;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
	}

	{
		ShaderBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\VelocityFragmentShader";
		parameters._ID = "VelocityFragmentShader";
		parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\VelocityFragmentShader.frag";
		parameters._Stage = ShaderStage::FRAGMENT;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
	}

	{
		ShaderBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\ViewportVertexShader";
		parameters._ID = "ViewportVertexShader";
		parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\ViewportVertexShader.vert";
		parameters._Stage = ShaderStage::VERTEX;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
	}

	{
		ShaderBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\VirtualRealityFragmentShader";
		parameters._ID = "VirtualRealityFragmentShader";
		parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\VirtualRealityFragmentShader.frag";
		parameters._Stage = ShaderStage::FRAGMENT;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
	}

	{
		ShaderBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\VirtualRealityVertexShader";
		parameters._ID = "VirtualRealityVertexShader";
		parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\VirtualRealityVertexShader.vert";
		parameters._Stage = ShaderStage::VERTEX;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
	}

	{
		ShaderBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\VisibilityRayMissShader";
		parameters._ID = "VisibilityRayMissShader";
		parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\VisibilityRayMissShader.rmiss";
		parameters._Stage = ShaderStage::RAY_MISS;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
	}

	{
		ShaderBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\VolumetricLightingApplicationFragmentShader";
		parameters._ID = "VolumetricLightingApplicationFragmentShader";
		parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\VolumetricLightingApplicationFragmentShader.frag";
		parameters._Stage = ShaderStage::FRAGMENT;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
	}

	{
		ShaderBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\VolumetricLightingFragmentShader";
		parameters._ID = "VolumetricLightingFragmentShader";
		parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\VolumetricLightingFragmentShader.frag";
		parameters._Stage = ShaderStage::FRAGMENT;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
	}

	{
		ShaderBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\VolumetricLightingSpatialDenoisingFragmentShader";
		parameters._ID = "VolumetricLightingSpatialDenoisingFragmentShader";
		parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\VolumetricLightingSpatialDenoisingFragmentShader.frag";
		parameters._Stage = ShaderStage::FRAGMENT;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
	}

	{
		ShaderBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\VolumetricLightingTemporalDenoisingFragmentShader";
		parameters._ID = "VolumetricLightingTemporalDenoisingFragmentShader";
		parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\VolumetricLightingTemporalDenoisingFragmentShader.frag";
		parameters._Stage = ShaderStage::FRAGMENT;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildShader(parameters);
	}
#endif

#if BUILD_ENGINE_ALL || BUILD_ENGINE_MATERIALS
	{
		tasks.Emplace(new (MemorySystem::Instance->TypeAllocate<Task>()) Task());
		Task &task{ *tasks.Back() };

		task._Function = [](void* const RESTRICT)
		{
			//Build the material.
			MaterialBuildParameters parameters;

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\Catalyst_Engine_Default_Material";
			parameters._ID = "Catalyst_Engine_Default_Material";
			parameters._Type = MaterialResource::Type::OPAQUE;
			parameters._AlbedoThicknessComponent._Type = MaterialResource::MaterialResourceComponent::Type::COLOR;
			parameters._AlbedoThicknessComponent._Color = Color(Vector4<float32>(0.5f, 0.5f, 0.5f, 1.0f));
			parameters._AlbedoThicknessComponent._Color.ApplyGammaCorrection();
			parameters._NormalMapDisplacementComponent._Type = MaterialResource::MaterialResourceComponent::Type::COLOR;
			parameters._NormalMapDisplacementComponent._Color = Color(Vector4<float32>(0.5f, 0.5f, 1.0f, 0.5f));
			parameters._MaterialPropertiesComponent._Type = MaterialResource::MaterialResourceComponent::Type::COLOR;
			parameters._MaterialPropertiesComponent._Color = Color(Vector4<float32>(0.25f, 0.75f, 1.0f, 0.0f));
			parameters._OpacityComponent._Type = MaterialResource::MaterialResourceComponent::Type::COLOR;
			parameters._OpacityComponent._Color = Color(Vector4<float32>(1.0f, 1.0f, 1.0f, 1.0f));
			parameters._EmissiveMultiplier = 0.0f;
			parameters._DoubleSided = false;

			ResourceSystem::Instance->GetResourceBuildingSystem()->BuildMaterial(parameters);
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
			//Build the material.
			MaterialBuildParameters parameters;

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\Sponza_Arch_Material";
			parameters._ID = "Sponza_Arch_Material";
			parameters._Type = MaterialResource::Type::OPAQUE;
			parameters._AlbedoThicknessComponent._Type = MaterialResource::MaterialResourceComponent::Type::TEXTURE;
			parameters._AlbedoThicknessComponent._TextureResourceIdentifier = HashString("Sponza_Arch_AlbedoThickness_Texture2D");
			parameters._NormalMapDisplacementComponent._Type = MaterialResource::MaterialResourceComponent::Type::COLOR;
			parameters._NormalMapDisplacementComponent._Color = Color(Vector4<float32>(0.5f, 0.5f, 1.0f, 0.5f));
			parameters._MaterialPropertiesComponent._Type = MaterialResource::MaterialResourceComponent::Type::COLOR;
			parameters._MaterialPropertiesComponent._Color = Color(Vector4<float32>(0.789f, 0.0f, 1.0f, 0.0f));
			parameters._OpacityComponent._Type = MaterialResource::MaterialResourceComponent::Type::COLOR;
			parameters._OpacityComponent._Color = Color(Vector4<float32>(1.0f, 1.0f, 1.0f, 1.0f));
			parameters._EmissiveMultiplier = 0.0f;
			parameters._DoubleSided = false;

			ResourceSystem::Instance->GetResourceBuildingSystem()->BuildMaterial(parameters);
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
			//Build the material.
			MaterialBuildParameters parameters;

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\Sponza_Bricks_Material";
			parameters._ID = "Sponza_Bricks_Material";
			parameters._Type = MaterialResource::Type::OPAQUE;
			parameters._AlbedoThicknessComponent._Type = MaterialResource::MaterialResourceComponent::Type::TEXTURE;
			parameters._AlbedoThicknessComponent._TextureResourceIdentifier = HashString("Sponza_Bricks_AlbedoThickness_Texture2D");
			parameters._NormalMapDisplacementComponent._Type = MaterialResource::MaterialResourceComponent::Type::TEXTURE;
			parameters._NormalMapDisplacementComponent._TextureResourceIdentifier = HashString("Sponza_Bricks_NormalMapDisplacement_Texture2D");
			parameters._MaterialPropertiesComponent._Type = MaterialResource::MaterialResourceComponent::Type::COLOR;
			parameters._MaterialPropertiesComponent._Color = Color(Vector4<float32>(0.895f, 0.0f, 1.0f, 0.0f));
			parameters._OpacityComponent._Type = MaterialResource::MaterialResourceComponent::Type::COLOR;
			parameters._OpacityComponent._Color = Color(Vector4<float32>(1.0f, 1.0f, 1.0f, 1.0f));
			parameters._EmissiveMultiplier = 0.0f;
			parameters._DoubleSided = false;

			ResourceSystem::Instance->GetResourceBuildingSystem()->BuildMaterial(parameters);
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
			//Build the material.
			MaterialBuildParameters parameters;

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\Sponza_Ceiling_Material";
			parameters._ID = "Sponza_Ceiling_Material";
			parameters._Type = MaterialResource::Type::OPAQUE;
			parameters._AlbedoThicknessComponent._Type = MaterialResource::MaterialResourceComponent::Type::TEXTURE;
			parameters._AlbedoThicknessComponent._TextureResourceIdentifier = HashString("Sponza_Ceiling_AlbedoThickness_Texture2D");
			parameters._NormalMapDisplacementComponent._Type = MaterialResource::MaterialResourceComponent::Type::COLOR;
			parameters._NormalMapDisplacementComponent._Color = Color(Vector4<float32>(0.5f, 0.5f, 1.0f, 0.5f));
			parameters._MaterialPropertiesComponent._Type = MaterialResource::MaterialResourceComponent::Type::COLOR;
			parameters._MaterialPropertiesComponent._Color = Color(Vector4<float32>(0.895f, 0.0f, 1.0f, 0.0f));
			parameters._OpacityComponent._Type = MaterialResource::MaterialResourceComponent::Type::COLOR;
			parameters._OpacityComponent._Color = Color(Vector4<float32>(1.0f, 1.0f, 1.0f, 1.0f));
			parameters._EmissiveMultiplier = 0.0f;
			parameters._DoubleSided = false;

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

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\Catalyst_Engine_Default_Model";
			parameters._ResourceIdentifier = "Catalyst_Engine_Default_Model";
			parameters._LevelOfDetails.Emplace("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Models\\Catalyst_Engine_Default_Model.fbx");
			parameters._Transformation = Matrix4x4();
			parameters._TextureCoordinateMultiplier = 1.0f;
			parameters._TexturCoordinateRotation = 0.0f;
			parameters._ProceduralFunction = nullptr;
			parameters._CollisionModelFilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Models\\Catalyst_Engine_Default_Model.fbx";

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

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\Sponza_Arch_Model";
			parameters._ResourceIdentifier = "Sponza_Arch_Model";
			parameters._LevelOfDetails.Emplace("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Models\\Sponza_Arch.fbx");
			parameters._Transformation = Matrix4x4();
			parameters._TextureCoordinateMultiplier = 1.0f;
			parameters._TexturCoordinateRotation = 0.0f;
			parameters._ProceduralFunction = nullptr;
			parameters._CollisionModelFilePath = nullptr;

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

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\Sponza_Bricks_Model";
			parameters._ResourceIdentifier = "Sponza_Bricks_Model";
			parameters._LevelOfDetails.Emplace("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Models\\Sponza_Bricks.fbx");
			parameters._Transformation = Matrix4x4();
			parameters._TextureCoordinateMultiplier = 1.0f;
			parameters._TexturCoordinateRotation = 0.0f;
			parameters._ProceduralFunction = nullptr;
			parameters._CollisionModelFilePath = nullptr;

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

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\Sponza_Ceiling_Model";
			parameters._ResourceIdentifier = "Sponza_Ceiling_Model";
			parameters._LevelOfDetails.Emplace("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Models\\Sponza_Ceiling.fbx");
			parameters._Transformation = Matrix4x4();
			parameters._TextureCoordinateMultiplier = 1.0f;
			parameters._TexturCoordinateRotation = 0.0f;
			parameters._ProceduralFunction = nullptr;
			parameters._CollisionModelFilePath = nullptr;

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

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\Sponza_Chain_Model";
			parameters._ResourceIdentifier = "Sponza_Chain_Model";
			parameters._LevelOfDetails.Emplace("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Models\\Sponza_Chain.fbx");
			parameters._Transformation = Matrix4x4();
			parameters._TextureCoordinateMultiplier = 1.0f;
			parameters._TexturCoordinateRotation = 0.0f;
			parameters._ProceduralFunction = nullptr;
			parameters._CollisionModelFilePath = nullptr;

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

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\Sponza_Column_A_Model";
			parameters._ResourceIdentifier = "Sponza_Column_A_Model";
			parameters._LevelOfDetails.Emplace("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Models\\Sponza_Column_A.fbx");
			parameters._Transformation = Matrix4x4();
			parameters._TextureCoordinateMultiplier = 1.0f;
			parameters._TexturCoordinateRotation = 0.0f;
			parameters._ProceduralFunction = nullptr;
			parameters._CollisionModelFilePath = nullptr;

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

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\Sponza_Column_B_Model";
			parameters._ResourceIdentifier = "Sponza_Column_B_Model";
			parameters._LevelOfDetails.Emplace("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Models\\Sponza_Column_B.fbx");
			parameters._Transformation = Matrix4x4();
			parameters._TextureCoordinateMultiplier = 1.0f;
			parameters._TexturCoordinateRotation = 0.0f;
			parameters._ProceduralFunction = nullptr;
			parameters._CollisionModelFilePath = nullptr;

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

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\Sponza_Column_C_Model";
			parameters._ResourceIdentifier = "Sponza_Column_C_Model";
			parameters._LevelOfDetails.Emplace("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Models\\Sponza_Column_C.fbx");
			parameters._Transformation = Matrix4x4();
			parameters._TextureCoordinateMultiplier = 1.0f;
			parameters._TexturCoordinateRotation = 0.0f;
			parameters._ProceduralFunction = nullptr;
			parameters._CollisionModelFilePath = nullptr;

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

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\Sponza_Details_Model";
			parameters._ResourceIdentifier = "Sponza_Details_Model";
			parameters._LevelOfDetails.Emplace("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Models\\Sponza_Details.fbx");
			parameters._Transformation = Matrix4x4();
			parameters._TextureCoordinateMultiplier = 1.0f;
			parameters._TexturCoordinateRotation = 0.0f;
			parameters._ProceduralFunction = nullptr;
			parameters._CollisionModelFilePath = nullptr;

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

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\Sponza_Fabric_A_Model";
			parameters._ResourceIdentifier = "Sponza_Fabric_A_Model";
			parameters._LevelOfDetails.Emplace("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Models\\Sponza_Fabric_A.fbx");
			parameters._Transformation = Matrix4x4();
			parameters._TextureCoordinateMultiplier = 1.0f;
			parameters._TexturCoordinateRotation = 0.0f;
			parameters._ProceduralFunction = nullptr;
			parameters._CollisionModelFilePath = nullptr;

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

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\Sponza_Fabric_C_Model";
			parameters._ResourceIdentifier = "Sponza_Fabric_C_Model";
			parameters._LevelOfDetails.Emplace("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Models\\Sponza_Fabric_C.fbx");
			parameters._Transformation = Matrix4x4();
			parameters._TextureCoordinateMultiplier = 1.0f;
			parameters._TexturCoordinateRotation = 0.0f;
			parameters._ProceduralFunction = nullptr;
			parameters._CollisionModelFilePath = nullptr;

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

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\Sponza_Fabric_D_Model";
			parameters._ResourceIdentifier = "Sponza_Fabric_D_Model";
			parameters._LevelOfDetails.Emplace("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Models\\Sponza_Fabric_D.fbx");
			parameters._Transformation = Matrix4x4();
			parameters._TextureCoordinateMultiplier = 1.0f;
			parameters._TexturCoordinateRotation = 0.0f;
			parameters._ProceduralFunction = nullptr;
			parameters._CollisionModelFilePath = nullptr;

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

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\Sponza_Fabric_E_Model";
			parameters._ResourceIdentifier = "Sponza_Fabric_E_Model";
			parameters._LevelOfDetails.Emplace("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Models\\Sponza_Fabric_E.fbx");
			parameters._Transformation = Matrix4x4();
			parameters._TextureCoordinateMultiplier = 1.0f;
			parameters._TexturCoordinateRotation = 0.0f;
			parameters._ProceduralFunction = nullptr;
			parameters._CollisionModelFilePath = nullptr;

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

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\Sponza_Fabric_F_Model";
			parameters._ResourceIdentifier = "Sponza_Fabric_F_Model";
			parameters._LevelOfDetails.Emplace("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Models\\Sponza_Fabric_F.fbx");
			parameters._Transformation = Matrix4x4();
			parameters._TextureCoordinateMultiplier = 1.0f;
			parameters._TexturCoordinateRotation = 0.0f;
			parameters._ProceduralFunction = nullptr;
			parameters._CollisionModelFilePath = nullptr;

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

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\Sponza_Fabric_G_Model";
			parameters._ResourceIdentifier = "Sponza_Fabric_G_Model";
			parameters._LevelOfDetails.Emplace("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Models\\Sponza_Fabric_G.fbx");
			parameters._Transformation = Matrix4x4();
			parameters._TextureCoordinateMultiplier = 1.0f;
			parameters._TexturCoordinateRotation = 0.0f;
			parameters._ProceduralFunction = nullptr;
			parameters._CollisionModelFilePath = nullptr;

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

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\Sponza_Flagpole_Model";
			parameters._ResourceIdentifier = "Sponza_Flagpole_Model";
			parameters._LevelOfDetails.Emplace("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Models\\Sponza_Flagpole.fbx");
			parameters._Transformation = Matrix4x4();
			parameters._TextureCoordinateMultiplier = 1.0f;
			parameters._TexturCoordinateRotation = 0.0f;
			parameters._ProceduralFunction = nullptr;
			parameters._CollisionModelFilePath = nullptr;

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

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\Sponza_Floor_Model";
			parameters._ResourceIdentifier = "Sponza_Floor_Model";
			parameters._LevelOfDetails.Emplace("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Models\\Sponza_Floor.fbx");
			parameters._Transformation = Matrix4x4();
			parameters._TextureCoordinateMultiplier = 1.0f;
			parameters._TexturCoordinateRotation = 0.0f;
			parameters._ProceduralFunction = nullptr;
			parameters._CollisionModelFilePath = nullptr;

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

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\Sponza_Leaf_Model";
			parameters._ResourceIdentifier = "Sponza_Leaf_Model";
			parameters._LevelOfDetails.Emplace("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Models\\Sponza_Leaf.fbx");
			parameters._Transformation = Matrix4x4();
			parameters._TextureCoordinateMultiplier = 1.0f;
			parameters._TexturCoordinateRotation = 0.0f;
			parameters._ProceduralFunction = nullptr;
			parameters._CollisionModelFilePath = nullptr;

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

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\Sponza_Material_25_Model";
			parameters._ResourceIdentifier = "Sponza_Material_25_Model";
			parameters._LevelOfDetails.Emplace("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Models\\Sponza_Material_25.fbx");
			parameters._Transformation = Matrix4x4();
			parameters._TextureCoordinateMultiplier = 1.0f;
			parameters._TexturCoordinateRotation = 0.0f;
			parameters._ProceduralFunction = nullptr;
			parameters._CollisionModelFilePath = nullptr;

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

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\Sponza_Material_47_Model";
			parameters._ResourceIdentifier = "Sponza_Material_47_Model";
			parameters._LevelOfDetails.Emplace("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Models\\Sponza_Material_47.fbx");
			parameters._Transformation = Matrix4x4();
			parameters._TextureCoordinateMultiplier = 1.0f;
			parameters._TexturCoordinateRotation = 0.0f;
			parameters._ProceduralFunction = nullptr;
			parameters._CollisionModelFilePath = nullptr;

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

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\Sponza_Material_57_Model";
			parameters._ResourceIdentifier = "Sponza_Material_57_Model";
			parameters._LevelOfDetails.Emplace("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Models\\Sponza_Material_57.fbx");
			parameters._Transformation = Matrix4x4();
			parameters._TextureCoordinateMultiplier = 1.0f;
			parameters._TexturCoordinateRotation = 0.0f;
			parameters._ProceduralFunction = nullptr;
			parameters._CollisionModelFilePath = nullptr;

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

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\Sponza_Material_298_Model";
			parameters._ResourceIdentifier = "Sponza_Material_298_Model";
			parameters._LevelOfDetails.Emplace("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Models\\Sponza_Material_298.fbx");
			parameters._Transformation = Matrix4x4();
			parameters._TextureCoordinateMultiplier = 1.0f;
			parameters._TexturCoordinateRotation = 0.0f;
			parameters._ProceduralFunction = nullptr;
			parameters._CollisionModelFilePath = nullptr;

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

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\Sponza_Roof_Model";
			parameters._ResourceIdentifier = "Sponza_Roof_Model";
			parameters._LevelOfDetails.Emplace("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Models\\Sponza_Roof.fbx");
			parameters._Transformation = Matrix4x4();
			parameters._TextureCoordinateMultiplier = 1.0f;
			parameters._TexturCoordinateRotation = 0.0f;
			parameters._ProceduralFunction = nullptr;
			parameters._CollisionModelFilePath = nullptr;

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

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\Sponza_Vase_Model";
			parameters._ResourceIdentifier = "Sponza_Vase_Model";
			parameters._LevelOfDetails.Emplace("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Models\\Sponza_Vase.fbx");
			parameters._Transformation = Matrix4x4();
			parameters._TextureCoordinateMultiplier = 1.0f;
			parameters._TexturCoordinateRotation = 0.0f;
			parameters._ProceduralFunction = nullptr;
			parameters._CollisionModelFilePath = nullptr;

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

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\Sponza_Vase_Hanging_Model";
			parameters._ResourceIdentifier = "Sponza_Vase_Hanging_Model";
			parameters._LevelOfDetails.Emplace("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Models\\Sponza_Vase_Hanging.fbx");
			parameters._Transformation = Matrix4x4();
			parameters._TextureCoordinateMultiplier = 1.0f;
			parameters._TexturCoordinateRotation = 0.0f;
			parameters._ProceduralFunction = nullptr;
			parameters._CollisionModelFilePath = nullptr;

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

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\Sponza_Vase_Round_Model";
			parameters._ResourceIdentifier = "Sponza_Vase_Round_Model";
			parameters._LevelOfDetails.Emplace("..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Models\\Sponza_Vase_Round.fbx");
			parameters._Transformation = Matrix4x4();
			parameters._TextureCoordinateMultiplier = 1.0f;
			parameters._TexturCoordinateRotation = 0.0f;
			parameters._ProceduralFunction = nullptr;
			parameters._CollisionModelFilePath = nullptr;

			ResourceSystem::Instance->GetResourceBuildingSystem()->BuildModel(parameters);
		};
		task._Arguments = nullptr;
		task._ExecutableOnSameThread = false;

		TaskSystem::Instance->ExecuteTask(&task);
	}
#endif

#if BUILD_ENGINE_ALL || BUILD_ENGINE_MISCELLANEOUS
	{
		tasks.Emplace(new (MemorySystem::Instance->TypeAllocate<Task>()) Task());
		Task &task{ *tasks.Back() };

		task._Function = [](void* const RESTRICT)
		{
			//Gather the data.
			DynamicArray<byte> data;

			std::ifstream lookup_file{ "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Raw\\Miscellaneous\\PI Decimal Lookup.txt" };

			if (lookup_file.is_open())
			{
				char next_byte;

				while (!lookup_file.eof())
				{
					lookup_file.read(&next_byte, sizeof(char));

					if (next_byte != ' ')
					{
						data.Emplace(static_cast<byte>(next_byte - '0'));
					}
				}

				lookup_file.close();
			}

			else
			{
				ASSERT(false, "Couldn't open PI decimal lookup file!");
			}

			//Build the raw data.
			RawDataBuildParameters parameters;

			parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Extra\\PI_Decimal_Lookup";
			parameters._ResourceIdentifier = "PI_Decimal_Lookup";
			parameters._Data = &data;

			ResourceSystem::Instance->GetResourceBuildingSystem()->BuildRawData(parameters);
		};
		task._Arguments = nullptr;
		task._ExecutableOnSameThread = false;

		TaskSystem::Instance->ExecuteTask(&task);
	}
#endif

#if BUILD_ENGINE_ALL || BUILD_ENGINE_STAR_TEXTURE
	BuildStarTexture();
#endif

	//Wait for all tasks to finish.
	TaskSystem::Instance->WaitForAllTasksToFinish();

	//Free all tasks.
	for (Task *const RESTRICT task : tasks)
	{
		MemorySystem::Instance->TypeFree<Task>(task);
	}

	if (BUILD_ENGINE_ALL
		|| BUILD_ENGINE_BASE
		|| BUILD_ENGINE_FONTS
		|| BUILD_ENGINE_BLUE_NOISE_TEXTURES
		|| BUILD_ENGINE_SHADERS
		|| BUILD_ENGINE_DEFAULT_SKY_TEXTURE
		|| BUILD_ENGINE_DEFAULT_TEXTURE_2D
		|| BUILD_ENGINE_DEFAULT_TEXTURE_3D
		|| BUILD_ENGINE_MODELS
		|| BUILD_ENGINE_MATERIALS 
		| BUILD_ENGINE_RESOURCE_COLLECTIONS
		|| new_rendering_data_was_compiled)
	{
		ResourceCollectionBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Final\\CatalystEngineBaseResourceCollection";
		parameters._Folder = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base";
		parameters._MaximumFileSize = 1'000'000'000;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildResourceCollections(parameters);
	}

#if BUILD_ENGINE_ALL || BUILD_ENGINE_CLOUD_TEXTURE || BUILD_ENGINE_OCEAN_TEXTURE || BUILD_ENGINE_STAR_TEXTURE || BUILD_ENGINE_RESOURCE_COLLECTIONS
	{
		ResourceCollectionBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Final\\CatalystEngineEnvironmentResourceCollection";
		parameters._Folder = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Environment";
		parameters._MaximumFileSize = 1'000'000'000;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildResourceCollections(parameters);
	}
#endif

#if BUILD_ENGINE_ALL || BUILD_ENGINE_MISCELLANEOUS || BUILD_ENGINE_RESOURCE_COLLECTIONS
	{
		ResourceCollectionBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Final\\CatalystEngineExtraResourceCollection";
		parameters._Folder = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Extra";
		parameters._MaximumFileSize = 1'000'000'000;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildResourceCollections(parameters);
	}
#endif

	//Terminate the task system so that it can be re-initialized with the proper arguments.
	TaskSystem::Instance->Terminate();
}

/*
*	Builds the cloud texture.
*/
void CatalystEngineResourceBuilding::BuildCloudTexture() NOEXCEPT
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
	for (int8 X{ static_cast<int8>(-1) }; X <= 1; ++X)
	{
		for (int8 Y{ static_cast<int8>(-1) }; Y <= 1; ++Y)
		{
			for (int8 Z{ static_cast<int8>(-1) }; Z <= 1; ++Z)
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

	parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Environment\\Cloud_Texture3D";
	parameters._ID = "Cloud_Texture3D";
	parameters._Texture = &final_texture;

	ResourceSystem::Instance->GetResourceBuildingSystem()->BuildTexture3D(parameters);
}

/*
*	Builds the ocean texture.
*/
void CatalystEngineResourceBuilding::BuildOceanTexture() NOEXCEPT
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
	for (int8 X{ static_cast<int8>(-1) }; X <= 1; ++X)
	{
		for (int8 Y{ static_cast<int8>(-1) }; Y <= 1; ++Y)
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
	DynamicString file_name{ "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Environment\\Ocean_Texture2D" };
	file_name += ".cr";

	//Open the file to be written to.
	BinaryFile<BinaryFileMode::OUT> file{ file_name.Data() };

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
void CatalystEngineResourceBuilding::BuildDefaultSkyTexture() NOEXCEPT
{
	//What should the resource be called?
	DynamicString file_name{ "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\Catalyst_Engine_Default_TextureCube.cr" };

	//Open the file to be written to.
	BinaryFile<BinaryFileMode::OUT> file{ file_name.Data() };

	//Write the resource header to the file.
	const ResourceHeader header{ ResourceConstants::TEXTURE_CUBE_TYPE_IDENTIFIER, HashString("Catalyst_Engine_Default_TextureCube"), "Catalyst_Engine_Default_TextureCube" };
	file.Write(&header, sizeof(ResourceHeader));

	//Write the resolution to the file.
	const uint32 resolution{ 1 };
	file.Write(&resolution, sizeof(uint32));

	//Write the number of mipmap levels to the file.
	const uint8 mipmap_levels{ 1 };
	file.Write(&mipmap_levels, sizeof(uint8));

	//Write the data to the file.
	for (uint8 face_index{ 0 }; face_index < 6; ++face_index)
	{
		Vector4<float32> color{ 0.1f, 0.1f, 0.1f, 1.0f };
		file.Write(&color, sizeof(Vector4<float32>));
	}

	//Close the file.
	file.Close();
}

/*
*	Builds the default texture 3D.
*/
void CatalystEngineResourceBuilding::BuildDefaultTexture2D() NOEXCEPT
{
	//Create the texture.
	Texture2DBuildParameters parameters;

	parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\Catalyst_Engine_Default_Texture_2D";
	parameters._ID = "Catalyst_Engine_Default_Texture_2D";
	parameters._DefaultWidth = 1;
	parameters._DefaultHeight = 1;
	parameters._File1 = nullptr;
	parameters._File2 = nullptr;
	parameters._File3 = nullptr;
	parameters._File4 = nullptr;
	parameters._Default = Vector4<float32>(0.5f, 0.5f, 0.5f, 1.0f);
	parameters._ChannelMappings[0] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::DEFAULT, Texture2DBuildParameters::Channel::RED);
	parameters._ChannelMappings[1] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::DEFAULT, Texture2DBuildParameters::Channel::GREEN);
	parameters._ChannelMappings[2] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::DEFAULT, Texture2DBuildParameters::Channel::BLUE);
	parameters._ChannelMappings[3] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::DEFAULT, Texture2DBuildParameters::Channel::ALPHA);
	parameters._ApplyGammaCorrection = false;
	parameters._TransformFunction = nullptr;
	parameters._BaseMipmapLevel = 0;
	parameters._MipmapLevels = 1;

	ResourceSystem::Instance->GetResourceBuildingSystem()->BuildTexture2D(parameters);
}

/*
*	Builds the default texture 3D.
*/
void CatalystEngineResourceBuilding::BuildDefaultTexture3D() NOEXCEPT
{
	//Create the texture.
	Texture3D<Vector4<byte>> texture{ 1 };

	for (uint32 X{ 0 }; X < 1; ++X)
	{
		for (uint32 Y{ 0 }; Y < 1; ++Y)
		{
			for (uint32 Z{ 0 }; Z < 1; ++Z)
			{
				//Write to the texture.
				texture.At(X, Y, Z) = Vector4<byte>(BYTE_MAXIMUM / 2, BYTE_MAXIMUM / 2, BYTE_MAXIMUM / 2, BYTE_MAXIMUM / 2);
			}
		}
	}

	Texture3DBuildParameters parameters;

	parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\Catalyst_Engine_Default_Texture_3D";
	parameters._ID = "Catalyst_Engine_Default_Texture_3D";
	parameters._Texture = &texture;

	ResourceSystem::Instance->GetResourceBuildingSystem()->BuildTexture3D(parameters);
}

/*
*	Builds the star texture.
*/
void CatalystEngineResourceBuilding::BuildStarTexture() NOEXCEPT
{
	//Define constants.
	constexpr uint64 NUMBER_OF_STARS{ 1'024 * 256 };
	constexpr float32 MINIMUM_DISTANCE_FROM_CENTER{ 2'048.0f };
	constexpr float32 MINIMUM_DISTANCE_FROM_CENTER_SQUARED{ MINIMUM_DISTANCE_FROM_CENTER * MINIMUM_DISTANCE_FROM_CENTER };
	constexpr float32 MAXIMUM_DISTANCE_FROM_CENTER{ 8'192.0f };
	constexpr float32 MAXIMUM_DISTANCE_FROM_CENTER_SQUARED{ MAXIMUM_DISTANCE_FROM_CENTER * MAXIMUM_DISTANCE_FROM_CENTER };
	constexpr float32 MINIMUM_RADIUS{ 1.0f };
	constexpr float32 MAXIMUM_RADIUS{ 2.0f };
	constexpr float32 MINIMUM_INTENSITY{ 2.0f };
	constexpr float32 MAXIMUM_INTENSITY{ 4.0f };

	/*
	*	Star class definition.
	*/
	class Star final
	{

	public:

		//The sphere.
		Sphere _Sphere;

		//The intensity.
		float32 _Intensity;

	};

	//Scatter some stars. (:
	DynamicArray<Star> stars;

	for (uint64 i{ 0 }; i < NUMBER_OF_STARS; ++i)
	{
		//Generate a random position.
		const Vector3<float32> position{ CatalystRandomMath::RandomPointInSphere(MAXIMUM_DISTANCE_FROM_CENTER) };

		//Calculate the squared distance from the center.
		const float32 distance_from_center{ Vector3<float32>::Length(position) };

		//Discard if too close.
		if (distance_from_center < MINIMUM_DISTANCE_FROM_CENTER)
		{
			continue;
		}

		//Construct the star!
		stars.Emplace();
		Star &new_star{ stars.Back() };

		new_star._Sphere._Position = position;
		new_star._Sphere._Radius = CatalystRandomMath::RandomFloatInRange(MINIMUM_RADIUS, MAXIMUM_RADIUS);
		new_star._Intensity = CatalystRandomMath::RandomFloatInRange(MINIMUM_INTENSITY, MAXIMUM_INTENSITY);
	}

#if 0 //Debug the stars.
	{
		//Define constants.
		constexpr uint32 TEXTURE_RESOLUTION{ 1'024 };

		for (uint8 face_index{ 0 }; face_index < 6; ++face_index)
		{
			Vector3<float32> ray_direction;
			Vector3<float32> ray_opposite_direction;

			switch (face_index)
			{
				//Left.
				case 0:
				{
					ray_direction = Vector3<float32>(1.0f, 0.0f, 0.0f);
					ray_opposite_direction = -ray_direction;

					break;
				}

				//Right.
				case 1:
				{
					ray_direction = Vector3<float32>(-1.0f, 0.0f, 0.0f);
					ray_opposite_direction = -ray_direction;

					break;
				}

				//Down.
				case 2:
				{
					ray_direction = Vector3<float32>(0.0f, 1.0f, 0.0f);
					ray_opposite_direction = -ray_direction;

					break;
				}

				//Up.
				case 3:
				{
					ray_direction = Vector3<float32>(0.0f, -1.0f, 0.0f);
					ray_opposite_direction = -ray_direction;

					break;
				}

				//Backward.
				case 4:
				{
					ray_direction = Vector3<float32>(0.0f, 0.0f, 1.0f);
					ray_opposite_direction = -ray_direction;

					break;
				}

				//Forward.
				case 5:
				{
					ray_direction = Vector3<float32>(0.0f, 0.0f, -1.0f);
					ray_opposite_direction = -ray_direction;

					break;
				}

				default:
				{
					ASSERT(false, "Invalid case!");

					break;
				}
			}

			//Set up the ray.
			Ray ray;

			ray.SetDirection(ray_direction);

			//Set up the texture.
			Texture2D<Vector4<float32>> texture;
			texture.Initialize(TEXTURE_RESOLUTION);

			for (uint32 Y{ 0 }; Y < TEXTURE_RESOLUTION; ++Y)
			{
				for (uint32 X{ 0 }; X < TEXTURE_RESOLUTION; ++X)
				{
					//Calculate the normalized coordinate.
					const Vector2<float32> normalized_coordinate{	(static_cast<float32>(X) + 0.5f) / static_cast<float32>(TEXTURE_RESOLUTION),
																	(static_cast<float32>(Y) + 0.5f) / static_cast<float32>(TEXTURE_RESOLUTION) };

					//Calculate the ray offset.
					Vector3<float32> ray_offset;

					if (ray_direction._X != 0.0f)
					{
						ray_offset._X = 0.0f;
						ray_offset._Y = CatalystBaseMath::LinearlyInterpolate(-MAXIMUM_DISTANCE_FROM_CENTER, MAXIMUM_DISTANCE_FROM_CENTER, normalized_coordinate._X);
						ray_offset._Z = CatalystBaseMath::LinearlyInterpolate(-MAXIMUM_DISTANCE_FROM_CENTER, MAXIMUM_DISTANCE_FROM_CENTER, normalized_coordinate._Y);
					}

					else if (ray_direction._Y != 0.0f)
					{
						ray_offset._X = CatalystBaseMath::LinearlyInterpolate(-MAXIMUM_DISTANCE_FROM_CENTER, MAXIMUM_DISTANCE_FROM_CENTER, normalized_coordinate._X);
						ray_offset._Y = 0.0f;
						ray_offset._Z = CatalystBaseMath::LinearlyInterpolate(-MAXIMUM_DISTANCE_FROM_CENTER, MAXIMUM_DISTANCE_FROM_CENTER, normalized_coordinate._Y);
					}

					else
					{
						ray_offset._X = CatalystBaseMath::LinearlyInterpolate(-MAXIMUM_DISTANCE_FROM_CENTER, MAXIMUM_DISTANCE_FROM_CENTER, normalized_coordinate._X);
						ray_offset._Y = CatalystBaseMath::LinearlyInterpolate(-MAXIMUM_DISTANCE_FROM_CENTER, MAXIMUM_DISTANCE_FROM_CENTER, normalized_coordinate._Y);
						ray_offset._Z = 0.0f;
					}

					//Set the ray origin.
					ray.SetOrigin(ray_opposite_direction * MAXIMUM_DISTANCE_FROM_CENTER + ray_offset);

					//Raycast the stars. (:
					bool hit{ false };

					for (const Star &star : stars)
					{
						if (CatalystGeometryMath::RaySphereIntersection(ray, star._Sphere, nullptr))
						{
							hit = true;

							break;
						}
					}

					if (hit)
					{
						texture.At(X, Y) = Vector4<float32>(1.0f, 1.0f, 1.0f, 1.0f);
					}

					else
					{
						texture.At(X, Y) = Vector4<float32>(0.0f, 0.0f, 0.0f, 1.0f);
					}
				}
			}

			//Write the texture.
			char buffer[MAXIMUM_FILE_PATH_LENGTH];
			sprintf_s(buffer, "Star_Texture_Debug_%u.png", static_cast<uint32>(face_index + 1));

			PNGWriter::Write(texture, buffer);

			PRINT_TO_OUTPUT("Wrote " << buffer);
		}

		BREAKPOINT();
	}
#endif

	//Build the texture cube.
	TextureCubeBuildParameters parameters;

	parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Environment\\Catalyst_Engine_Star_TextureCube";
	parameters._ID = "Catalyst_Engine_Star_TextureCube";
	parameters._File = nullptr;
	parameters._DefaultResolution = 1'024 / 4;
	parameters._ProceduralFunction = [](const Vector3<float32> &direction, void *const RESTRICT user_data)
	{
		/*
		const float32 left_weight{ CatalystBaseMath::Maximum<float32>(Vector3<float32>::DotProduct(Vector3<float32>(-1.0f, 0.0f, 0.0f), direction), 0.0f) };
		const float32 right_weight{ CatalystBaseMath::Maximum<float32>(Vector3<float32>::DotProduct(Vector3<float32>(1.0f, 0.0f, 0.0f), direction), 0.0f) };
		const float32 down_weight{ CatalystBaseMath::Maximum<float32>(Vector3<float32>::DotProduct(Vector3<float32>(0.0f, -1.0f, 0.0f), direction), 0.0f) };
		const float32 up_weight{ CatalystBaseMath::Maximum<float32>(Vector3<float32>::DotProduct(Vector3<float32>(0.0f, 1.0f, 0.0f), direction), 0.0f) };
		const float32 backward_weight{ CatalystBaseMath::Maximum<float32>(Vector3<float32>::DotProduct(Vector3<float32>(0.0f, 0.0f, -1.0f), direction), 0.0f) };
		const float32 forward_weight{ CatalystBaseMath::Maximum<float32>(Vector3<float32>::DotProduct(Vector3<float32>(0.0f, 0.0f, -1.0f), direction), 0.0f) };

		Vector3<float32> final_color
		{
			Vector3<float32>(1.0f, 0.0f, 0.0f) * left_weight
			+ Vector3<float32>(0.0f, 1.0f, 1.0f) * right_weight
			+ Vector3<float32>(0.0f, 1.0f, 0.0f) * down_weight
			+ Vector3<float32>(1.0f, 0.0f, 1.0f) * up_weight
			+ Vector3<float32>(0.0f, 0.0f, 1.0f) * backward_weight
			+ Vector3<float32>(1.0f, 1.0f, 0.0f) * forward_weight
		};

		final_color.Normalize();

		return Vector4<float32>(final_color, 1.0f);
		*/

		//return Vector4<float32>(direction._X * 0.5f + 0.5f, direction._Y * 0.5f + 0.5f, direction._Z * 0.5f + 0.5f, 1.0f);

		//Cache the stars.
		const DynamicArray<Star> &stars{ *static_cast<const DynamicArray<Star> *const RESTRICT>(user_data) };

		//Construct the ray.
		Ray ray;

		ray.SetOrigin(Vector3<float32>(0.0f, 0.0f, 0.0f));
		ray.SetDirection(direction);

		//Iterate over all the stars and see if any hit.
		float32 closest_intersection_distance{ FLOAT32_MAXIMUM };
		float32 closest_intensity;

		for (const Star &star : stars)
		{
			float32 intersection_distance{ FLOAT32_MAXIMUM };
			const bool hit{ CatalystGeometryMath::RaySphereIntersection(ray, star._Sphere, &intersection_distance) };

			if (hit
				&& closest_intersection_distance > intersection_distance)
			{
				closest_intersection_distance = intersection_distance;
				closest_intensity = star._Intensity;
			}
		}

		if (closest_intersection_distance != FLOAT32_MAXIMUM)
		{
			return Vector4<float32>(closest_intensity, closest_intensity, closest_intensity, 1.0f);
		}
		
		else
		{
			return Vector4<float32>(0.0f, 0.0f, 0.0f, 0.0f);
		}
	};
	parameters._ProceduralFunctionUserData = &stars;
	parameters._ProceduralFunctionSuperSample = true;

	ResourceSystem::Instance->GetResourceBuildingSystem()->BuildTextureCube(parameters);

	BREAKPOINT();
}
#endif