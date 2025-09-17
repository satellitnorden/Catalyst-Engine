#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Audio.
#include <Audio/Core/Audio.h>

/*
*	Class encapsulating an audio stream, that can hold audio samples of different formats and exposes a couple of functions for sampling the stream.
*/
class AudioStream final
{

public:

	/*
	*	Default constructor.
	*/
	FORCE_INLINE AudioStream() NOEXCEPT
	{

	}

	/*
	*	Returns the sample rate.
	*/
	FORCE_INLINE NO_DISCARD uint32 GetSampleRate() const NOEXCEPT
	{
		return _SampleRate;
	}

	/*
	*	Sets the sample rate.
	*/
	FORCE_INLINE void SetSampleRate(const uint32 value) NOEXCEPT
	{
		_SampleRate = value;
	}

	/*
	*	Returns the number of channels.
	*/
	FORCE_INLINE NO_DISCARD uint8 GetNumberOfChannels() const NOEXCEPT
	{
		return _NumberOfChannels;
	}

	/*
	*	Sets the number of channels.
	*/
	FORCE_INLINE void SetNumberOfChannels(const uint8 value) NOEXCEPT
	{
		_NumberOfChannels = value;
	}

	/*
	*	Returns the format.
	*/
	FORCE_INLINE NO_DISCARD Audio::Format GetFormat() const NOEXCEPT
	{
		return _Format;
	}

	/*
	*	Sets the format.
	*/
	FORCE_INLINE void SetFormat(const Audio::Format value) NOEXCEPT
	{
		_Format = value;
	}

	/*
	*	Returns the number of samples.
	*/
	FORCE_INLINE NO_DISCARD uint32 GetNumberOfSamples() const NOEXCEPT
	{
		return _NumberOfSamples;
	}

	/*
	*	Sets the number of samples.
	*/
	FORCE_INLINE void SetNumberOfSamples(const uint32 value) NOEXCEPT
	{
		_NumberOfSamples = value;
	}

	/*
	*	Sets the data (internally).
	*/
	FORCE_INLINE void SetDataInternal(const byte *const RESTRICT data) NOEXCEPT
	{
		ASSERT(_NumberOfChannels > 0, "Need to set number of channels first!");
		ASSERT(_NumberOfSamples > 0, "Need to set number of samples first!");
		ASSERT(_Data._Internal.Empty(), "Internal data already set!");
		ASSERT(!_Data._External, "External data already set!");

		const uint64 data_size{ _NumberOfChannels * _NumberOfSamples * (Audio::BitsPerSample(_Format) / 8) };

		_Data._Internal.Upsize<false>(data_size);
		Memory::Copy(_Data._Internal.Data(), data, data_size);
	}

	/*
	*	Sets the data (externally).
	*/
	FORCE_INLINE void SetDataExternal(const byte *const RESTRICT data) NOEXCEPT
	{
		ASSERT(_NumberOfChannels > 0, "Need to set number of channels first!");
		ASSERT(_NumberOfSamples > 0, "Need to set number of samples first!");
		ASSERT(_Data._Internal.Empty(), "Internal data already set!");
		ASSERT(!_Data._External, "External data already set!");

		_Data._External = data;
	}

	/*
	*	Samples this audio stream.
	*/
	FORCE_INLINE NO_DISCARD float32 Sample(const uint8 channel_index, const uint32 sample_index) NOEXCEPT
	{
		const uint64 data_index{ sample_index * _NumberOfChannels * (Audio::BitsPerSample(_Format) / 8) + channel_index * (Audio::BitsPerSample(_Format) / 8) };

		return Audio::ConvertToFloat32(_Format, &GetData()[data_index]);
	}

private:

	//The sample rate.
	uint32 _SampleRate{ 0 };

	//The number of channels.
	uint8 _NumberOfChannels{ 0 };

	//The format.
	Audio::Format _Format{ Audio::Format::UNKNOWN };

	//The number of samples.
	uint32 _NumberOfSamples{ 0 };

	/*
	*	The data can be held either internally within the audio stream itself, or as a pointer to an external source.
	*	If the data is internal, the audio stream cleans it up upon deletion.
	*	If external, the responsibility is on whoever passed the pointer to clean the data up.
	*	For multi-channel data, the data is expected to be interleaved,
	*	so for example for stereo we would have '[Sample 1 Left, Sample 1 Right, Sample 2 Left, Sample 2 Right]' etc.
	*/
	struct
	{
		//The internal data.
		DynamicArray<byte> _Internal;

		//The external data.
		const byte *RESTRICT _External{ nullptr };
	} _Data;

	/*
	*	Returns the data.
	*/
	FORCE_INLINE NO_DISCARD const byte *const RESTRICT GetData() const NOEXCEPT
	{
		ASSERT((!_Data._Internal.Empty() && !_Data._External) || (_Data._Internal.Empty() && _Data._External), "Either not data is set or it is set both internally or externally.");

		return _Data._Internal.Empty() ? _Data._External : _Data._Internal.Data();
	}

};