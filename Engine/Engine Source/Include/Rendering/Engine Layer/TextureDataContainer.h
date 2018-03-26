#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Rendering.
#include <Rendering/Engine Layer/CPUTexture4.h>
#include <Rendering/Engine Layer/TextureLoader.h>

class TextureDataContainer final
{

public:

	/*
	*	Copy constructor.
	*/
	TextureDataContainer(const TextureDataContainer &otherContainer) NOEXCEPT
	{
		textureData = otherContainer.textureData;
		textureWidth = otherContainer.textureWidth;
		textureHeight = otherContainer.textureHeight;
		textureChannels = otherContainer.textureChannels;
		textureTexelSize = otherContainer.textureTexelSize;
	}

	/*
	*	Move constructor.
	*/
	TextureDataContainer(TextureDataContainer &&otherContainer) NOEXCEPT
	{
		textureData = otherContainer.textureData;
		textureWidth = otherContainer.textureWidth;
		textureHeight = otherContainer.textureHeight;
		textureChannels = otherContainer.textureChannels;
		textureTexelSize = otherContainer.textureTexelSize;
		releaseOnDestruction = otherContainer.releaseOnDestruction;

		otherContainer.releaseOnDestruction = false;
	}

	/*
	*	Constructor taking a byte pointer along with the width, height and number of channels of the texture.
	*/
	TextureDataContainer(byte *RESTRICT textureDataPointer, const uint16 initialWidth, const uint16 initialHeight, const uint8 initilNumberOfChannels) NOEXCEPT
	{
		textureData.Reserve(1);
		textureData.EmplaceFast(textureDataPointer);
		textureWidth = initialWidth;
		textureHeight = initialHeight;
		textureChannels = initilNumberOfChannels;
		textureTexelSize = sizeof(byte);
	}

	/*
	*	Constructor taking a CPUTexture4.
	*/
	TextureDataContainer(const CPUTexture4 &texture) NOEXCEPT
	{
		textureData.Reserve(1);
		textureData.EmplaceFast(reinterpret_cast<void *RESTRICT>(const_cast<CPUTexture4&>(texture).Data()));
		textureWidth = static_cast<uint16>(texture.GetWidth());
		textureHeight = static_cast<uint16>(texture.GetHeight());
		textureChannels = 4;
		textureTexelSize = sizeof(float);
	}

	/*
	*	Constructor taking a texture path.
	*/
	TextureDataContainer(const char *const RESTRICT texturePath) NOEXCEPT
	{
		//Load the texture.
		byte *RESTRICT data;
		TextureLoader::LoadTexture(texturePath, textureWidth, textureHeight, textureChannels, &data);

		textureData.Reserve(1);
		textureData.EmplaceFast(data);
		textureChannels = 4;
		textureTexelSize = sizeof(byte);

		releaseOnDestruction = true;
	}

	/*
	*	Array taking an array of texture paths, for mip generation.
	*/
	TextureDataContainer(const DynamicArray<const char *RESTRICT> &texturePaths) NOEXCEPT
	{
		const uint64 numberOfTextures{ texturePaths.Size() };
		textureData.Reserve(numberOfTextures);

		//Load the texture.
		for (uint64 i = 0; i < numberOfTextures; ++i)
		{
			byte *RESTRICT data;
			uint16 tempWidth, tempHeight;
			uint8 tempNumberOfChannels;
			TextureLoader::LoadTexture(texturePaths[i], tempWidth, tempHeight, tempNumberOfChannels, &data);

			//Only store the width/height for the base mip, it is assumed that following mip layers are half it's size.
			if (i == 0)
			{
				textureWidth = tempWidth;
				textureHeight = tempHeight;
				textureChannels = 4;
				textureTexelSize = sizeof(byte);
			}

			textureData.EmplaceFast(data);
		}
		
		releaseOnDestruction = true;
	}

	/*
	*	Default destructor.
	*/
	~TextureDataContainer() NOEXCEPT
	{
		if (releaseOnDestruction)
		{
			for (void *RESTRICT textureDataChunk : textureData)
			{
				TextureLoader::FreeTexture(static_cast<byte *RESTRICT>(textureDataChunk));
			}
		}
	}

	//Pointer to the underlying texture data.
	DynamicArray<void *RESTRICT> textureData;

	//The width of the texture.
	uint16 textureWidth;

	//The height of the texture.
	uint16 textureHeight;

	//The number of channels of the texture.
	uint8 textureChannels;

	//The size of each texel, denoted in bytes.
	uint8 textureTexelSize;

	//Defines whether or not texture data should be released on destruction.
	bool releaseOnDestruction{ false };

};