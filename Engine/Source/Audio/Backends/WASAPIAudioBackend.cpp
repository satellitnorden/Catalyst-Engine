//Header file.
#include <Audio/Backends/WASAPIAudioBackend.h>

//Core.
#include <Core/General/Pair.h>

//Systems.
#include <Systems/LogSystem.h>

//WASAPI audio backend constants.
namespace WASAPAudioBackendConstants
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
WASAPIAudioBackend::WASAPIAudioBackend(const Parameters &parameters) NOEXCEPT
	:
	AudioBackend(parameters)
{
	//Create the RT Audio object.
	_RtAudio = new RtAudio(RtAudio::Api::WINDOWS_WASAPI, ErrorCallback);

	//Cache the default device index.
	const uint32 default_device_index{ _RtAudio->getDefaultOutputDevice() };

	//Cache the default device info.
	const RtAudio::DeviceInfo default_device_info{ _RtAudio->getDeviceInfo(default_device_index) };

	//Set up the output stream parameters.
	RtAudio::StreamParameters output_stream_parameters;

	output_stream_parameters.deviceId = default_device_index;
	output_stream_parameters.nChannels = _Parameters._NumberOfOutputChannels;
	output_stream_parameters.firstChannel = _Parameters._StartOutputChannelIndex;

	//Figure out the format.
	RtAudioFormat format = 0;

	for (uint64 i{ 0 }; i < ARRAY_LENGTH(WASAPAudioBackendConstants::PREFERRED_FORMATS); ++i)
	{
		if (TEST_BIT(default_device_info.nativeFormats, WASAPAudioBackendConstants::PREFERRED_FORMATS[i]))
		{
			format = WASAPAudioBackendConstants::PREFERRED_FORMATS[i];

			break;
		}
	}

	ASSERT(format != 0, "Couldn't find a format for WASAPI stream!");

	//Figure out the sample rate.
	const uint32 sample_rate{ default_device_info.preferredSampleRate };

	//Set up the buffer frames for a decent default.
	uint32 buffer_frames{ 128 };

	//Set up the audio callback.
	constexpr auto AUDIO_CALLBACK
	{
		[](void *output_buffer, void *input_buffer, unsigned int number_of_samples, double stream_time, RtAudioStreamStatus status, void *user_data)
		{
			static_cast<WASAPIAudioBackend *const RESTRICT>(user_data)->AudioCallback(output_buffer, input_buffer, number_of_samples, stream_time, status);

			return 0;
		}
	};

	//Set up the stream options.
	RtAudio::StreamOptions stream_options;

	stream_options.flags = RTAUDIO_SCHEDULE_REALTIME;
	stream_options.numberOfBuffers = 0;
	stream_options.streamName = "Catalyst Engine WASAPI Audio Stream";
	stream_options.priority = 0;

	//Open the stream!
	_RtAudio->openStream
	(
		&output_stream_parameters,
		nullptr,
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
	for (const Pair<Audio::Format, RtAudioFormat> &format_lookup : WASAPAudioBackendConstants::FORMAT_LOOKUP)
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
	_NumberOfInputChannels = 0;

	//Set the number of output channels.
	_NumberOfOutputChannels = 2;
}

/*
*	The audio callback.
*/
void WASAPIAudioBackend::AudioCallback(void *output_buffer, void *input_buffer, unsigned int number_of_samples, double stream_time, RtAudioStreamStatus status)
{
	//Cache the bytes per sample.
	const uint8 bytes_per_sample{ static_cast<uint8>(Audio::BitsPerSample(_Format) >> 3) };

	//Set up the inputs.
	if (_NumberOfInputChannels > 0)
	{
		_Inputs.Resize<true>(_NumberOfInputChannels);

		for (DynamicArray<float32> &input_channel : _Inputs)
		{
			input_channel.Resize<false>(number_of_samples);
			
			ASSERT(false, "Implement the rest of this!");
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
	{
		uint32 output_sample_index{ 0 };

		for (uint32 sample_index{ 0 }; sample_index < number_of_samples; ++sample_index)
		{
			for (uint32 channel_index{ 0 }; channel_index < _NumberOfOutputChannels; ++channel_index)
			{
				Audio::ConvertToSample(_Format, _Outputs[channel_index][sample_index], AdvancePointer<void>(output_buffer, bytes_per_sample * (output_sample_index++)));
			}
		}
	}
}