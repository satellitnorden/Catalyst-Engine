#if defined(USE_NEW_AUDIO_SYSTEM)
//Header file.
#include <Systems/AudioSystem.h>

//Audio.
#include <Audio/Backends/WASAPIAudioBackend.h>

//Systems.
#include <Systems/CatalystEngineSystem.h>

/*
*	Initializes the audio system.
*/
void AudioSystem::Initialize() NOEXCEPT
{
	//Initialize the backend.
	InitializeBackend(Audio::Backend::WASAPI);

	//Launch the mix thread.
	_MixThread.SetFunction([]() { AudioSystem::Instance->Mix(); });
	_MixThread.SetPriority(Thread::Priority::HIGHEST);
#if !defined(CATALYST_CONFIGURATION_FINAL)
	_MixThread.SetName("Audio System - Mix Thread");
#endif

	_MixThread.Launch();
}

/*
*	Terminates the audio system.
*/
void AudioSystem::Terminate() NOEXCEPT
{
	//Join the mix thread.
	_MixThread.Join();
}

/*
*	Plays the given audio (in 2D).
*/
void AudioSystem::PlayAudio2D
(
	const AssetPointer<AudioAsset> asset,
	const float32 gain
) NOEXCEPT
{
	//Add the request.
	Request request;

	request._Type = Request::Type::PLAY_AUDIO_2D;
	request._PlayAudio2DData._Asset = asset;
	request._PlayAudio2DData._Gain = gain;

	_Requests.Push(request);
}

/*
*	Initializes the backend.
*/
void AudioSystem::InitializeBackend(const Audio::Backend backend) NOEXCEPT
{
	//Terminate the previous backend, if one exists.
	TerminateBackend();

	switch (backend)
	{
		case Audio::Backend::NONE:
		{
			//Nothing to do here! Should be valid to set up a "none" backend to disable audio.

			break;
		}

		case Audio::Backend::WASAPI:
		{
			_Backend = new WASAPIAudioBackend(CreateBackendParameters());

			break;
		}

		default:
		{
			ASSERT(false, "Invalid case!");

			break;
		}
	}
}

/*
*	Creates the backend parameters.
*/
NO_DISCARD AudioBackend::Parameters AudioSystem::CreateBackendParameters() NOEXCEPT
{
	AudioBackend::Parameters parameters;

	parameters._StartInputChannelIndex = 0;
	parameters._NumberOfInputChannels = 0;
	parameters._StartOutputChannelIndex = 0;
	parameters._NumberOfOutputChannels = 2;
	parameters._ProcessCallback = [](const DynamicArray<DynamicArray<float32>> &inputs, DynamicArray<DynamicArray<float32>> *const RESTRICT outputs)
	{
		AudioSystem::Instance->Process(inputs, outputs);
	};

	return parameters; 
}

/*
*	Terminates the backend.
*/
void AudioSystem::TerminateBackend() NOEXCEPT
{
	if (_Backend)
	{
		delete _Backend;
		_Backend = nullptr;
	}
}

/*
*	Mixes.
*/
void AudioSystem::Mix() NOEXCEPT
{
	while (!CatalystEngineSystem::Instance->ShouldTerminate())
	{
		//Process requests.
		const bool processed_any_request{ ProcessRequests() };

		//If there were no requests, yield for some time. (:
		if (!processed_any_request)
		{
			Concurrency::CurrentThread::Yield();
		}
	}
}

/*
*	Processes requests.
*/
NO_DISCARD bool AudioSystem::ProcessRequests() NOEXCEPT
{
	bool processed_any_request{ false };

	Optional<Request> request{ _Requests.Pop() };

	while (request.Valid())
	{
		const Request _request{ request.Get() };

		switch (_request._Type)
		{
			case Request::Type::NONE:
			{
				ASSERT(false, "Invalid request!");

				break;
			}

			case Request::Type::MIX_BUFFER:
			{
				ProcessMixBufferRequest(_request);

				break;
			}

			case Request::Type::PLAY_AUDIO_2D:
			{
				ProcessPlayAudio2DRequest(_request);

				break;
			}

			default:
			{
				ASSERT(false, "Invalid case!");

				break;
			}
		}

		processed_any_request = true;

		request = _Requests.Pop();
	}

	return processed_any_request;
}

/*
*	Processes a mix buffer request.
*/
void AudioSystem::ProcessMixBufferRequest(const Request &request) NOEXCEPT
{
	//Cache the mix buffer.
	MixBuffer &mix_buffer{ _MixBuffers[request._MixBufferData._BufferIndex] };

	//Cache the number of channels.
	const uint8 number_of_channels{ static_cast<uint8>(mix_buffer._Outputs.Size()) };

	//Cache the number of samples.
	const uint32 number_of_samples{ static_cast<uint32>(mix_buffer._Outputs[0].Size()) };

	//Process all samples!
	for (uint32 sample_index{ 0 }; sample_index < number_of_samples; ++sample_index)
	{
		for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
		{
			//Calculate the sample.
			float32 sample{ 0.0f };

			//Iterate through all 2D players.
			for (const AudioStreamPlayer &player : _2DPlayers)
			{
				sample += player.Sample(channel_index);
			}

			//Write the sample!
			mix_buffer._Outputs[channel_index][sample_index] = sample;
		}

		//Advance all 2D players.
		for (AudioStreamPlayer &player : _2DPlayers)
		{
			player.Advance();
		}
	}

	//The mix buffer is now mixed!
	mix_buffer._IsMixed.Set();
}

/*
*	Processes a play audio 2D request.
*/
void AudioSystem::ProcessPlayAudio2DRequest(const Request &request) NOEXCEPT
{
	//Add the new player.
	_2DPlayers.Emplace();
	AudioStreamPlayer &new_player{ _2DPlayers.Back() };

	new_player.SetAudioStream(&request._PlayAudio2DData._Asset->_AudioStream);
	new_player.SetGain(request._PlayAudio2DData._Gain);
	new_player.SetPlaybackRate(static_cast<float32>(request._PlayAudio2DData._Asset->_AudioStream.GetSampleRate()) / _Backend->_SampleRate);
}

/*
*	Processes.
*/
void AudioSystem::Process(const DynamicArray<DynamicArray<float32>> &inputs, DynamicArray<DynamicArray<float32>> *const RESTRICT outputs) NOEXCEPT
{
	//Cache the current mix buffer.
	MixBuffer &current_mix_buffer{ _MixBuffers[_CurrentMixBufferIndex] };

	//Determine if the current mix buffer is valid.
	bool current_mix_buffer_valid{ true };

	current_mix_buffer_valid &= current_mix_buffer._Outputs.Size() == outputs->Size();

	if (current_mix_buffer_valid)
	{
		for (uint64 i{ 0 }; i < current_mix_buffer._Outputs.Size() && current_mix_buffer_valid; ++i)
		{
			current_mix_buffer_valid &= current_mix_buffer._Outputs[i].Size() == (*outputs)[i].Size();
		}
	}

	//If the current mix buffer is valid, copy the data.
	if (current_mix_buffer_valid)
	{
		//Wait for it to complete.
		current_mix_buffer._IsMixed.Wait<WaitMode::PAUSE>();

		//Copy the outputs.
		*outputs = current_mix_buffer._Outputs;
	}

	//Otherwise, zero the outputs.
	else
	{
		for (DynamicArray<float32> &output_channel : (*outputs))
		{
			Memory::Set(output_channel.Data(), 0, sizeof(float32) * output_channel.Size());
		}
	}

	//Copy the inputs into the current mix buffer.
	current_mix_buffer._Inputs = inputs;

	//Ensure that the outputs have the correct size.
	current_mix_buffer._Outputs.Resize<true>(outputs->Size());

	for (uint64 i{ 0 }; i < current_mix_buffer._Outputs.Size(); ++i)
	{
		current_mix_buffer._Outputs[i].Resize<false>((*outputs)[i].Size());
	}

	//Reset the '_IsMixed' flag.
	current_mix_buffer._IsMixed.Clear();

	//Add a request to mix the current mix buffer.
	Request request;

	request._Type = Request::Type::MIX_BUFFER;
	request._MixBufferData._BufferIndex = _CurrentMixBufferIndex;

	_Requests.Push(request);

	//Flip the current mix buffer index.
	_CurrentMixBufferIndex = (_CurrentMixBufferIndex + 1) & 1;
}
#endif