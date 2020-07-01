#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Resources.
#include <Resources/Core/SoundResource.h>

//Third party.
#include <ThirdParty/AudioFile.h>

class WAVWriter final
{

public:

	/*
	*	Writes the sound resource to the given file path. Returns if the write was succesful.
	*/
	FORCE_INLINE static NO_DISCARD bool Write(const char *const RESTRICT file_path, const SoundResource &resource) NOEXCEPT
	{
		AudioFile<float32> audio_file;

		AudioFile<float32>::AudioBuffer audio_buffer;

		audio_buffer.resize(2);

		for (size_t i{ 0 }, size{ audio_buffer.size() }; i < size; ++i)
		{
			for (const int16 sample : resource._Samples[i])
			{
				audio_buffer[i].emplace_back(static_cast<float32>(sample) / static_cast<float32>(INT16_MAXIMUM));
			}
		}

		audio_file.setAudioBuffer(audio_buffer);
		audio_file.setBitDepth(16);
		audio_file.setSampleRate(static_cast<uint32>(resource._SampleRate));

		audio_file.save(file_path);

		return true;
	}

};