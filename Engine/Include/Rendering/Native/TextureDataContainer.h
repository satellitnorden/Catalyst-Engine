#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/Texture2D.h>

class TextureDataContainer final
{

public:

	//Pointer to the underlying texture data.
	DynamicArray<const void *RESTRICT> _TextureData;

	//The width of the texture.
	uint32 _TextureWidth;

	//The height of the texture.
	uint32 _TextureHeight;

	//The number of channels of the texture.
	uint8 _TextureChannels;

	//The size of each texel, denoted in bytes.
	uint8 _TextureTexelSize;

	/*
	*	Copy constructor.
	*/
	TextureDataContainer(const TextureDataContainer &otherContainer) NOEXCEPT
	{
		_TextureData = otherContainer._TextureData;
		_TextureWidth = otherContainer._TextureWidth;
		_TextureHeight = otherContainer._TextureHeight;
		_TextureChannels = otherContainer._TextureChannels;
		_TextureTexelSize = otherContainer._TextureTexelSize;
	}

	/*
	*	Move constructor.
	*/
	TextureDataContainer(TextureDataContainer &&otherContainer) NOEXCEPT
	{
		_TextureData = otherContainer._TextureData;
		_TextureWidth = otherContainer._TextureWidth;
		_TextureHeight = otherContainer._TextureHeight;
		_TextureChannels = otherContainer._TextureChannels;
		_TextureTexelSize = otherContainer._TextureTexelSize;
	}

	/*
	*	Constructor taking a byte pointer along with the width, height and number of channels of the texture.
	*/
	TextureDataContainer(byte *RESTRICT textureDataPointer, const uint32 initialWidth, const uint32 initialHeight, const uint8 initilNumberOfChannels) NOEXCEPT
	{
		_TextureData.Reserve(1);
		_TextureData.EmplaceFast(textureDataPointer);
		_TextureWidth = initialWidth;
		_TextureHeight = initialHeight;
		_TextureChannels = initilNumberOfChannels;
		_TextureTexelSize = sizeof(byte);
	}


	/*
	*	Constructor taking a float pointer along with the width, height and number of channels of the texture.
	*/
	TextureDataContainer(float *RESTRICT textureDataPointer, const uint32 initialWidth, const uint32 initialHeight, const uint8 initilNumberOfChannels) NOEXCEPT
	{
		_TextureData.Reserve(1);
		_TextureData.EmplaceFast(textureDataPointer);
		_TextureWidth = initialWidth;
		_TextureHeight = initialHeight;
		_TextureChannels = initilNumberOfChannels;
		_TextureTexelSize = sizeof(float);
	}

	/*
	*	Constructor taking a Texture2D with 1 channel.
	*/
	template <typename Type>
	TextureDataContainer(const Texture2D<Type> &texture) NOEXCEPT
	{
		_TextureData.Reserve(1);
		_TextureData.EmplaceFast(reinterpret_cast<const void *RESTRICT>(texture.Data()));
		_TextureWidth = static_cast<uint32>(texture.GetWidth());
		_TextureHeight = static_cast<uint32>(texture.GetHeight());
		_TextureChannels = 1;
		_TextureTexelSize = sizeof(Type);
	}

	/*
	*	Constructor taking a dynamic array of Texture2D with 1 channel.
	*/
	template <typename Type>
	TextureDataContainer(const DynamicArray<Texture2D<Type>> &texture) NOEXCEPT
	{
		_TextureData.Reserve(texture.Size());

		for (const Texture2D<byte> &mip : texture)
		{
			_TextureData.EmplaceFast(reinterpret_cast<const void *RESTRICT>(mip.Data()));
		}

		_TextureWidth = static_cast<uint32>(texture[0].GetWidth());
		_TextureHeight = static_cast<uint32>(texture[0].GetHeight());
		_TextureChannels = 1;
		_TextureTexelSize = sizeof(Type);
	}

	/*
	*	Constructor taking a Texture2D with 4 channels.
	*/
	template <typename Type>
	TextureDataContainer(const Texture2D<Vector4<Type>> &texture) NOEXCEPT
	{
		_TextureData.Reserve(1);
		_TextureData.EmplaceFast(reinterpret_cast<const void *RESTRICT>(texture.Data()));
		_TextureWidth = static_cast<uint32>(texture.GetWidth());
		_TextureHeight = static_cast<uint32>(texture.GetHeight());
		_TextureChannels = 4;
		_TextureTexelSize = sizeof(Type);
	}

	/*
	*	Constructor taking a double dynamic array of bytes.
	*/
	TextureDataContainer(const DynamicArray<DynamicArray<byte>> &initialTextureData, const uint32 initialTextureWidth, const uint32 initialTextureHeight, const uint8 initilTextureChannels) NOEXCEPT
	{
		_TextureData.Reserve(initialTextureData.Size());

		for (const DynamicArray<byte> &initialTextureDataChunk : initialTextureData)
		{
			_TextureData.EmplaceFast(initialTextureDataChunk.Data());
		}

		_TextureWidth = initialTextureWidth;
		_TextureHeight = initialTextureHeight;
		_TextureChannels = initilTextureChannels;
		_TextureTexelSize = sizeof(byte);
	}

	/*
	*	Constructor taking a double dynamic array of floats.
	*/
	TextureDataContainer(const DynamicArray<DynamicArray<float>> &initialTextureData, const uint32 initialTextureWidth, const uint32 initialTextureHeight, const uint8 initilTextureChannels) NOEXCEPT
	{
		_TextureData.Reserve(initialTextureData.Size());

		for (const DynamicArray<float> &initialTextureDataChunk : initialTextureData)
		{
			_TextureData.EmplaceFast(initialTextureDataChunk.Data());
		}

		_TextureWidth = initialTextureWidth;
		_TextureHeight = initialTextureHeight;
		_TextureChannels = initilTextureChannels;
		_TextureTexelSize = sizeof(float);
	}

	/*
	*	Constructor taking a static array of Vector.
	*/
	template <uint64 NumberOfTexels>
	TextureDataContainer(const StaticArray<Vector4<float>, NumberOfTexels> &initialTextureData, const uint32 initialTextureWidth, const uint32 initialTextureHeight) NOEXCEPT
	{
		_TextureData.Reserve(1);
		_TextureData.EmplaceFast(initialTextureData.Data());

		_TextureWidth = initialTextureWidth;
		_TextureHeight = initialTextureHeight;
		_TextureChannels = 4;
		_TextureTexelSize = sizeof(float);
	}

};