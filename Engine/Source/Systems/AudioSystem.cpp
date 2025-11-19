#if defined(USE_NEW_AUDIO_SYSTEM)
//Header file.
#include <Systems/AudioSystem.h>

//Audio.
#include <Audio/Backends/ASIOAudioBackend.h>
#include <Audio/Backends/WASAPIAudioBackend.h>

//Systems.
#include <Systems/CatalystEngineSystem.h>

/*
*	Initializes the audio system.
*/
void AudioSystem::Initialize() NOEXCEPT
{
	//Add the master audio track.
	_AudioTracks.Emplace();
	AudioTrack &master_audio_track{ _AudioTracks.Back() };

	master_audio_track._Information._Name = "Master";
	master_audio_track._Information._StartChannelIndex = UINT32_MAXIMUM;
	master_audio_track._Information._NumberOfInputChannels = UINT32_MAXIMUM;
	master_audio_track._Identifier = _AudioTrackIdentifierGenerator++;

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
*	Updates the audio system.
*/
void AudioSystem::Update(const UpdatePhase phase) NOEXCEPT
{

}

/*
*	Adds a master audio effect.
*/
void AudioSystem::AddMasterAudioEffect(AudioEffect *const RESTRICT effect) NOEXCEPT
{
	//Add the request.
	Request request;

	request._Type = Request::Type::ADD_AUDIO_EFFECT_TO_TRACK;
	request._AddAudioEffectToTrackData._Identifier = 0;
	request._AddAudioEffectToTrackData._Effect = effect;

	_Requests.Push(request);
}

/*
*	Plays the given audio (in 2D).
*/
Audio::Identifier AudioSystem::PlayAudio2D(const PlayAudio2DRequest &request) NOEXCEPT
{
	//Add the request.
	Request _request;

	_request._Type = Request::Type::PLAY_AUDIO_2D;
	_request._PlayAudio2DData._Identifier = _IdentifierGenerator++;
	_request._PlayAudio2DData._Request = request;

	_Requests.Push(_request);

	return _request._PlayAudio2DData._Identifier;
}

/*
*	Stops the audio 2D with the given identifier.
*/
void AudioSystem::StopAudio2D(const Audio::Identifier identifier) NOEXCEPT
{
	//Add the request.
	Request _request;

	_request._Type = Request::Type::STOP_AUDIO_2D;
	_request._StopAudio2DData._Identifier = identifier;

	_Requests.Push(_request);
}

/*
*	Retrieves the audio time for the given identifier.
*	Please note that this is an asynchronous call - It will send a request to the audio thread and set 'ready' when it has written the value.
*/
void AudioSystem::GetAudioTime(const Audio::Identifier identifier, float64 *const RESTRICT value, AtomicFlag *const RESTRICT ready) NOEXCEPT
{
	//Clear the ready flag.
	ready->Clear();

	//Add the request.
	Request request;

	request._Type = Request::Type::GET_AUDIO_TIME;
	request._GetAudioTimeData._Identifier = identifier;
	request._GetAudioTimeData._Value = value;
	request._GetAudioTimeData._Ready = ready;

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

		case Audio::Backend::ASIO:
		{
			_Backend = new ASIOAudioBackend(CreateBackendParameters());

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

			case Request::Type::ADD_AUDIO_EFFECT_TO_TRACK:
			{
				ProcessAddAudioEffectToTrackRequest(_request);

				break;
			}

			case Request::Type::PLAY_AUDIO_2D:
			{
				ProcessPlayAudio2DRequest(_request);

				break;
			}

			case Request::Type::STOP_AUDIO_2D:
			{
				ProcessStopAudio2DRequest(_request);

				break;
			}

			case Request::Type::MIX_BUFFER:
			{
				ProcessMixBufferRequest(_request);

				break;
			}

			case Request::Type::GET_AUDIO_TIME:
			{
				ProcessGetAudioTimeRequest(_request);

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
*	Processes an add audio effect to track request.
*/
void AudioSystem::ProcessAddAudioEffectToTrackRequest(const Request &request) NOEXCEPT
{
	for (AudioTrack &audio_track : _AudioTracks)
	{
		if (audio_track._Identifier == request._AddAudioEffectToTrackData._Identifier)
		{
			audio_track._Effects.Emplace(request._AddAudioEffectToTrackData._Effect);

			break;
		}
	}
}

/*
*	Processes a play audio 2D request.
*/
void AudioSystem::ProcessPlayAudio2DRequest(const Request &request) NOEXCEPT
{
	//Add the new playing audio.
	_PlayingAudio2D.Emplace();
	PlayingAudio2D &new_playing_audio{ _PlayingAudio2D.Back() };

	new_playing_audio._Identifier = request._PlayAudio2DData._Identifier;
	new_playing_audio._Player.SetAudioStream(&request._PlayAudio2DData._Request._Asset->_AudioStream);
	new_playing_audio._Player.SetGain(request._PlayAudio2DData._Request._Gain);
	new_playing_audio._Player.SetPlaybackRate(static_cast<float32>(request._PlayAudio2DData._Request._Asset->_AudioStream.GetSampleRate()) / _Backend->_SampleRate);
	new_playing_audio._Player.SetCurrentSample(static_cast<int64>(request._PlayAudio2DData._Request._StartTime * static_cast<float32>(request._PlayAudio2DData._Request._Asset->_AudioStream.GetSampleRate())));
}

/*
*	Processes a stop audio 2D request.
*/
void AudioSystem::ProcessStopAudio2DRequest(const Request &request) NOEXCEPT
{
	for (uint64 i{ 0 }; i < _PlayingAudio2D.Size(); ++i)
	{
		if (_PlayingAudio2D[i]._Identifier == request._StopAudio2DData._Identifier)
		{
			_PlayingAudio2D.EraseAt<false>(i);
			break;
		}
	}
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

			//Iterate through all playing audio.
			for (const PlayingAudio2D &playing_audio : _PlayingAudio2D)
			{
				sample += playing_audio._Player.Sample(channel_index);
			}

			//Constrain the sample.
			sample = BaseMath::Clamp<float32>(sample, -1.0f, 1.0f);

			//Write the sample!
			mix_buffer._Outputs[channel_index][sample_index] = sample;
		}

		//Advance all playing audio.
		for (PlayingAudio2D &playing_audio : _PlayingAudio2D)
		{
			playing_audio._Player.Advance();
		}
	}

	//Process the master effects, if there are any.
	if (!_AudioTracks[0]._Effects.Empty())
	{
		AudioProcessContext context;

		context._WasTimelineRunning = true;
		context._IsTimelineRunning = true;

		for (AudioEffect *const RESTRICT master_effect : _AudioTracks[0]._Effects)
		{
			master_effect->Process
			(
				context,
				mix_buffer._Outputs,
				&mix_buffer._Outputs,
				number_of_channels,
				number_of_samples
			);
		}
	}

	//The mix buffer is now mixed!
	mix_buffer._IsMixed.Set();
}

/*
*	Processes a get audio time request.
*/
void AudioSystem::ProcessGetAudioTimeRequest(const Request &request) NOEXCEPT
{
	for (const PlayingAudio2D &playing_audio : _PlayingAudio2D)
	{
		if (playing_audio._Identifier == request._GetAudioTimeData._Identifier)
		{
			(*request._GetAudioTimeData._Value) = static_cast<float64>(playing_audio._Player.GetCurrentSample()) / static_cast<float64>(playing_audio._Player.GetAudioStream()->GetSampleRate());

			break;
		}
	}

	//Whether or not we actually found the value, trust that the caller set a reasonable default value for it. (:
	request._GetAudioTimeData._Ready->Set();
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

	//Advance the current mix buffer index.
	++_CurrentMixBufferIndex;
	_CurrentMixBufferIndex *= static_cast<uint8>(_CurrentMixBufferIndex < NUMBER_OF_MIX_BUFFERS);
}
#endif