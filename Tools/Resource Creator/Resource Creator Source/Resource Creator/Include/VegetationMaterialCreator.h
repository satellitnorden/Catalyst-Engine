#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>
#include <Engine Core/HashString.h>

//Resources
#include <Resources/ResourcesCore.h>

//Third party libraries.
#include <stb_image.h>
#include <stb_image_resize.h>

class VegetationMaterialCreator final
{

public:

	/*
	*	Creates a vegetation material resource file.
	*/
	static void CreateVegetationMaterial(const int32 argumentCount, char *RESTRICT arguments[]) noexcept
	{
		//What should the material be called?
		DynamicString fileName{ arguments[2] };
		fileName += ".cr";

		//Open the file to be written to.
		BinaryFile<IOMode::Out> file{ fileName.CString() };

		//Write the resource type to the file.
		constexpr ResourceType resourceType{ ResourceType::VegetationMaterial };
		file.Write(&resourceType, sizeof(ResourceType));

		//Write the resource ID to the file.
		const HashString resourceID{ arguments[3] };
		file.Write(&resourceID, sizeof(HashString));

		//Determine how many mipmap levels that should be generated for the mask texture.
		const uint8 numberOfMaskMipmapLevels{ static_cast<uint8>(std::stoul(arguments[4])) };

		//Determine how many mipmap levels that should be generated for the remaining texture.
		const uint8 numberOfRemainingMipmapLevels{ static_cast<uint8>(std::stoul(arguments[5])) };

		//Write the number of mipmap levels to the file.
		file.Write(&numberOfMaskMipmapLevels, sizeof(uint8));
		file.Write(&numberOfRemainingMipmapLevels, sizeof(uint8));

		//Load the mask texture.
		int32 width, height, numberOfChannels;
		byte *RESTRICT data{ stbi_load(arguments[6], &width, &height, &numberOfChannels, STBI_rgb_alpha) };

		const uint32 uWidth{ static_cast<uint32>(width) };
		const uint32 uHeight{ static_cast<uint32>(height) };

		//Write the width and height of the albedo into the file, to be read into uint32's.
		file.Write(&uWidth, sizeof(uint32));
		file.Write(&uHeight, sizeof(uint32));

		//Write the mask to the file.
		for (uint8 i = 0; i < numberOfMaskMipmapLevels; ++i)
		{
			const uint64 textureSize{ (uWidth >> i) * (uHeight >> i) * 4 };

			//If this is the base mipmap level, just copy the thing directly into memory.
			if (i == 0)
			{
				file.Write(data, textureSize);
			}

			//Else, the image data should be resized.
			else
			{
				byte *RESTRICT downsampledData = static_cast<byte *RESTRICT>(MemoryUtilities::AllocateMemory(textureSize));
				stbir_resize_uint8(data, width, height, 0, downsampledData, uWidth >> i, uHeight >> i, 0, 4);

				file.Write(downsampledData, textureSize);

				MemoryUtilities::FreeMemory(downsampledData);
			}
		}

		//Free the mask data.
		stbi_image_free(data);

		//Load the albedo texture.
		data = stbi_load(arguments[7], &width, &height, &numberOfChannels, STBI_rgb_alpha);

		//Write the albedo to the file.
		for (uint8 i = 0; i < numberOfRemainingMipmapLevels; ++i)
		{
			const uint64 textureSize{ (width >> i) * (uHeight >> i) * 4 };

			//If this is the base mipmap level, just copy the thing directly into memory.
			if (i == 0)
			{
				file.Write(data, textureSize);
			}

			//Else, the image data should be resized.
			else
			{
				byte *RESTRICT downsampledData = static_cast<byte *RESTRICT>(MemoryUtilities::AllocateMemory(textureSize));
				stbir_resize_uint8(data, width, height, 0, downsampledData, uWidth >> i, uHeight >> i, 0, 4);

				file.Write(downsampledData, textureSize);

				MemoryUtilities::FreeMemory(downsampledData);
			}
		}

		//Free the albedo data.
		stbi_image_free(data);

		//Load the normal map texture.
		data = stbi_load(arguments[8], &width, &height, &numberOfChannels, STBI_rgb_alpha);

		//Write the normal map to the file.
		for (uint8 i = 0; i < numberOfRemainingMipmapLevels; ++i)
		{
			const uint64 textureSize{ (uWidth >> i) * (uHeight >> i) * 4 };

			//If this is the base mipmap level, just copy the thing directly into memory.
			if (i == 0)
			{
				file.Write(data, textureSize);
			}

			//Else, the image data should be resized.
			else
			{
				byte *RESTRICT downsampledData = static_cast<byte *RESTRICT>(MemoryUtilities::AllocateMemory(textureSize));
				stbir_resize_uint8(data, width, height, 0, downsampledData, uWidth >> i, uHeight >> i, 0, 4);

				file.Write(downsampledData, textureSize);

				MemoryUtilities::FreeMemory(downsampledData);
			}
		}

		//Free the normal map data.
		stbi_image_free(data);

		//Load the roughness, ambient occlusion and thinness data.
		byte *RESTRICT roughnessData = stbi_load(arguments[9], &width, &height, &numberOfChannels, STBI_rgb_alpha);
		byte *RESTRICT ambientOcclusionData = stbi_load(arguments[10], &width, &height, &numberOfChannels, STBI_rgb_alpha);
		byte *RESTRICT thinnessData = stbi_load(arguments[11], &width, &height, &numberOfChannels, STBI_rgb_alpha);

		//Write the roughness, ambient occlusion and thinness data to the file.
		constexpr byte defaultRoughness{ 255 };
		constexpr byte defaultMetallic{ 0 };
		constexpr byte defaultAmbientOcclusion{ 255 };
		constexpr byte defaultThinness{ 0 };

		for (uint8 i = 0; i < numberOfRemainingMipmapLevels; ++i)
		{
			const uint64 textureSize{ (width >> i) * (uHeight >> i) };

			//If this is the base mipmap level, treat it differently.
			if (i == 0)
			{
				for (uint64 j = 0; j < textureSize; ++j)
				{
					file.Write(roughnessData ? &roughnessData[j * 4] : &defaultRoughness, sizeof(byte));
					file.Write(&defaultMetallic, sizeof(byte));
					file.Write(ambientOcclusionData ? &ambientOcclusionData[j * 4] : &defaultAmbientOcclusion, sizeof(byte));
					file.Write(thinnessData ? &thinnessData[j * 4] : &defaultThinness, sizeof(byte));
				}
			}

			else
			{
				byte *RESTRICT downsampledRoughnessData = roughnessData ? static_cast<byte *RESTRICT >(MemoryUtilities::AllocateMemory(textureSize * 4)) : nullptr;
				byte *RESTRICT downsampledAmbientOcclusionData = ambientOcclusionData ? static_cast<byte *RESTRICT >(MemoryUtilities::AllocateMemory(textureSize * 4)) : nullptr;
				byte *RESTRICT downsampledThinnessData = thinnessData ? static_cast<byte *RESTRICT >(MemoryUtilities::AllocateMemory(textureSize * 4)) : nullptr;

				if (roughnessData) stbir_resize_uint8(roughnessData, width, height, 0, downsampledRoughnessData, uWidth >> i, uHeight >> i, 0, 4);
				if (ambientOcclusionData) stbir_resize_uint8(ambientOcclusionData, width, height, 0, downsampledAmbientOcclusionData, uWidth >> i, uHeight >> i, 0, 4);
				if (thinnessData) stbir_resize_uint8(thinnessData, width, height, 0, downsampledThinnessData, uWidth >> i, uHeight >> i, 0, 4);

				for (uint64 j = 0; j < textureSize; ++j)
				{
					file.Write(downsampledRoughnessData ? &downsampledRoughnessData[j * 4] : &defaultRoughness, sizeof(byte));
					file.Write(&defaultMetallic, sizeof(byte));
					file.Write(downsampledAmbientOcclusionData ? &downsampledAmbientOcclusionData[j * 4] : &defaultAmbientOcclusion, sizeof(byte));
					file.Write(downsampledThinnessData ? &downsampledThinnessData[j * 4] : &defaultThinness, sizeof(byte));
				}

				MemoryUtilities::FreeMemory(downsampledRoughnessData);
				MemoryUtilities::FreeMemory(downsampledAmbientOcclusionData);
				MemoryUtilities::FreeMemory(downsampledThinnessData);
			}
		}

		//Free the roughness, ambient occlusion and thinness data.
		stbi_image_free(roughnessData);
		stbi_image_free(ambientOcclusionData);
		stbi_image_free(thinnessData);

		//Close the file.
		file.Close();
	}

};