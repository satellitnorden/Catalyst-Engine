#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/General/HashString.h>

//Resources
#include <Resources/ResourcesCore.h>

//Third party libraries.
#include <stb_image.h>
#include <stb_image_resize.h>

class TreeVegetationMaterialCreator final
{

public:

	class TreeVegetationMaterialCreationParameters final
	{

	public:

		//The output file path.
		const char *RESTRICT _Output;

		//The resource id.
		const char *RESTRICT _ID;

		//The number of mipmap levels for the crown mask texture.
		uint8 _CrownMaskMipmapLevels;

		//The mask texture file path for the crown.
		const char *RESTRICT _CrownMaskFile;

		//The number of mipmap levels for the crown.
		uint8 _CrownMipmapLevels;

		//The albedo file path for the crown.
		const char *RESTRICT _CrownAlbedoFile;

		//The normal map file path for the crown.
		const char *RESTRICT _CrownNormalMapFile;

		//The number of mipmap levels for the trunk.
		uint8 _TrunkMipmapLevels;

		//The trunk albedo file path.
		const char *RESTRICT _TrunkAlbedoFile;

		//The trunk normal map file path.
		const char *RESTRICT _TrunkNormalMapFile;

		//The trunk roughness file path.
		const char *RESTRICT _TrunkRoughnessFile;

		//The trunk metallic file path.
		const char *RESTRICT _TrunkMetallicFile;

		//The trunk ambient occlusion file path.
		const char *RESTRICT _TrunkAmbientOcclusionFile;

	};

	/*
	*	Creates a tree vegetation material resource file.
	*/
	static void CreateTreeVegetationMaterial(const TreeVegetationMaterialCreationParameters &parameters) noexcept
	{
		//What should the material be called?
		DynamicString fileName{ parameters._Output };
		fileName += ".cr";

		//Open the file to be written to.
		BinaryFile<IOMode::Out> file{ fileName.CString() };

		//Write the resource type to the file.
		constexpr ResourceType resourceType{ ResourceType::TreeVegetationMaterial };
		file.Write(&resourceType, sizeof(ResourceType));

		//Write the resource ID to the file.
		const HashString resourceID{ parameters._ID };
		file.Write(&resourceID, sizeof(HashString));

		{
			//Write the number of mipmap levels that should be generated for the crown mask texture to the file.
			file.Write(&parameters._CrownMaskMipmapLevels, sizeof(uint8));

			//Load the crown mask texture.
			int32 width, height, numberOfChannels;
			byte *RESTRICT data{ stbi_load(parameters._CrownMaskFile, &width, &height, &numberOfChannels, STBI_rgb_alpha) };

			uint32 uWidth{ static_cast<uint32>(width) };
			uint32 uHeight{ static_cast<uint32>(height) };

			//Write the width and height of the crown mask texture into the file.
			file.Write(&uWidth, sizeof(uint32));
			file.Write(&uHeight, sizeof(uint32));

			//Write the crown mask texture to the file.
			for (uint8 i = 0; i < parameters._CrownMaskMipmapLevels; ++i)
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

			//Free the mask texture data.
			stbi_image_free(data);

			//Write the number of crown mipmap levels  to the file.
			file.Write(&parameters._CrownMipmapLevels, sizeof(uint8));

			//Load the crown albedo texture.
			data = stbi_load(parameters._CrownAlbedoFile, &width, &height, &numberOfChannels, STBI_rgb_alpha);

			uWidth = static_cast<uint32>(width);
			uHeight = static_cast<uint32>(height);

			//Write the width and height into the file.
			file.Write(&uWidth, sizeof(uint32));
			file.Write(&uHeight, sizeof(uint32));

			//Write the crown albedo texture to the file.
			for (uint8 i = 0; i < parameters._CrownMipmapLevels; ++i)
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

			//Free the crown albedo texture data.
			stbi_image_free(data);

			//Load the crown normal map texture.
			data = stbi_load(parameters._CrownNormalMapFile, &width, &height, &numberOfChannels, STBI_rgb_alpha);

			//Write the crown normal map texture to the file.
			for (uint8 i = 0; i < parameters._CrownMipmapLevels; ++i)
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

			//Free the normal map texture data.
			stbi_image_free(data);
		}

		//Write the number of trunk mipmap levels to the file.
		file.Write(&parameters._TrunkMipmapLevels, sizeof(uint8));

		//Load the trunk albedo.
		int32 width, height, numberOfChannels;
		byte *RESTRICT data{ stbi_load(parameters._TrunkAlbedoFile, &width, &height, &numberOfChannels, STBI_rgb_alpha) };

		const uint32 uWidth{ static_cast<uint32>(width) };
		const uint32 uHeight{ static_cast<uint32>(height) };

		//Write the width and height of the trunk albedo into the file, to be read into uint32's.
		file.Write(&uWidth, sizeof(uint32));
		file.Write(&uHeight, sizeof(uint32));

		//Write the trunk albedo to the file.
		for (uint8 i = 0; i < parameters._TrunkMipmapLevels; ++i)
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

		//Free the layer trunk albedo data.
		stbi_image_free(data);

		//Load the trunk normal map.
		data = stbi_load(parameters._TrunkNormalMapFile, &width, &height, &numberOfChannels, STBI_rgb_alpha);

		//Write the trunk albedo to the file, to be read into byte's.
		for (uint8 i = 0; i < parameters._TrunkMipmapLevels; ++i)
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

		//Free the trunk normal map data.
		stbi_image_free(data);

		//Load the roughness, metallic, ambient occlusion and variant data.
		byte *RESTRICT roughnessData = parameters._TrunkRoughnessFile ? stbi_load(parameters._TrunkRoughnessFile, &width, &height, &numberOfChannels, STBI_rgb_alpha) : nullptr;
		byte *RESTRICT metallicData = parameters._TrunkMetallicFile ? stbi_load(parameters._TrunkMetallicFile, &width, &height, &numberOfChannels, STBI_rgb_alpha) : nullptr;
		byte *RESTRICT ambientOcclusionData = parameters._TrunkAmbientOcclusionFile ? stbi_load(parameters._TrunkAmbientOcclusionFile, &width, &height, &numberOfChannels, STBI_rgb_alpha) : nullptr;

		//Write the roughness, metallic, ambient occlusion and variant data to the file.
		constexpr byte defaultRoughness{ 255 };
		constexpr byte defaultMetallic{ 0 };
		constexpr byte defaultAmbientOcclusion{ 255 };
		constexpr byte defaultVariant{ 255 };

		for (uint8 i = 0; i < parameters._TrunkMipmapLevels; ++i)
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
					file.Write(&defaultVariant, sizeof(byte));
				}
			}

			else
			{
				byte *RESTRICT downsampledRoughnessData = roughnessData ? static_cast<byte *RESTRICT >(MemoryUtilities::AllocateMemory(textureSize * 4)) : nullptr;
				byte *RESTRICT downsampledMetallicData = metallicData ? static_cast<byte *RESTRICT >(MemoryUtilities::AllocateMemory(textureSize * 4)) : nullptr;
				byte *RESTRICT downsampledAmbientOcclusionData = ambientOcclusionData ? static_cast<byte *RESTRICT >(MemoryUtilities::AllocateMemory(textureSize * 4)) : nullptr;

				if (roughnessData) stbir_resize_uint8(roughnessData, width, height, 0, downsampledRoughnessData, uWidth >> i, uHeight >> i, 0, 4);
				if (metallicData) stbir_resize_uint8(metallicData, width, height, 0, downsampledMetallicData, uWidth >> i, uHeight >> i, 0, 4);
				if (ambientOcclusionData) stbir_resize_uint8(ambientOcclusionData, width, height, 0, downsampledAmbientOcclusionData, uWidth >> i, uHeight >> i, 0, 4);

				for (uint64 j = 0; j < textureSize; ++j)
				{
					file.Write(downsampledRoughnessData ? &downsampledRoughnessData[j * 4] : &defaultRoughness, sizeof(byte));
					file.Write(downsampledMetallicData ? &downsampledMetallicData[j * 4] : &defaultMetallic, sizeof(byte));
					file.Write(downsampledAmbientOcclusionData ? &downsampledAmbientOcclusionData[j * 4] : &defaultAmbientOcclusion, sizeof(byte));
					file.Write(&defaultVariant, sizeof(byte));
				}

				MemoryUtilities::FreeMemory(downsampledRoughnessData);
				MemoryUtilities::FreeMemory(downsampledMetallicData);
				MemoryUtilities::FreeMemory(downsampledAmbientOcclusionData);
			}
		}

		//Free the roughness, metallic, ambient occlusion and displacement data.
		stbi_image_free(roughnessData);
		stbi_image_free(metallicData);
		stbi_image_free(ambientOcclusionData);

		//Close the file.
		file.Close();
	}

};