#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/DynamicString.h>

//Audio.
#include <Audio/Effects/Core/AudioEffect.h>

//File.
#include <File/Writers/WAVWriter.h>

/*
*	Recorder audio effect.
*	Utility effect that records the incoming audio, and on destruction writes it out as a .wav file.
*/
class Recorder final : public AudioEffect
{

public:

	/*
	*	Default constructor.
	*/
	FORCE_INLINE Recorder(const char *const RESTRICT file_path) NOEXCEPT
		:
		_FilePath(file_path)
	{

	}

	/*
	*	Default destructor.
	*/
	FORCE_INLINE ~Recorder() NOEXCEPT
	{
		//Set up the audio stream.
		AudioStream audio_stream;

		audio_stream.SetSampleRate(static_cast<uint32>(_SampleRate));
		audio_stream.SetNumberOfChannels(_NumberOfChannels);
		audio_stream.SetFormat(Audio::Format::FLOAT_32_BIT);
		audio_stream.SetNumberOfSamples(_Samples.Size() / _NumberOfChannels);
		audio_stream.SetDataExternal(reinterpret_cast<const byte *const RESTRICT>(_Samples.Data()));

		//Write the file!
		WAVWriter::Write(_FilePath.Data(), audio_stream);
	}

	/*
	*	Callback for this audio effect to process the given buffer.
	*/
	FORCE_INLINE void Process
	(
		const AudioProcessContext &context,
		const DynamicArray<DynamicArray<float32>> &inputs,
		DynamicArray<DynamicArray<float32>> *const RESTRICT outputs,
		const uint8 number_of_channels,
		const uint32 number_of_samples
	) NOEXCEPT override
	{
		//Copy the inputs into the samples.
		for (uint32 sample_index{ 0 }; sample_index < number_of_samples; ++sample_index)
		{
			for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
			{
				_Samples.Emplace(inputs.At(channel_index).At(sample_index));
			}
		}

		//Copy the inputs into the outputs.
		for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
		{
			Memory::Copy(outputs->At(channel_index).Data(), inputs.At(channel_index).Data(), number_of_samples * sizeof(float32));
		}

		//Set the number of channels.
		_NumberOfChannels = number_of_channels;
	}

private:

	//The file path.
	DynamicString _FilePath;

	//The samples.
	DynamicArray<float32> _Samples;

	//The number of channels.
	uint8 _NumberOfChannels;

};