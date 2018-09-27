#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/General/HashString.h>

//Resources
#include <Resources/ResourcesCore.h>

//Third party libraries.
#include <stb_image.h>
#include <stb_image_resize.h>

class OceanMaterialCreator final
{

public:

	class OceanMaterialCreationParameters final
	{

	public:

		//The output file path.
		const char *RESTRICT _Output;

		//The resource id.
		const char *RESTRICT _ID;

		//The number of mipmap levels.
		uint8 _MipmapLevels;

		//The normal file path.
		const char *RESTRICT _Normal;

		//The foam file path.
		const char *RESTRICT _Foam;

	};

	/*
	*	Creates an ocean material resource file.
	*/
	static void CreateOceanMaterial(const OceanMaterialCreationParameters &parameters) NOEXCEPT
	{
		//What should the material be called?
		DynamicString fileName{ parameters._Output };
		fileName += ".cr";

		//Open the file to be written to.
		BinaryFile<IOMode::Out> file{ fileName.CString() };

		//Write the resource type to the file.
		constexpr ResourceType resourceType{ ResourceType::OceanMaterial };
		file.Write(&resourceType, sizeof(ResourceType));

		//Write the resource ID to the file.
		const HashString resourceID{ parameters._ID };
		file.Write(&resourceID, sizeof(HashString));

		//Determine how many mipmap levels that should be generated.
		const uint8 numberOfMipmapLevels{ parameters._MipmapLevels };

		//Write the number of mipmap levels to the file.
		file.Write(&numberOfMipmapLevels, sizeof(uint8));

		//Load the normal.
		int32 width, height, numberOfChannels;
		byte *RESTRICT data{ stbi_load(parameters._Normal, &width, &height, &numberOfChannels, STBI_rgb_alpha) };

		const uint32 uWidth{ static_cast<uint32>(width) };
		const uint32 uHeight{ static_cast<uint32>(height) };

		//Write the width and height of the albedo into the file, to be read into uint32's.
		file.Write(&uWidth, sizeof(uint32));
		file.Write(&uHeight, sizeof(uint32));

		//Write the normal map to the file.
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

		//Free the normal  data.
		stbi_image_free(data);

		//Load the foam.
		data = stbi_load(parameters._Foam, &width, &height, &numberOfChannels, STBI_rgb_alpha);

		//Write the normal map to the file.
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

		//Free the normal map data.
		stbi_image_free(data);

		//Close the file.
		file.Close();
	}
};