#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/General/HashString.h>

//Resources
#include <Resources/ResourcesCore.h>

//Third party libraries.
#include <stb_image.h>
#include <stb_image_resize.h>

class VegetationMaterialCreator final
{

public:

	class VegetationMaterialCreationParameters final
	{

	public:

		//The output file path.
		const char *RESTRICT _Output;

		//The resource id.
		const char *RESTRICT _ID;

		//The number of mipmap levels for the mask texture.
		uint8 _MaskMipmapLevels;

		//The mask texture file path.
		const char *RESTRICT _MaskFile;

		//The number of mipmap levels.
		uint8 _MipmapLevels;

		//The albedo file path.
		const char *RESTRICT _AlbedoFile;

		//The normal map file path.
		const char *RESTRICT _NormalMapFile;

	};

	/*
	*	Creates a vegetation material resource file.
	*/
	static void CreateVegetationMaterial(const VegetationMaterialCreationParameters &parameters) noexcept
	{
		//What should the material be called?
		DynamicString fileName{ parameters._Output };
		fileName += ".cr";

		//Open the file to be written to.
		BinaryFile<IOMode::Out> file{ fileName.CString() };

		//Write the resource type to the file.
		constexpr ResourceType resourceType{ ResourceType::GrassVegetationMaterial };
		file.Write(&resourceType, sizeof(ResourceType));

		//Write the resource ID to the file.
		const HashString resourceID{ parameters._ID };
		file.Write(&resourceID, sizeof(HashString));

		//Write the number of mipmap levels that should be generated for the mask texture to the file.
		file.Write(&parameters._MaskMipmapLevels, sizeof(uint8));

		//Load the mask texture.
		int32 width, height, numberOfChannels;
		byte *RESTRICT data{ stbi_load(parameters._MaskFile, &width, &height, &numberOfChannels, STBI_rgb_alpha) };

		uint32 uWidth{ static_cast<uint32>(width) };
		uint32 uHeight{ static_cast<uint32>(height) };

		//Write the width and height of the mask texture into the file.
		file.Write(&uWidth, sizeof(uint32));
		file.Write(&uHeight, sizeof(uint32));

		//Write the mask texture to the file.
		for (uint8 i = 0; i < parameters._MaskMipmapLevels; ++i)
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

		//Write the number of mipmap levels  to the file.
		file.Write(&parameters._MipmapLevels, sizeof(uint8));

		//Load the albedo texture.
		data = stbi_load(parameters._AlbedoFile, &width, &height, &numberOfChannels, STBI_rgb_alpha);

		uWidth = static_cast<uint32>(width);
		uHeight = static_cast<uint32>(height);

		//Write the width and height into the file.
		file.Write(&uWidth, sizeof(uint32));
		file.Write(&uHeight, sizeof(uint32));

		//Write the albedo texture to the file.
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

		//Free the albedo texture data.
		stbi_image_free(data);

		//Load the normal map texture.
		data = stbi_load(parameters._NormalMapFile, &width, &height, &numberOfChannels, STBI_rgb_alpha);

		//Write the normal map texture to the file.
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

		//Free the normal map texture data.
		stbi_image_free(data);

		//Close the file.
		file.Close();
	}

};