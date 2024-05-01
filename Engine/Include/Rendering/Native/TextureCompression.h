#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

class TextureCompression final
{

public:

	//Enumeration covering all modes.
	enum class Mode : uint8
	{
		/*
		*	No compression at all.
		*/
		NONE,

		/*
		*	Applies BC7 texture compression.
		*/
		BC7
	};

	//The mode.
	Mode _Mode{ Mode::NONE };

	//Denotes whether or not the compression should be perceptual (good for color images) or not (linear, good for stuff like roughness maps).
	bool _Perceptual{ false };

	/*
	*	Returns the compression ratio for a 2D texture.
	*/
	NO_DISCARD uint32 CompressionRatio() const NOEXCEPT;

	/*
	*	Returns the size required for compression for a 2D texture.
	*/
	NO_DISCARD uint64 Size2D(const uint32 width, const uint32 height) const NOEXCEPT;

	/*
	*	Compresses a 2D texture.
	*/
	void Compress2D(const byte *const RESTRICT input_data, const uint32 width, const uint32 height, byte *const RESTRICT output_data) const NOEXCEPT;

private:

	/*
	*	Compresses a 2D texture with the BC7 mode.
	*/
	void Compress2DBC7(const byte *const RESTRICT input_data, const uint32 width, const uint32 height, byte *const RESTRICT output_data) const NOEXCEPT;

};