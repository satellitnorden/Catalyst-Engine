//Header file.
#include <Rendering/Native/TextureCompression.h>

//Core.
#include <Core/Containers/StaticArray.h>

//Math.
#include <Math/General/Vector.h>

//Third party.
#include <ThirdParty/bc7enc/bc7enc.h>

class TextureCompressionInitializer final
{

public:

	/*
	*	Default constructor.
	*/
	FORCE_INLINE TextureCompressionInitializer() NOEXCEPT
	{
		//Initialize the "bc7enc" library.
		bc7enc_compress_block_init();
	}

};

TextureCompressionInitializer INITIALIZER;

/*
*	Returns the compression ratio for a 2D texture.
*/
NO_DISCARD uint32 TextureCompression::CompressionRatio() const NOEXCEPT
{
	switch (_Mode)
	{
		case Mode::NONE:
		{
			return 1;
		}

		case Mode::BC7:
		{
			return 4;
		}

		default:
		{
			ASSERT(false, "Invalid case!");

			return 1;
		}
	}
}

/*
*	Returns the size required for compression for a 2D texture.
*/
NO_DISCARD uint64 TextureCompression::Size2D(const uint32 width, const uint32 height) const NOEXCEPT
{
	return width * height * (sizeof(byte) * 4) / CompressionRatio();
}

/*
*	Compresses a 2D texture.
*/
void TextureCompression::Compress2D(const byte *const RESTRICT input_data, const uint32 width, const uint32 height, byte *const RESTRICT output_data) const NOEXCEPT
{
	switch (_Mode)
	{
		case Mode::NONE:
		{
			ASSERT(false, "Don't call this function without a mode set!");

			break;
		}

		case Mode::BC7:
		{
			Compress2DBC7(input_data, width, height, output_data);

			break;
		}

		default:
		{
			ASSERT(false, "Invalid case!");

			break;
		}
	}
}

/*
*	Compresses a 2D texture with the BC7 mode.
*/
void TextureCompression::Compress2DBC7(const byte *const RESTRICT input_data, const uint32 width, const uint32 height, byte *const RESTRICT output_data) const NOEXCEPT
{
	bc7enc_compress_block_params parameters;

	bc7enc_compress_block_params_init(&parameters);

	if (_Perceptual)
	{
		bc7enc_compress_block_params_init_perceptual_weights(&parameters);
	}

	else
	{
		bc7enc_compress_block_params_init_linear_weights(&parameters);
	}

	uint64 output_cursor{ 0 };

	StaticArray<Vector4<byte>, 16> block;
	StaticArray<uint64, 2> output;

	for (uint32 Y{ 0 }; Y < height; Y += 4)
	{
		for (uint32 X{ 0 }; X < width; X += 4)
		{
			{
				uint64 block_index{ 0 };

				for (uint32 _Y{ 0 }; _Y < 4; ++_Y)
				{
					for (uint32 _X{ 0 }; _X < 4; ++_X)
					{
						const uint32 __X{ X + _X };
						const uint32 __Y{ Y + _Y };

						block[block_index++] = ((const Vector4<byte> *const RESTRICT)input_data)[__X + (__Y * width)];
					}
				}
			}

			bc7enc_compress_block(output.Data(), block.Data(), &parameters);

			((uint64 *const RESTRICT)output_data)[output_cursor++] = output[0];
			((uint64 *const RESTRICT)output_data)[output_cursor++] = output[1];
		}
	}
}