#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/Texture2D.h>
#include <Rendering/Native/Texture3D.h>

class TextureDataContainer final
{

public:

	//Pointer to the underlying texture data.
	DynamicArray<const void *RESTRICT> _TextureData;

	//The width of the texture.
	uint32 _TextureWidth;

	//The height of the texture.
	uint32 _TextureHeight;

	//The depth of the texture. Only applies to 3D textures.
	uint32 _TextureDepth;

	//The number of channels of the texture.
	uint8 _TextureChannels;

	//The size of each texel, denoted in bytes.
	uint8 _TextureTexelSize;

	/*
	*	Copy constructor.
	*/
	FORCE_INLINE TextureDataContainer(const TextureDataContainer &other) NOEXCEPT
	{
		_TextureData = other._TextureData;
		_TextureWidth = other._TextureWidth;
		_TextureHeight = other._TextureHeight;
		_TextureDepth = other._TextureDepth;
		_TextureChannels = other._TextureChannels;
		_TextureTexelSize = other._TextureTexelSize;
	}

	/*
	*	Move constructor.
	*/
	FORCE_INLINE TextureDataContainer(TextureDataContainer &&other) NOEXCEPT
	{
		_TextureData = std::move(other._TextureData);
		_TextureWidth = other._TextureWidth;
		_TextureHeight = other._TextureHeight;
		_TextureDepth = other._TextureDepth;
		_TextureChannels = other._TextureChannels;
		_TextureTexelSize = other._TextureTexelSize;
	}

	/*
	*	Constructor taking a byte pointer along with the width, height and number of channels of the texture.
	*/
	FORCE_INLINE TextureDataContainer(byte *RESTRICT texture_data, const uint32 initial_width, const uint32 initial_height, const uint32 initial_depth, const uint8 initial_number_of_channels) NOEXCEPT
	{
		_TextureData.Reserve(1);
		_TextureData.Emplace(texture_data);
		_TextureWidth = initial_width;
		_TextureHeight = initial_height;
		_TextureDepth = initial_depth;
		_TextureChannels = initial_number_of_channels;
		_TextureTexelSize = sizeof(byte);
	}

	/*
	*	Constructor taking a float pointer along with the width, height and number of channels of the texture.
	*/
	FORCE_INLINE TextureDataContainer(float *RESTRICT texture_data, const uint32 initial_width, const uint32 initial_height, const uint32 initial_depth, const uint8 initial_number_of_channels) NOEXCEPT
	{
		_TextureData.Reserve(1);
		_TextureData.Emplace(texture_data);
		_TextureWidth = initial_width;
		_TextureHeight = initial_height;
		_TextureDepth = initial_depth;
		_TextureChannels = initial_number_of_channels;
		_TextureTexelSize = sizeof(float);
	}

	/*
	*	Constructor taking a Texture2D with 1 channel.
	*/
	template <typename TYPE>
	FORCE_INLINE TextureDataContainer(const Texture2D<TYPE> &texture) NOEXCEPT
	{
		_TextureData.Reserve(1);
		_TextureData.Emplace(reinterpret_cast<const void *RESTRICT>(texture.Data()));
		_TextureWidth = static_cast<uint32>(texture.GetWidth());
		_TextureHeight = static_cast<uint32>(texture.GetHeight());
		_TextureDepth = 1;
		_TextureChannels = 1;
		_TextureTexelSize = sizeof(TYPE);
	}

	/*
	*	Constructor taking a dynamic array of Texture2D with 1 channel.
	*/
	template <typename TYPE>
	FORCE_INLINE TextureDataContainer(const DynamicArray<Texture2D<TYPE>> &texture) NOEXCEPT
	{
		_TextureData.Reserve(texture.Size());

		for (const Texture2D<TYPE> &mip : texture)
		{
			_TextureData.Emplace(reinterpret_cast<const void *RESTRICT>(mip.Data()));
		}

		_TextureWidth = static_cast<uint32>(texture[0].GetWidth());
		_TextureHeight = static_cast<uint32>(texture[0].GetHeight());
		_TextureDepth = 1;
		_TextureChannels = 1;
		_TextureTexelSize = sizeof(TYPE);
	}

	/*
	*	Constructor taking a Texture2D with 4 channels.
	*/
	template <typename TYPE>
	FORCE_INLINE TextureDataContainer(const Texture2D<Vector4<TYPE>> &texture) NOEXCEPT
	{
		_TextureData.Reserve(1);
		_TextureData.Emplace(reinterpret_cast<const void *RESTRICT>(texture.Data()));
		_TextureWidth = static_cast<uint32>(texture.GetWidth());
		_TextureHeight = static_cast<uint32>(texture.GetHeight());
		_TextureDepth = 1;
		_TextureChannels = 4;
		_TextureTexelSize = sizeof(TYPE);
	}

	/*
	*	Constructor taking a Texture3D with 1 channel.
	*/
	template <typename TYPE>
	FORCE_INLINE TextureDataContainer(const Texture3D<TYPE>& texture) NOEXCEPT
	{
		_TextureData.Reserve(1);
		_TextureData.Emplace(reinterpret_cast<const void* RESTRICT>(texture.Data()));
		_TextureWidth = static_cast<uint32>(texture.GetWidth());
		_TextureHeight = static_cast<uint32>(texture.GetHeight());
		_TextureDepth = static_cast<uint32>(texture.GetDepth());
		_TextureChannels = 1;
		_TextureTexelSize = sizeof(TYPE);
	}

	/*
	*	Constructor taking a double dynamic array of bytes.
	*/
	FORCE_INLINE TextureDataContainer(const DynamicArray<DynamicArray<byte>> &initialTextureData, const uint32 initialTextureWidth, const uint32 initialTextureHeight, const uint8 initilTextureChannels) NOEXCEPT
	{
		_TextureData.Reserve(initialTextureData.Size());

		for (const DynamicArray<byte> &initialTextureDataChunk : initialTextureData)
		{
			_TextureData.Emplace(initialTextureDataChunk.Data());
		}

		_TextureWidth = initialTextureWidth;
		_TextureHeight = initialTextureHeight;
		_TextureDepth = 1;
		_TextureChannels = initilTextureChannels;
		_TextureTexelSize = sizeof(byte);
	}

	/*
	*	Constructor taking a double dynamic array of floats.
	*/
	FORCE_INLINE TextureDataContainer(const DynamicArray<DynamicArray<float>> &initialTextureData, const uint32 initialTextureWidth, const uint32 initialTextureHeight, const uint8 initilTextureChannels) NOEXCEPT
	{
		_TextureData.Reserve(initialTextureData.Size());

		for (const DynamicArray<float> &initialTextureDataChunk : initialTextureData)
		{
			_TextureData.Emplace(initialTextureDataChunk.Data());
		}

		_TextureWidth = initialTextureWidth;
		_TextureHeight = initialTextureHeight;
		_TextureDepth = 1;
		_TextureChannels = initilTextureChannels;
		_TextureTexelSize = sizeof(float);
	}

	/*
	*	Constructor taking a static array of Vector.
	*/
	template <uint64 NUMBER_OF_TEXELS>
	FORCE_INLINE TextureDataContainer(const StaticArray<Vector4<float>, NUMBER_OF_TEXELS> &initialTextureData, const uint32 initialTextureWidth, const uint32 initialTextureHeight) NOEXCEPT
	{
		_TextureData.Reserve(1);
		_TextureData.Emplace(initialTextureData.Data());

		_TextureWidth = initialTextureWidth;
		_TextureHeight = initialTextureHeight;
		_TextureDepth = 1;
		_TextureChannels = 4;
		_TextureTexelSize = sizeof(float);
	}

};