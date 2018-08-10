#pragma once

//Core.
#include <Core/EngineCore.h>
#include <Core/HashString.h>

//Resources
#include <Resources/ResourcesCore.h>

//Third party libraries.
#include <stb_image.h>
#include <stb_image_resize.h>

class WaterMaterialCreator final
{

public:

	/*
	*	Creates a water material resource file.
	*/
	static void CreateWaterMaterial(const char *const RESTRICT arguments[]) NOEXCEPT
	{
		//What should the material be called?
		DynamicString fileName{ arguments[0] };
		fileName += ".cr";

		//Open the file to be written to.
		BinaryFile<IOMode::Out> file{ fileName.CString() };

		//Write the resource type to the file.
		constexpr ResourceType resourceType{ ResourceType::OceanMaterial };
		file.Write(&resourceType, sizeof(ResourceType));

		//Write the resource ID to the file.
		const HashString resourceID{ arguments[1] };
		file.Write(&resourceID, sizeof(HashString));

		//Determine how many mipmap levels that should be generated.
		const uint8 numberOfMipmapLevels{ static_cast<uint8>(*arguments[2] - '0') };

		//Write the number of mipmap levels to the file.
		file.Write(&numberOfMipmapLevels, sizeof(uint8));

		//Load the normal map.
		int32 width, height, numberOfChannels;
		byte *RESTRICT data{ stbi_load(arguments[3], &width, &height, &numberOfChannels, STBI_rgb_alpha) };

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

		//Free the normal map data.
		stbi_image_free(data);

		//Close the file.
		file.Close();
	}
};