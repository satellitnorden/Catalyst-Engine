#pragma once

/*
*	This file is a bunch of functionality for dealing with Binary Audio Data files, or .bad files.
*	This is a file format that grew out of the necessity of having larger than 4GB audio files, which is the limit for .wav files, for example.
*	The layout of a .bad file is as follows:
*	4 bytes: A uint32 denoting the sample rate.
*	8 bytes: A uint64 denoting the number of samples present in the files.
*	? bytes: The actual sample data. This will be sizeof(float32) * number of samples.
*/

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/ArrayProxy.h>
#include <Core/Containers/DynamicArray.h>

//File.
#include <File/Core/BinaryInputFile.h>
#include <File/Core/BinaryOutputFile.h>

//Third party.
#include <ThirdParty/AudioFile/AudioFile.h>

/*
*	Binary audio data class definition.
*/
class BinaryAudioData final
{

public:

	//The sample rate.
	uint32 _SampleRate{ 0 };

	//The samples.
	DynamicArray<float32> _Samples;

	/*
	*	Default constructor.
	*/
	FORCE_INLINE BinaryAudioData() NOEXCEPT
	{

	}

	/*
	*	Creates this binary audio data object from a .bad file.
	*	If data already exists in this object, it will be appended.
	*/
	FORCE_INLINE void FromBADFile(const char const *RESTRICT file_path) NOEXCEPT
	{
		//Open the file.
		BinaryInputFile file{ file_path };

		//Read the sample rate.
		{
			const uint32 old_sample_rate{ _SampleRate };
			file.Read(&_SampleRate, sizeof(uint32));
			ASSERT(old_sample_rate == 0 || old_sample_rate == _SampleRate, "Mismatching sample rate!");
		}

		//Read the number of samples.
		uint64 number_of_samples;
		file.Read(&number_of_samples, sizeof(uint64));

		//Read the samples.
		_Samples.Upsize<false>(number_of_samples);
		file.Read(_Samples.Data(), sizeof(float32) * number_of_samples);

		//Close the file.
		file.Close();
	}

	/*
	*	Creates this binary audio data object from a .wav file.
	*	If data already exists in this object, it will be appended.
	*/
	FORCE_INLINE void FromWAVFile(const char const* RESTRICT file_path) NOEXCEPT
	{
		//Read the audio file.
		AudioFile<float32> audio_file;

		if (!audio_file.load(file_path))
		{
			ASSERT(false, "Couldn't read audio file!");
		}

		//Do some sanity checks.
		ASSERT(_SampleRate == 0 || _SampleRate == audio_file.getSampleRate(), "Mismatching sample rate!");
		ASSERT(audio_file.getNumChannels() == 1, "Only mono files are not supported!");

		//Set the sample rate.
		_SampleRate = audio_file.getSampleRate();

		//Copy over the samples.
		const uint64 old_size{ _Samples.Size() };
		_Samples.Upsize<false>(_Samples.Size() + audio_file.samples[0].size());
		Memory::Copy(&_Samples[old_size], audio_file.samples[0].data(), sizeof(float32) * audio_file.samples[0].size());
	}

	/*
	*	Exports this binary audio data object to the given file path.
	*/
	FORCE_INLINE void Export(const char *const RESTRICT file_path) NOEXCEPT
	{
		//Open the output file.
		BinaryOutputFile output_file{ file_path };

		//Write the sample rate.
		output_file.Write(&_SampleRate, sizeof(uint32));

		//Write the number of samples.
		const uint64 number_of_samples{ _Samples.Size() };
		output_file.Write(&number_of_samples, sizeof(uint64));

		//Write the samples.
		output_file.Write(_Samples.Data(), sizeof(float32) * number_of_samples);

		//Close the output file.
		output_file.Close();
	}

};