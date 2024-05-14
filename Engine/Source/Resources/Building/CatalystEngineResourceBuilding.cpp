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
#include <File/Writers/PNGWriter.h>

//Math.
#include <Math/Core/CatalystGeometryMath.h>
#include <Math/Core/CatalystRandomMath.h>
#include <Math/Geometry/Sphere.h>

//Rendering.
#include <Rendering/Native/Compilation/RenderingCompiler.h>
#include <Rendering/Native/Texture2D.h>

//Resources.
#include <Resources/Core/ResourcesCore.h>

//Systems.
#include <Systems/ContentSystem.h>
#include <Systems/MemorySystem.h>
#include <Systems/ResourceSystem.h>
#include <Systems/TaskSystem.h>

//Defines.
#define CONTENT_INTERMEDIATE "..\\..\\..\\..\\..\\Catalyst-Engine\\Engine\\Content\\Intermediate\\"
#define CONTENT_RAW "..\\..\\..\\..\\..\\Catalyst-Engine\\Engine\\Content\\Raw\\"

#define BUILD_ENGINE_ALL (0)

#define BUILD_ENGINE_BASE (0)
#define BUILD_ENGINE_CLOUD_TEXTURE (0)
#define BUILD_ENGINE_BLUE_NOISE_TEXTURES (0)
#define BUILD_ENGINE_SHADERS (0)
#define BUILD_ENGINE_DEFAULT_TEXTURE_3D (0)
#define BUILD_ENGINE_MATERIALS (0)
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

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\IndirectLightingTemporalDenoisingFragmentShader";
		parameters._ID = "IndirectLightingTemporalDenoisingFragmentShader";
		parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\IndirectLightingTemporalDenoisingFragmentShader.frag";
		parameters._Stage = ShaderStage::FRAGMENT;

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

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Resources\\Intermediate\\Base\\ViewportVertexShader";
		parameters._ID = "ViewportVertexShader";
		parameters._FilePath = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Shaders\\ViewportVertexShader.vert";
		parameters._Stage = ShaderStage::VERTEX;

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

	//Wait for all tasks to finish.
	TaskSystem::Instance->WaitForAllTasksToFinish();

	//Free all tasks.
	for (Task *const RESTRICT task : tasks)
	{
		MemorySystem::Instance->TypeFree<Task>(task);
	}

	if (BUILD_ENGINE_ALL
		|| BUILD_ENGINE_BASE
		|| BUILD_ENGINE_BLUE_NOISE_TEXTURES
		|| BUILD_ENGINE_SHADERS
		|| BUILD_ENGINE_DEFAULT_TEXTURE_3D
		|| BUILD_ENGINE_MATERIALS 
		|| BUILD_ENGINE_RESOURCE_COLLECTIONS
		|| new_rendering_data_was_compiled)
	{
		ResourceCollectionBuildParameters parameters;

		parameters._Output = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Content\\Final\\CatalystEngineBaseResourceCollection";
		parameters._Folder = "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Content\\Intermediate\\Base";
		parameters._MaximumFileSize = 1'000'000'000;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildResourceCollections(parameters);
	}

#if BUILD_ENGINE_ALL || BUILD_ENGINE_CLOUD_TEXTURE || BUILD_ENGINE_RESOURCE_COLLECTIONS
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
	StaticArray<float, 4> longest_distances{ -FLOAT32_MAXIMUM, -FLOAT32_MAXIMUM, -FLOAT32_MAXIMUM, -FLOAT32_MAXIMUM };

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
					float closest_distance{ FLOAT32_MAXIMUM };

					for (const Vector3<float>& point : points[i])
					{
						const float distance{ Vector3<float>::Length(position - point) };
						closest_distance = BaseMath::Minimum<float>(closest_distance, distance);
					}

					//Write to the texture.
					temporary_texture.At(X, Y, Z)[i] = closest_distance;

					//Update the longest distance.
					longest_distances[i] = BaseMath::Maximum<float>(longest_distances[i], closest_distance);
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
#endif