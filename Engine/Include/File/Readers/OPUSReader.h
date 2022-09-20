#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Resources.
#include <Resources/Core/SoundResource.h>

//Third party.
#include <ThirdParty/libsoundwave/OpusDecoder.h>

class OPUSReader final
{

public:

	/*
	*	Reads the sound resource at the given file path. Returns if the read was succesful.
	*/
	FORCE_INLINE static NO_DISCARD bool Read(const char *const RESTRICT file_path, SoundResource *const RESTRICT resource) NOEXCEPT
	{
		//Open the audio data.
		soundwave::AudioData audio_data;
		soundwave::OpusDecoder opus_decoder;
		opus_decoder.LoadFromPath(&audio_data, file_path);

		//Set the sample rate.
		resource->_SampleRate = static_cast<float32>(audio_data.sampleRate);

		//Set the number of channels.
		resource->_NumberOfChannels = static_cast<uint8>(audio_data.channelCount);

		//Set up the channels.
		resource->_Samples.Upsize<true>(resource->_NumberOfChannels);

		for (uint8 i{ 0 }; i < resource->_NumberOfChannels; ++i)
		{
			resource->_Samples[i].Reserve(audio_data.samples.size() / resource->_NumberOfChannels);
		}

		//Add the samples.
		for (uint64 i{ 0 }; i < audio_data.samples.size(); ++i)
		{
			resource->_Samples[i % resource->_NumberOfChannels].Emplace(static_cast<int16>(CatalystBaseMath::Clamp(audio_data.samples[i], -1.0f, 1.0f) * static_cast<float32>(INT16_MAXIMUM)));
		}

		//Return that the read was successful.
		return true;
	}

};