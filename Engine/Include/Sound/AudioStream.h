#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Sound.
#include <Sound/SoundCore.h>

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
	FORCE_INLINE NO_DISCARD SoundFormat GetFormat() const NOEXCEPT
	{
		return _Format;
	}

	/*
	*	Sets the format.
	*/
	FORCE_INLINE void SetFormat(const SoundFormat value) NOEXCEPT
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
	*	Returns the data.
	*/
	FORCE_INLINE NO_DISCARD const byte *const RESTRICT GetData() const NOEXCEPT
	{
		ASSERT((!_Data._Internal.Empty() && !_Data._External) || (_Data._Internal.Empty() && _Data._External), "Either not data is set or it is set both internally or externally.");
	
		return _Data._Internal.Empty() ? _Data._External : _Data._Internal.Data();
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

		_Data._Internal.Upsize<false>(_NumberOfChannels * _NumberOfChannels);
		Memory::Copy(_Data._Internal.Data(), data, _NumberOfChannels * _NumberOfChannels);
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

private:

	//The sample rate.
	uint32 _SampleRate{ 0 };

	//The number of channels.
	uint8 _NumberOfChannels{ 0 };

	//The format.
	SoundFormat _Format{ SoundFormat::UNKNOWN };

	//The number of samples.
	uint32 _NumberOfSamples{ 0 };

	/*
	*	The data can be held either internally within the audio stream itself, or as a pointer to an external source.
	*	If the data is internal, the audio stream cleans it up upon deletion.
	*	If external, the responsibility is on whoever passed the pointer to clean the data up.
	*	For multi-channel data, the data is expected to be appended after another,
	*	so if we have a stream of 100 samples, the first 100 samples would be channel 1, the next 100 samples be channel 2 etc.
	*/
	struct
	{
		//The internal data.
		DynamicArray<byte> _Internal;

		//The external data.
		const byte *RESTRICT _External{ nullptr };
	} _Data;

};