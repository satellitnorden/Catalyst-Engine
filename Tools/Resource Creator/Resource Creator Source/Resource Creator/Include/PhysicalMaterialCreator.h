#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>
#include <Engine Core/HashString.h>

//Resources
#include <Resources/ResourcesCore.h>

//Third party libraries.
#include <stb_image.h>
#include <stb_image_resize.h>

class PhysicalMaterialCreator final
{

public:

	/*
	*	Creates a physical material resource file.
	*/
	static void CreatePhysicalMaterial(const char *const RESTRICT arguments[]) noexcept
	{
		//What should the material be called?
		DynamicString fileName{ arguments[0] };
		fileName += ".cr";

		//Open the file to be written to.
		BinaryFile<IOMode::Out> file{ fileName.CString() };

		//Write the resource type to the file.
		constexpr ResourceType resourceType{ ResourceType::PhysicalMaterial };
		file.Write(&resourceType, sizeof(ResourceType));

		//Write the resource ID to the file.
		const HashString resourceID{ arguments[1] };
		file.Write(&resourceID, sizeof(HashString));

		//Determine how many mipmap levels that should be generated.
		const uint8 numberOfMipmapLevels{ static_cast<uint8>(*arguments[2] - '0') };

		//Write the number of mipmap levels to the file.
		file.Write(&numberOfMipmapLevels, sizeof(uint8));

		//Load the albedo.
		int32 width, height, numberOfChannels;
		byte *RESTRICT data{ stbi_load(arguments[3], &width, &height, &numberOfChannels, STBI_rgb_alpha) };

		const uint32 uWidth{ static_cast<uint32>(width) };
		const uint32 uHeight{ static_cast<uint32>(height) };

		//Write the width and height of the albedo into the file, to be read into uint32's.
		file.Write(&uWidth, sizeof(uint32));
		file.Write(&uHeight, sizeof(uint32));

		//Write the albedo to the file.
		for (uint8 i = 0; i < numberOfMipmapLevels; ++i)
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

		//Free the layer albedo data.
		stbi_image_free(data);

		//Load the normal map.
		data = stbi_load(arguments[4], &width, &height, &numberOfChannels, STBI_rgb_alpha);

		//Write the layer albedo to the file, to be read into byte's.
		for (uint8 i = 0; i < numberOfMipmapLevels; ++i)
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

		//Free the layer normal map data.
		stbi_image_free(data);

		//Load the roughness, metallic, ambient occlusion and displacement data.
		byte *RESTRICT roughnessData = stbi_load(arguments[5], &width, &height, &numberOfChannels, STBI_rgb_alpha);
		byte *RESTRICT metallicData = stbi_load(arguments[6], &width, &height, &numberOfChannels, STBI_rgb_alpha);
		byte *RESTRICT ambientOcclusionData = stbi_load(arguments[7], &width, &height, &numberOfChannels, STBI_rgb_alpha);
		byte *RESTRICT displacementData = stbi_load(arguments[8], &width, &height, &numberOfChannels, STBI_rgb_alpha);

		//Write the roughness, metallic, ambient occlusion and displacement data to the file.
		constexpr byte defaultRoughness{ 255 };
		constexpr byte defaultMetallic{ 0 };
		constexpr byte defaultAmbientOcclusion{ 255 };
		constexpr byte defaultDisplacement{ 0 };

		for (uint8 i = 0; i < numberOfMipmapLevels; ++i)
		{
			const uint64 textureSize{ (uWidth >> i) * (uHeight >> i) };

			//If this is the base mipmap level, treat it differently.
			if (i == 0)
			{
				for (uint64 j = 0; j < textureSize; ++j)
				{
					file.Write(roughnessData ? &roughnessData[j * 4] : &defaultRoughness, sizeof(byte));
					file.Write(metallicData ? &metallicData[j * 4] : &defaultMetallic, sizeof(byte));
					file.Write(ambientOcclusionData ? &ambientOcclusionData[j * 4] : &defaultAmbientOcclusion, sizeof(byte));
					file.Write(displacementData ? &displacementData[j * 4] : &defaultDisplacement, sizeof(byte));
				}
			}

			else
			{
				byte *RESTRICT downsampledRoughnessData = roughnessData ? static_cast<byte *RESTRICT >(MemoryUtilities::AllocateMemory(textureSize * 4)) : nullptr;
				byte *RESTRICT downsampledMetallicData = metallicData ? static_cast<byte *RESTRICT >(MemoryUtilities::AllocateMemory(textureSize * 4)) : nullptr;
				byte *RESTRICT downsampledAmbientOcclusionData = ambientOcclusionData ? static_cast<byte *RESTRICT >(MemoryUtilities::AllocateMemory(textureSize * 4)) : nullptr;
				byte *RESTRICT downsampledDisplacementData = displacementData ? static_cast<byte *RESTRICT >(MemoryUtilities::AllocateMemory(textureSize * 4)) : nullptr;

				if (roughnessData) stbir_resize_uint8(roughnessData, width, height, 0, downsampledRoughnessData, uWidth >> i, uHeight >> i, 0, 4);
				if (metallicData) stbir_resize_uint8(metallicData, width, height, 0, downsampledMetallicData, uWidth >> i, uHeight >> i, 0, 4);
				if (ambientOcclusionData) stbir_resize_uint8(ambientOcclusionData, width, height, 0, downsampledAmbientOcclusionData, uWidth >> i, uHeight >> i, 0, 4);
				if (displacementData) stbir_resize_uint8(displacementData, width, height, 0, downsampledDisplacementData, uWidth >> i, uHeight >> i, 0, 4);

				for (uint64 j = 0; j < textureSize; ++j)
				{
					file.Write(downsampledRoughnessData ? &downsampledRoughnessData[j * 4] : &defaultRoughness, sizeof(byte));
					file.Write(downsampledMetallicData ? &downsampledMetallicData[j * 4] : &defaultMetallic, sizeof(byte));
					file.Write(downsampledAmbientOcclusionData ? &downsampledAmbientOcclusionData[j * 4] : &defaultAmbientOcclusion, sizeof(byte));
					file.Write(downsampledDisplacementData ? &downsampledDisplacementData[j * 4] : &defaultDisplacement, sizeof(byte));
				}

				MemoryUtilities::FreeMemory(downsampledRoughnessData);
				MemoryUtilities::FreeMemory(downsampledMetallicData);
				MemoryUtilities::FreeMemory(downsampledAmbientOcclusionData);
				MemoryUtilities::FreeMemory(downsampledDisplacementData);
			}
		}

		//Free the roughness, metallic, ambient occlusion and displacement data.
		stbi_image_free(roughnessData);
		stbi_image_free(metallicData);
		stbi_image_free(ambientOcclusionData);
		stbi_image_free(displacementData);

		//Close the file.
		file.Close();
	}

};