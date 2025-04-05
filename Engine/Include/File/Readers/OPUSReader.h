#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Content.
#include <Content/Assets/SoundAsset.h>

//Third party.
#include <ThirdParty/libsoundwave/OpusDecoder.h>

class OPUSReader final
{

public:

	/*
	*	Reads the sound asset at the given file path. Returns if the read was succesful.
	*/
	FORCE_INLINE static NO_DISCARD bool Read(const char *const RESTRICT file_path, SoundAsset *const RESTRICT asset) NOEXCEPT
	{
		//Open the audio data.
		soundwave::AudioData audio_data;
		soundwave::OpusDecoder opus_decoder;
		opus_decoder.LoadFromPath(&audio_data, file_path);

		//Set the sample rate.
		asset->_SampleRate = static_cast<float32>(audio_data.sampleRate);

		//Set the number of channels.
		asset->_NumberOfChannels = static_cast<uint8>(audio_data.channelCount);

		//Set up the channels.
		asset->_Samples.Upsize<true>(asset->_NumberOfChannels);

		for (uint8 i{ 0 }; i < asset->_NumberOfChannels; ++i)
		{
			asset->_Samples[i].Reserve(audio_data.samples.size() / asset->_NumberOfChannels);
		}

		//Add the samples.
		for (uint64 i{ 0 }; i < audio_data.samples.size(); ++i)
		{
			asset->_Samples[i % asset->_NumberOfChannels].Emplace(static_cast<int16>(BaseMath::Clamp(audio_data.samples[i], -1.0f, 1.0f) * static_cast<float32>(INT16_MAXIMUM)));
		}

		//Return that the read was successful.
		return true;
	}

};