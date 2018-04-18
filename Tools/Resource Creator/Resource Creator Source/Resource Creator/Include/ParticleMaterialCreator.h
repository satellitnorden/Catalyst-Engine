#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>
#include <Engine Core/HashString.h>

//Resources
#include <Resources/ResourcesCore.h>

//Third party libraries.
#include <stb_image.h>
#include <stb_image_resize.h>

class ParticleMaterialCreator final
{

public:

	/*
	*	Creates a particle material resource file.
	*/
	static void CreateParticleMaterial(const int32 argumentCount, char *RESTRICT arguments[]) noexcept
	{
		//What should the material be called?
		DynamicString fileName{ arguments[2] };
		fileName += ".cr";

		//Open the file to be written to.
		BinaryFile<IOMode::Out> file{ fileName.CString() };

		//Write the resource type to the file.
		constexpr ResourceType resourceType{ ResourceType::ParticleMaterial };
		file.Write(&resourceType, sizeof(ResourceType));

		//Write the resource ID to the file.
		const HashString resourceID{ arguments[3] };
		file.Write(&resourceID, sizeof(HashString));

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
				textureSize = (width >> i) * (height >> i) * 4;

				byte *RESTRICT downsampledData = static_cast<byte *RESTRICT>(MemoryUtilities::AllocateMemory(textureSize));
				stbir_resize_uint8(data, width, height, 0, downsampledData, width >> i, height >> i, 0, 4);

				file.Write(downsampledData, textureSize);

				MemoryUtilities::FreeMemory(downsampledData);
			}
		}

		//Free the layer albedo data.
		stbi_image_free(data);

		//Close the file.
		file.Close();
	}

};