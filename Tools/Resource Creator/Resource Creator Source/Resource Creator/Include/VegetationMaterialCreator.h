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
		const uint8 numberOfMaskMipmapLevels{ static_cast<uint8>(*arguments[4] - '0') };

		//Determine how many mipmap levels that should be generated for the remaining texture.
		const uint8 numberOfRemainingMipmapLevels{ static_cast<uint8>(*arguments[5] - '0') };

		//Write the number of mipmap levels to the file.
		file.Write(&numberOfMaskMipmapLevels, sizeof(uint8));
		file.Write(&numberOfRemainingMipmapLevels, sizeof(uint8));

		//Load the mask texture.
		int32 width, height, numberOfChannels;
		byte *RESTRICT data{ stbi_load(arguments[6], &width, &height, &numberOfChannels, STBI_rgb_alpha) };

		//Write the width and height into the file, to be read into uint32's.
		file.Write(&width, sizeof(int32));
		file.Write(&height, sizeof(int32));

		//Write the mask to the file.
		uint64 textureSize{ static_cast<uint64>(width * height * 4) };

		for (uint8 i = 0; i < numberOfMaskMipmapLevels; ++i)
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

		//Free the mask data.
		stbi_image_free(data);

		//Load the albedo texture.
		data = stbi_load(arguments[7], &width, &height, &numberOfChannels, STBI_rgb_alpha);

		//Write the albedo to the file.
		for (uint8 i = 0; i < numberOfRemainingMipmapLevels; ++i)
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

		//Free the albedo data.
		stbi_image_free(data);

		//Load the normal map texture.
		data = stbi_load(arguments[8], &width, &height, &numberOfChannels, STBI_rgb_alpha);

		//Write the normal map to the file.
		for (uint8 i = 0; i < numberOfRemainingMipmapLevels; ++i)
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