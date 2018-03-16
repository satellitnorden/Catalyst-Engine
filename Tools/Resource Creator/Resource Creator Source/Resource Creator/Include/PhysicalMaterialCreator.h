#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

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
	static void CreatePhysicalMaterial(const int32 argumentCount, char *RESTRICT arguments[]) noexcept
	{
		//What should the material be called?
		DynamicString fileName{ arguments[2] };
		fileName += ".cr";

		//Open the file to be written to.
		BinaryFile<IOMode::Out> file{ fileName.CString() };

		//Write the resource type to the file.
		constexpr ResourceType resourceType{ ResourceType::PhysicalMaterial };
		file.Write(&resourceType, sizeof(ResourceType));

		//Write the resource ID to the file.
		const uint64 resourceID{ std::strtoull(arguments[3], nullptr, 0) };
		file.Write(&resourceID, sizeof(uint64));

		//Determine how many mipmap levels that should be generated.
		const uint8 numberOfMipmapLevels{ static_cast<uint8>(*arguments[4] - '0') };

		//Write the number of mipmap levels to the file.
		file.Write(&numberOfMipmapLevels, sizeof(uint8));

		//Load the albedo.
		int32 width, height, numberOfChannels;
		byte *RESTRICT data{ stbi_load(arguments[5], &width, &height, &numberOfChannels, STBI_rgb_alpha) };

		//Write the width and height of the albedo into the file, to be read into uint32's.
		file.Write(&width, sizeof(int32));
		file.Write(&height, sizeof(int32));

		//Write the albedo to the file.
		uint64 textureSize{ static_cast<uint64>(width * height * 4) };

		for (uint8 i = 0; i < numberOfMipmapLevels; ++i)
		{
			//If this is the base mipmap level, just copy the thing directly into memory.
			if (i == 0)
			{
				file.Write(data, textureSize);
			}

			//Else, the image data should be resized.
			else
			{
				byte *RESTRICT downsampledData = static_cast<byte *RESTRICT>(MemoryUtilities::AllocateMemory(textureSize >> i));
				stbir_resize_uint8(data, width, height, 0, downsampledData, width >> i, height >> i, 0, 4);

				file.Write(downsampledData, textureSize >> i);

				MemoryUtilities::FreeMemory(downsampledData);
			}
		}

		//Free the layer albedo data.
		stbi_image_free(data);

		//Load the normal map.
		data = stbi_load(arguments[6], &width, &height, &numberOfChannels, STBI_rgb_alpha);

		//Write the layer albedo to the file, to be read into byte's.
		for (uint8 i = 0; i < numberOfMipmapLevels; ++i)
		{
			//If this is the base mipmap level, just copy the thing directly into memory.
			if (i == 0)
			{
				file.Write(data, textureSize);
			}

			//Else, the image data should be resized.
			else
			{
				byte *RESTRICT downsampledData = static_cast<byte *RESTRICT>(MemoryUtilities::AllocateMemory(textureSize >> i));
				stbir_resize_uint8(data, width, height, 0, downsampledData, width >> i, height >> i, 0, 4);

				file.Write(downsampledData, textureSize >> i);

				MemoryUtilities::FreeMemory(downsampledData);
			}
		}

		//Free the layer normal map data.
		stbi_image_free(data);

		//Load the roughness, metallic, ambient occlusion and displacement data.
		byte *RESTRICT roughnessData = stbi_load(arguments[7], &width, &height, &numberOfChannels, 0);
		byte *RESTRICT metallicData = stbi_load(arguments[8], &width, &height, &numberOfChannels, 0);
		byte *RESTRICT ambientOcclusionData = stbi_load(arguments[9], &width, &height, &numberOfChannels, 0);
		byte *RESTRICT displacementData = stbi_load(arguments[10], &width, &height, &numberOfChannels, 0);

		//Write the roughness, metallic, ambient occlusion and displacement data to the file.
		constexpr byte defaultRoughness{ 255 };
		constexpr byte defaultMetallic{ 0 };
		constexpr byte defaultAmbientOcclusion{ 255 };
		constexpr byte defaultDisplacement{ 0 };

		textureSize = static_cast<uint64>(width * height);

		for (uint8 i = 0; i < numberOfMipmapLevels; ++i)
		{
			//If this is the base mipmap level, treat it differently.
			if (i == 0)
			{
				for (uint64 j = 0; j < textureSize; ++j)
				{
					file.Write(roughnessData ? &roughnessData[j] : &defaultRoughness, sizeof(byte));
					file.Write(metallicData ? &metallicData[j] : &defaultMetallic, sizeof(byte));
					file.Write(ambientOcclusionData ? &ambientOcclusionData[j] : &defaultAmbientOcclusion, sizeof(byte));
					file.Write(displacementData ? &displacementData[j] : &defaultDisplacement, sizeof(byte));
				}
			}

			else
			{
				byte *RESTRICT downsampledRoughnessData = roughnessData ? static_cast<byte *RESTRICT >(MemoryUtilities::AllocateMemory(textureSize >> i)) : nullptr;
				byte *RESTRICT downsampledMetallicData = metallicData ? static_cast<byte *RESTRICT >(MemoryUtilities::AllocateMemory(textureSize >> i)) : nullptr;
				byte *RESTRICT downsampledAmbientOcclusionData = ambientOcclusionData ? static_cast<byte *RESTRICT >(MemoryUtilities::AllocateMemory(textureSize >> i)) : nullptr;
				byte *RESTRICT downsampledDisplacementData = displacementData ? static_cast<byte *RESTRICT >(MemoryUtilities::AllocateMemory(textureSize >> i)) : nullptr;

				if (roughnessData) stbir_resize_uint8(roughnessData, width, height, 0, downsampledRoughnessData, width >> i, height >> i, 0, 1);
				if (metallicData) stbir_resize_uint8(metallicData, width, height, 0, downsampledMetallicData, width >> i, height >> i, 0, 1);
				if (ambientOcclusionData) stbir_resize_uint8(ambientOcclusionData, width, height, 0, downsampledAmbientOcclusionData, width >> i, height >> i, 0, 1);
				if (displacementData) stbir_resize_uint8(displacementData, width, height, 0, downsampledDisplacementData, width >> i, height >> i, 0, 1);

				for (uint64 j = 0; j < textureSize >> i; ++j)
				{
					file.Write(downsampledRoughnessData ? &downsampledRoughnessData[j] : &defaultRoughness, sizeof(byte));
					file.Write(downsampledMetallicData ? &downsampledMetallicData[j] : &defaultMetallic, sizeof(byte));
					file.Write(downsampledAmbientOcclusionData ? &downsampledAmbientOcclusionData[j] : &defaultAmbientOcclusion, sizeof(byte));
					file.Write(downsampledDisplacementData ? &downsampledDisplacementData[j] : &defaultDisplacement, sizeof(byte));
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