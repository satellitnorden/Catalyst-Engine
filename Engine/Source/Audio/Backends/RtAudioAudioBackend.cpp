//Header file.
#include <Audio/Backends/RtAudioAudioBackend.h>

//Core.
#include <Core/General/Pair.h>

//Systems.
#include <Systems/LogSystem.h>

#define PROFILE_AUDIO (0)

#if PROFILE_AUDIO
	#include <Core/General/Time.h>
	#include <Systems/LogSystem.h>
#endif

//RT Audio audio backend constants.
namespace RtAudioAudioBackendConstants
{
	constexpr Pair<Audio::Format, RtAudioFormat> FORMAT_LOOKUP[]
	{
		Pair<Audio::Format, RtAudioFormat>(Audio::Format::INTEGER_8_BIT, RTAUDIO_SINT8),
		Pair<Audio::Format, RtAudioFormat>(Audio::Format::INTEGER_16_BIT, RTAUDIO_SINT16),
		Pair<Audio::Format, RtAudioFormat>(Audio::Format::INTEGER_24_BIT, RTAUDIO_SINT24),
		Pair<Audio::Format, RtAudioFormat>(Audio::Format::INTEGER_32_BIT, RTAUDIO_SINT32),
		Pair<Audio::Format, RtAudioFormat>(Audio::Format::FLOAT_32_BIT, RTAUDIO_FLOAT32),
		Pair<Audio::Format, RtAudioFormat>(Audio::Format::FLOAT_64_BIT, RTAUDIO_FLOAT64)
	};

	constexpr RtAudioFormat PREFERRED_FORMATS[]
	{
		RTAUDIO_FLOAT32,
		RTAUDIO_SINT32,
		RTAUDIO_SINT24,
		RTAUDIO_SINT16,
		RTAUDIO_FLOAT64,
		RTAUDIO_SINT8
	};
}

/*
*	The error callback.
*/
static void ErrorCallback(RtAudioErrorType type, const std::string& error_text) NOEXCEPT
{
	switch (type)
	{
		case RtAudioErrorType::RTAUDIO_NO_ERROR:
		{
			ASSERT(false, "This should never happen?");

			break;
		}

		case RtAudioErrorType::RTAUDIO_WARNING:
		{
			LOG_WARNING("Warning: %s", error_text.c_str());

			break;
		}

		case RtAudioErrorType::RTAUDIO_UNKNOWN_ERROR:
		case RtAudioErrorType::RTAUDIO_NO_DEVICES_FOUND:
		case RtAudioErrorType::RTAUDIO_INVALID_DEVICE:
		case RtAudioErrorType::RTAUDIO_DEVICE_DISCONNECT:
		case RtAudioErrorType::RTAUDIO_MEMORY_ERROR:
		case RtAudioErrorType::RTAUDIO_INVALID_PARAMETER:
		case RtAudioErrorType::RTAUDIO_INVALID_USE:
		case RtAudioErrorType::RTAUDIO_DRIVER_ERROR:
		case RtAudioErrorType::RTAUDIO_SYSTEM_ERROR:
		case RtAudioErrorType::RTAUDIO_THREAD_ERROR:
		{
			LOG_ERROR("Error: %s", error_text.c_str());

			break;
		}

		default:
		{
			ASSERT(false, "Unhandled case!");

			break;
		}
	}
}

/*
*	Default constructor.
*/
RtAudioAudioBackend::RtAudioAudioBackend
(
	const Parameters &parameters,
	const Audio::Backend backend,
	const RtAudio::Api api,
	const uint32 wanted_buffer_size
) NOEXCEPT
	:
	AudioBackend(parameters)
{
	//Set the backend.
	_Backend = backend;

	//Create the RT Audio object.
	_RtAudio = new RtAudio(api, ErrorCallback);

	//Cache the audio device informations - It seems RtAudio doesn't quite like doing it while a stream is open, so better to do it here. (:
	{
		const uint32 device_count{ _RtAudio->getDeviceCount() };
		const std::vector<uint32> device_ids{ _RtAudio->getDeviceIds()};

		_AudioDeviceInformations.Reserve(device_count);

		for (uint32 device_index{ 0 }; device_index < device_count; ++device_index)
		{
			_AudioDeviceInformations.Emplace();
			AudioDeviceInformation &audio_device_information{ _AudioDeviceInformations.Back() };

			const uint32 device_id{ device_ids[device_index] };
			const RtAudio::DeviceInfo &device_info{ _RtAudio->getDeviceInfo(device_id) };

			audio_device_information._Backend = backend;
			audio_device_information._Name = device_info.name.c_str();
			audio_device_information._Identifier = device_id;
			audio_device_information._IsDefault = device_info.isDefaultOutput;
		}
	}

	//Figure out the device identifier.
	uint32 device_identifier;

	if (parameters._RequestedDeviceIdentifier != UINT32_MAXIMUM)
	{
		device_identifier = parameters._RequestedDeviceIdentifier;
	}

	else
	{
		device_identifier = _RtAudio->getDefaultOutputDevice();
	}

	//Cache the device info.
	const RtAudio::DeviceInfo device_info{ _RtAudio->getDeviceInfo(device_identifier) };

	//Set up the output stream parameters.
	RtAudio::StreamParameters output_stream_parameters;

	output_stream_parameters.deviceId = device_identifier;
	output_stream_parameters.nChannels = _Parameters._NumberOfOutputChannels;
	output_stream_parameters.firstChannel = _Parameters._StartOutputChannelIndex;

	//Set up the input stream parameters.
	RtAudio::StreamParameters input_stream_parameters;

	input_stream_parameters.deviceId = device_identifier;
	input_stream_parameters.nChannels = BaseMath::Minimum<uint32>(_Parameters._NumberOfInputChannels, BaseMath::Maximum<int32>(device_info.inputChannels - _Parameters._StartInputChannelIndex, 0));
	input_stream_parameters.firstChannel = BaseMath::Minimum<uint32>(_Parameters._StartInputChannelIndex, BaseMath::Maximum<int32>(device_info.inputChannels - 1, 0));

	//Figure out the format.
	RtAudioFormat format = 0;

	for (uint64 i{ 0 }; i < ARRAY_LENGTH(RtAudioAudioBackendConstants::PREFERRED_FORMATS); ++i)
	{
		if (TEST_BIT(device_info.nativeFormats, RtAudioAudioBackendConstants::PREFERRED_FORMATS[i]))
		{
			format = RtAudioAudioBackendConstants::PREFERRED_FORMATS[i];

			break;
		}
	}

	ASSERT(format != 0, "Couldn't find a format for WASAPI stream!");

	//Figure out the sample rate.
	const uint32 sample_rate{ device_info.preferredSampleRate };

	//Set up the buffer frames for a decent default.
	uint32 buffer_frames{ wanted_buffer_size };

	//Set up the audio callback.
	constexpr auto AUDIO_CALLBACK
	{
		[](void *output_buffer, void *input_buffer, unsigned int number_of_samples, double stream_time, RtAudioStreamStatus status, void *user_data)
		{
			static_cast<RtAudioAudioBackend *const RESTRICT>(user_data)->AudioCallback(output_buffer, input_buffer, number_of_samples, stream_time, status);

			return 0;
		}
	};

	//Set up the stream options.
	RtAudio::StreamOptions stream_options;

	stream_options.flags = RTAUDIO_NONINTERLEAVED | RTAUDIO_MINIMIZE_LATENCY | RTAUDIO_SCHEDULE_REALTIME;
	stream_options.numberOfBuffers = 0;
	stream_options.streamName = "Catalyst Engine Audio Backend Stream";
	stream_options.priority = 0;

	//Open the stream!
	_RtAudio->openStream
	(
		&output_stream_parameters,
		input_stream_parameters.nChannels > 0 ? &input_stream_parameters : nullptr,
		format,
		sample_rate,
		&buffer_frames,
		AUDIO_CALLBACK,
		this,
		&stream_options
	);

	//Start the stream!
	_RtAudio->startStream();

	//Set the format.
	for (const Pair<Audio::Format, RtAudioFormat> &format_lookup : RtAudioAudioBackendConstants::FORMAT_LOOKUP)
	{
		if (format_lookup._Second == format)
		{
			_Format = format_lookup._First;

			break;
		}
	}

	ASSERT(_Format != Audio::Format::UNKNOWN, "Could not lookup audio format!");

	//Set the sample rate.
	_SampleRate = static_cast<float32>(sample_rate);

	//Set the buffer size.
	_BufferSize = buffer_frames;

	//Set the number of input channels.
	_NumberOfInputChannels = input_stream_parameters.nChannels;

	//Set the number of output channels.
	_NumberOfOutputChannels = output_stream_parameters.nChannels;
}

/*
*	Default destructor.
*/
RtAudioAudioBackend::~RtAudioAudioBackend() NOEXCEPT
{
	//Stop the stream.
	_RtAudio->stopStream();

	//Close the stream.
	_RtAudio->closeStream();

	//Destroy the RT Audio object.
	delete _RtAudio;
}

/*
*	The audio callback.
*/
void RtAudioAudioBackend::AudioCallback(void *output_buffer, void *input_buffer, unsigned int number_of_samples, double stream_time, RtAudioStreamStatus status)
{
#if PROFILE_AUDIO
	TimePoint time_point;
#endif

	//Cache the bytes per sample.
	const uint8 bytes_per_sample{ static_cast<uint8>(Audio::BitsPerSample(_Format) >> 3) };

	//Set up the inputs.
	if (_NumberOfInputChannels > 0)
	{
		_Inputs.Resize<true>(_NumberOfInputChannels);

		for (uint8 channel_index{ 0 }; channel_index < _NumberOfInputChannels; ++channel_index)
		{
			DynamicArray<float32> &input_channel{ _Inputs[channel_index] };
			input_channel.Resize<false>(number_of_samples);
			Audio::ConvertToFloat32(_Format, AdvancePointer(input_buffer, bytes_per_sample * number_of_samples * channel_index), input_channel.Data(), number_of_samples);
		}
	}

	else
	{
		_Inputs.Clear();
	}

	//Set up the outputs.
	if (_NumberOfOutputChannels > 0)
	{
		_Outputs.Resize<true>(_NumberOfOutputChannels);

		for (DynamicArray<float32> &output_channel : _Outputs)
		{
			output_channel.Resize<false>(number_of_samples);
			Memory::Set(output_channel.Data(), 0, sizeof(float32) * number_of_samples);
		}
	}

	else
	{
		_Outputs.Clear();
	}

	//Call the process callback.
	_Parameters._ProcessCallback(_Inputs, &_Outputs);

	//Now write the outputs.
	for (uint8 channel_index{ 0 }; channel_index < _NumberOfOutputChannels; ++channel_index)
	{
		DynamicArray<float32> &output_channel{ _Outputs[channel_index] };
		Audio::ConvertToSample(_Format, output_channel.Data(), AdvancePointer(output_buffer, bytes_per_sample * number_of_samples * channel_index), number_of_samples);
	}

#if PROFILE_AUDIO
	{
		static StaticArray<float64, 4'096> sample_durations;
		static uint64 number_of_sample_durations{ 0 };

		sample_durations[number_of_sample_durations % sample_durations.Size()] = time_point.GetSecondsSince();
		++number_of_sample_durations;

		if (number_of_sample_durations % sample_durations.Size() == 0)
		{
			float64 maximum_duration{ 0.0 };
			float64 accumulated_duration{ 0.0 };

			for (const float64 sample_duration : sample_durations)
			{
				maximum_duration = BaseMath::Maximum<float64>(maximum_duration, sample_duration);
				accumulated_duration += sample_duration;
			}

			accumulated_duration /= static_cast<float64>(sample_durations.Size());

			const float64 deadline_duration{ static_cast<float64>(number_of_samples) / static_cast<float64>(_SampleRate) };

			LOG_INFORMATION("Rt Audio profiling: Max: %f ms - Average: %f ms / Deadline: %f ms", maximum_duration * 1'000.0, accumulated_duration * 1'000.0, deadline_duration * 1'000.0);
		}
	}
#endif
}