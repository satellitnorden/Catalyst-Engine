#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/HashString.h>

//Resources
#include <Resources/ResourcesCore.h>

//Third party libraries.
#include <stb_image.h>
#include <stb_image_resize.h>

class PhysicalMaterialCreator final
{

public:

	class PhysicalMaterialCreationParameters final
	{

	public:

		//The output file path.
		const char *RESTRICT _Output;

		//The resource id.
		const char *RESTRICT _ID;

		//The number of mipmap levels.
		uint8 _MipmapLevels;

		//The albedo file path.
		const char *RESTRICT _AlbedoFile;

		//The normal map file path.
		const char *RESTRICT _NormalMapFile;

		//The roughness file path.
		const char *RESTRICT _RoughnessFile;

		//The metallic file path.
		const char *RESTRICT _MetallicFile;

		//The ambient occlusion file path.
		const char *RESTRICT _AmbientOcclusionFile;

		//The variant file path.
		const char *RESTRICT _VariantFile;

	};

	/*
	*	Creates a physical material resource file.
	*/
	static void CreatePhysicalMaterial(const PhysicalMaterialCreationParameters &parameters) noexcept
	{
		//What should the material be called?
		DynamicString fileName{ parameters._Output };
		fileName += ".cr";

		//Open the file to be written to.
		BinaryFile<IOMode::Out> file{ fileName.CString() };

		//Write the resource type to the file.
		constexpr ResourceType resourceType{ ResourceType::PhysicalMaterial };
		file.Write(&resourceType, sizeof(ResourceType));

		//Write the resource ID to the file.
		const HashString resourceID{ parameters._ID };
		file.Write(&resourceID, sizeof(HashString));

		//Write the number of mipmap levels to the file.
		file.Write(&parameters._MipmapLevels, sizeof(uint8));

		//Load the albedo.
		int32 width, height, numberOfChannels;
		byte *RESTRICT data{ stbi_load(parameters._AlbedoFile, &width, &height, &numberOfChannels, STBI_rgb_alpha) };

		const uint32 uWidth{ static_cast<uint32>(width) };
		const uint32 uHeight{ static_cast<uint32>(height) };

		//Write the width and height of the albedo into the file, to be read into uint32's.
		file.Write(&uWidth, sizeof(uint32));
		file.Write(&uHeight, sizeof(uint32));

		//Write the albedo to the file.
		for (uint8 i = 0; i < parameters._MipmapLevels; ++i)
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
		data = stbi_load(parameters._NormalMapFile, &width, &height, &numberOfChannels, STBI_rgb_alpha);

		//Write the layer albedo to the file, to be read into byte's.
		for (uint8 i = 0; i < parameters._MipmapLevels; ++i)
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

		//Load the roughness, metallic, ambient occlusion and variant data.
		byte *RESTRICT roughnessData = parameters._RoughnessFile ? stbi_load(parameters._RoughnessFile, &width, &height, &numberOfChannels, STBI_rgb_alpha) : nullptr;
		byte *RESTRICT metallicData = parameters._MetallicFile ? stbi_load(parameters._MetallicFile, &width, &height, &numberOfChannels, STBI_rgb_alpha) : nullptr;
		byte *RESTRICT ambientOcclusionData = parameters._AmbientOcclusionFile ? stbi_load(parameters._AmbientOcclusionFile, &width, &height, &numberOfChannels, STBI_rgb_alpha) : nullptr;
		byte *RESTRICT variantData = parameters._VariantFile ? stbi_load(parameters._VariantFile, &width, &height, &numberOfChannels, STBI_rgb_alpha) : nullptr;

		//Write the roughness, metallic, ambient occlusion and variant data to the file.
		constexpr byte defaultRoughness{ 255 };
		constexpr byte defaultMetallic{ 0 };
		constexpr byte defaultAmbientOcclusion{ 255 };
		constexpr byte defaultVariant{ 255 };

		for (uint8 i = 0; i < parameters._MipmapLevels; ++i)
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
					file.Write(variantData ? &variantData[j * 4] : &defaultVariant, sizeof(byte));
				}
			}

			else
			{
				byte *RESTRICT downsampledRoughnessData = roughnessData ? static_cast<byte *RESTRICT >(MemoryUtilities::AllocateMemory(textureSize * 4)) : nullptr;
				byte *RESTRICT downsampledMetallicData = metallicData ? static_cast<byte *RESTRICT >(MemoryUtilities::AllocateMemory(textureSize * 4)) : nullptr;
				byte *RESTRICT downsampledAmbientOcclusionData = ambientOcclusionData ? static_cast<byte *RESTRICT >(MemoryUtilities::AllocateMemory(textureSize * 4)) : nullptr;
				byte *RESTRICT downsampledVariantData = variantData ? static_cast<byte *RESTRICT >(MemoryUtilities::AllocateMemory(textureSize * 4)) : nullptr;

				if (roughnessData) stbir_resize_uint8(roughnessData, width, height, 0, downsampledRoughnessData, uWidth >> i, uHeight >> i, 0, 4);
				if (metallicData) stbir_resize_uint8(metallicData, width, height, 0, downsampledMetallicData, uWidth >> i, uHeight >> i, 0, 4);
				if (ambientOcclusionData) stbir_resize_uint8(ambientOcclusionData, width, height, 0, downsampledAmbientOcclusionData, uWidth >> i, uHeight >> i, 0, 4);
				if (variantData) stbir_resize_uint8(variantData, width, height, 0, downsampledVariantData, uWidth >> i, uHeight >> i, 0, 4);

				for (uint64 j = 0; j < textureSize; ++j)
				{
					file.Write(downsampledRoughnessData ? &downsampledRoughnessData[j * 4] : &defaultRoughness, sizeof(byte));
					file.Write(downsampledMetallicData ? &downsampledMetallicData[j * 4] : &defaultMetallic, sizeof(byte));
					file.Write(downsampledAmbientOcclusionData ? &downsampledAmbientOcclusionData[j * 4] : &defaultAmbientOcclusion, sizeof(byte));
					file.Write(downsampledVariantData ? &downsampledVariantData[j * 4] : &defaultVariant, sizeof(byte));
				}

				MemoryUtilities::FreeMemory(downsampledRoughnessData);
				MemoryUtilities::FreeMemory(downsampledMetallicData);
				MemoryUtilities::FreeMemory(downsampledAmbientOcclusionData);
				MemoryUtilities::FreeMemory(downsampledVariantData);
			}
		}

		//Free the roughness, metallic, ambient occlusion and displacement data.
		stbi_image_free(roughnessData);
		stbi_image_free(metallicData);
		stbi_image_free(ambientOcclusionData);
		stbi_image_free(variantData);

		//Close the file.
		file.Close();
	}

};