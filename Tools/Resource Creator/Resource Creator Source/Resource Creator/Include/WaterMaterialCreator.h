#pragma once

//Engine core.
#include <EngineCore.h>

//Resources
#include <ResourcesCore.h>

//Third party libraries.
#include <stb_image.h>
#include <stb_image_resize.h>

class WaterMaterialCreator final
{

public:

	/*
	*	Creates a water material resource file.
	*/
	static void CreateWaterMaterial(const int32 argumentCount, char *RESTRICT arguments[]) NOEXCEPT
	{
		//What should the material be called?
		DynamicString fileName{ arguments[2] };
		fileName += ".cwm";

		//Open the file to be written to.
		BinaryFile<IOMode::Out> file{ fileName.CString() };

		//Write the resource type to the file.
		constexpr ResourceType resourceType{ ResourceType::WaterMaterial };
		file.Write(&resourceType, sizeof(ResourceType));

		//Write the resource ID to the file.
		const uint64 resourceID{ std::strtoull(arguments[3], nullptr, 0) };
		file.Write(&resourceID, sizeof(uint64));

		//Determine how many mipmap levels that should be generated.
		const uint8 numberOfMipmapLevels{ static_cast<uint8>(*arguments[4] - '0') };

		//Write the number of mipmap levels to the file.
		file.Write(&numberOfMipmapLevels, sizeof(uint8));

		//Load the normal map.
		int32 width, height, numberOfChannels;
		byte *RESTRICT data{ stbi_load(arguments[5], &width, &height, &numberOfChannels, STBI_rgb_alpha) };

		//Write the width and height of the normal map into the file.
		file.Write(&width, sizeof(int32));
		file.Write(&height, sizeof(int32));

		//Write the normal map to the file.
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

		//Free the normal map data.
		stbi_image_free(data);

		//Close the file.
		file.Close();
	}
};