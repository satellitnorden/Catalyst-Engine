#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Rendering.
#include <Rendering/Engine Layer/CPUTexture2D.h>

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
	*	Constructor taking a CPUTexture2D.
	*/
	TextureDataContainer(const CPUTexture2D &texture) NOEXCEPT
	{
		textureData.Reserve(1);
		textureData.EmplaceFast(reinterpret_cast<void *RESTRICT>(const_cast<CPUTexture2D&>(texture).Data()));
		textureWidth = static_cast<uint16>(texture.GetWidth());
		textureHeight = static_cast<uint16>(texture.GetHeight());
		textureChannels = 4;
		textureTexelSize = sizeof(float);
	}

	/*
	*	Constructor taking a double dynamic array of bytes.
	*/
	TextureDataContainer(const DynamicArray<DynamicArray<byte>> &initialTextureData, const uint16 initialTextureWidth, const uint16 initialTextureHeight, const uint8 initilTextureChannels) NOEXCEPT
	{
		textureData.Reserve(initialTextureData.Size());

		for (const DynamicArray<byte> &initialTextureDataChunk : initialTextureData)
		{
			textureData.EmplaceFast(initialTextureDataChunk.Data());
		}

		textureWidth = initialTextureWidth;
		textureHeight = initialTextureHeight;
		textureChannels = initilTextureChannels;
		textureTexelSize = sizeof(byte);
	}

	/*
	*	Constructor taking a double dynamic array of floats.
	*/
	TextureDataContainer(const DynamicArray<DynamicArray<float>> &initialTextureData, const uint16 initialTextureWidth, const uint16 initialTextureHeight, const uint8 initilTextureChannels) NOEXCEPT
	{
		textureData.Reserve(initialTextureData.Size());

		for (const DynamicArray<float> &initialTextureDataChunk : initialTextureData)
		{
			textureData.EmplaceFast(initialTextureDataChunk.Data());
		}

		textureWidth = initialTextureWidth;
		textureHeight = initialTextureHeight;
		textureChannels = initilTextureChannels;
		textureTexelSize = sizeof(float);
	}

	/*
	*	Constructor taking a static array of Vector.
	*/
	template <uint64 NumberOfTexels>
	TextureDataContainer(const StaticArray<Vector4, NumberOfTexels> &initialTextureData, const uint16 initialTextureWidth, const uint16 initialTextureHeight) NOEXCEPT
	{
		textureData.Reserve(1);
		textureData.EmplaceFast(initialTextureData.Data());

		textureWidth = initialTextureWidth;
		textureHeight = initialTextureHeight;
		textureChannels = 4;
		textureTexelSize = sizeof(float);
	}

	//Pointer to the underlying texture data.
	DynamicArray<const void *RESTRICT> textureData;

	//The width of the texture.
	uint16 textureWidth;

	//The height of the texture.
	uint16 textureHeight;

	//The number of channels of the texture.
	uint8 textureChannels;

	//The size of each texel, denoted in bytes.
	uint8 textureTexelSize;

};