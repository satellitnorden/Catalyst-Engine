#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Audio.
#include <Audio/AudioStream.h>

//Third party.
#include <ThirdParty/libsoundwave/OpusDecoder.h>

class OPUSReader final
{

public:

	/*
	*	Reads the sound asset at the given file path. Returns if the read was succesful.
	*/
	FORCE_INLINE static NO_DISCARD bool Read(const char *const RESTRICT file_path, AudioStream *const RESTRICT audio_stream) NOEXCEPT
	{
		//Open the audio data.
		soundwave::AudioData audio_data;
		soundwave::OpusDecoder opus_decoder;
		opus_decoder.LoadFromPath(&audio_data, file_path);

		//Set the sample rate.
		audio_stream->SetSampleRate(static_cast<uint32>(audio_data.sampleRate));

		//Set the number of channels.
		audio_stream->SetNumberOfChannels(static_cast<uint8>(audio_data.channelCount));

		//Set the format.
		audio_stream->SetFormat(Audio::Format::FLOAT_32_BIT);

		//Set the number of samples.
		audio_stream->SetNumberOfSamples(audio_data.samples.size() / audio_data.channelCount);

		//Set the data.
		audio_stream->SetDataInternal(reinterpret_cast<const byte *const RESTRICT>(audio_data.samples.data()));

		//Return that the read was successful.
		return true;
	}

};