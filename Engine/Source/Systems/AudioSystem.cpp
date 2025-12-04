//Header file.
#include <Systems/AudioSystem.h>

//Core.
#include <Core/General/SIMD.h>

//Audio.
#include <Audio/Backends/ASIOAudioBackend.h>
#include <Audio/Backends/WASAPIAudioBackend.h>

//Systems.
#include <Systems/CatalystEngineSystem.h>
#include <Systems/LogSystem.h>

/*
*	Initializes the audio system.
*/
void AudioSystem::Initialize() NOEXCEPT
{
	//Add the master audio track.
	{
		AudioTrackInformation master_audio_track_information;

		master_audio_track_information._Name = "Master";
		master_audio_track_information._StartChannelIndex = UINT32_MAXIMUM;
		master_audio_track_information._NumberOfInputChannels = UINT32_MAXIMUM;

		AddAudioTrack(master_audio_track_information);
	}

	//Initialize the backend.
	if (!_Backend)
	{
		InitializeBackend(_RequestedBackend);
	}

	//The mix thread should start mixing right away.
	_ShouldMix.Set();

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
	//Terminate the backend.
	TerminateBackend();

	//Tell the mix thread to shop mixing.
	_ShouldMix.Clear();

	//Join the mix thread.
	_MixThread.Join();
}

/*
*	Returns the default audio device.
*/
NO_DISCARD AudioDeviceInformation AudioSystem::GetDefaultAudioDevice() const NOEXCEPT
{
	DynamicArray<AudioDeviceInformation> audio_devices;
	QueryAudioDevices(&audio_devices);

	for (const AudioDeviceInformation &audio_device : audio_devices)
	{
		if (audio_device._IsDefault)
		{
			return audio_device;
		}
	}

	ASSERT(false, "Couldn't find default audio device!");

	return AudioDeviceInformation();
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
*	Adds an audio track with the given information. Returns it's identifier.
*/
Audio::Identifier AudioSystem::AddAudioTrack(const AudioTrackInformation &information) NOEXCEPT
{
	//Terminate the backend if this track has input channels.
	if (information._NumberOfInputChannels > 0)
	{
		TerminateBackend();
	}

	//Add the main thread audio track immediately.
	_MainThreadAudioTracks.Emplace();
	AudioTrack &main_thread_audio_track{ _MainThreadAudioTracks.Back() };

	main_thread_audio_track._Information = information;
	main_thread_audio_track._Identifier = _AudioTrackIdentifierGenerator++;

	//Add the request.
	Request request;

	request._Type = Request::Type::ADD_AUDIO_TRACK;
	request._AddAudioTrackData._Information = information;
	request._AddAudioTrackData._Identifier = main_thread_audio_track._Identifier;

	_Requests.Push(request);

	//If the audio track has any input channels, (probably) has to reconstruct the backend.
	if (information._NumberOfInputChannels > 0)
	{
		InitializeBackend(_RequestedBackend);
	}

	//Return the identifier.
	return main_thread_audio_track._Identifier;
}

/*
*	Removes the audio track with the given identifier.
*/
void AudioSystem::RemoveAudioTrack(const Audio::Identifier identifier) NOEXCEPT
{
	//Remove the main thread audio track. Remember if it had inputs.
	bool removed_track_had_inputs{ false };

	for (uint64 i{ 0 }; i < _MainThreadAudioTracks.Size(); ++i)
	{
		if (_MainThreadAudioTracks[i]._Identifier == identifier)
		{
			removed_track_had_inputs = _MainThreadAudioTracks[i]._Information._NumberOfInputChannels > 0;

			_MainThreadAudioTracks.EraseAt<false>(i);

			break;
		}
	}

	//If the track had inputs, reconstruct the backend.
	if (removed_track_had_inputs)
	{
		InitializeBackend(_RequestedBackend);
	}

	//Add the request.
	Request request;

	request._Type = Request::Type::REMOVE_AUDIO_TRACK;
	request._RemoveAudioTrackData._Identifier = identifier;

	_Requests.Push(request);
}

/*
*	Adds an effect to the track with the given identifier.
*/
void AudioSystem::AddEffectToAudioTrack(const Audio::Identifier identifier, AudioEffect *const RESTRICT effect) NOEXCEPT
{
	//Add the request.
	Request request;

	request._Type = Request::Type::ADD_AUDIO_EFFECT_TO_TRACK;
	request._AddAudioEffectToTrackData._Identifier = identifier;
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

	//Set the sample rate.
	_SampleRate.Store(static_cast<float32>(_Backend->_SampleRate));
}

/*
*	Creates the backend parameters.
*/
NO_DISCARD AudioBackend::Parameters AudioSystem::CreateBackendParameters() NOEXCEPT
{
	AudioBackend::Parameters parameters;

	if (_RequestedAudioDevice._Backend != Audio::Backend::NONE && _RequestedAudioDevice._Backend == _RequestedBackend)
	{
		parameters._RequestedDeviceIdentifier = _RequestedAudioDevice._Identifier;
	}

	else
	{
		parameters._RequestedDeviceIdentifier = UINT32_MAXIMUM;
	}

	parameters._StartInputChannelIndex = 0;
	parameters._NumberOfInputChannels = 0;

	{
		uint32 minimum_input_channel_index{ UINT32_MAXIMUM };
		uint32 maximum_input_channel_index{ 0 };

		for (const AudioTrack &main_thread_audio_track : _MainThreadAudioTracks)
		{
			if (main_thread_audio_track._Information._StartChannelIndex != UINT32_MAXIMUM && main_thread_audio_track._Information._NumberOfInputChannels != UINT32_MAXIMUM)
			{
				minimum_input_channel_index = BaseMath::Minimum<uint32>(minimum_input_channel_index, main_thread_audio_track._Information._StartChannelIndex);
				maximum_input_channel_index = BaseMath::Maximum<uint32>(maximum_input_channel_index, main_thread_audio_track._Information._StartChannelIndex + main_thread_audio_track._Information._NumberOfInputChannels);
			}
		}

		if (minimum_input_channel_index != UINT32_MAXIMUM && maximum_input_channel_index != 0)
		{
			parameters._StartInputChannelIndex = minimum_input_channel_index;
			parameters._NumberOfInputChannels = maximum_input_channel_index - minimum_input_channel_index;
		}
		
		else
		{
			parameters._StartInputChannelIndex = 0;
			parameters._NumberOfInputChannels = 0;
		}
	}

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
	//Disable float denormals.
	_MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);
	_MM_SET_DENORMALS_ZERO_MODE(_MM_DENORMALS_ZERO_ON);

	while (_ShouldMix.IsSet())
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

			case Request::Type::ADD_AUDIO_TRACK:
			{
				ProcessAddAudioTrackRequest(_request);

				break;
			}

			case Request::Type::REMOVE_AUDIO_TRACK:
			{
				ProcessRemoveAudioTrackRequest(_request);

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
*	Processes an add audio track request.
*/
void AudioSystem::ProcessAddAudioTrackRequest(const Request &request) NOEXCEPT
{
	//Add the audio track.
	_MixThreadAudioTracks.Emplace();
	AudioTrack &mix_thread_audio_track{ _MixThreadAudioTracks.Back() };

	mix_thread_audio_track._Information = request._AddAudioTrackData._Information;
	mix_thread_audio_track._Identifier = request._AddAudioTrackData._Identifier;
}

/*
*	Processes a remove audio track request.
*/
void AudioSystem::ProcessRemoveAudioTrackRequest(const Request &request) NOEXCEPT
{
	//Remove the audio track.
	for (uint64 i{ 0 }; i < _MixThreadAudioTracks.Size(); ++i)
	{
		if (_MixThreadAudioTracks[i]._Identifier == request._RemoveAudioTrackData._Identifier)
		{
			_MixThreadAudioTracks.EraseAt<false>(i);

			break;
		}
	}
}

/*
*	Processes an add audio effect to track request.
*/
void AudioSystem::ProcessAddAudioEffectToTrackRequest(const Request &request) NOEXCEPT
{
	for (AudioTrack &audio_track : _MixThreadAudioTracks)
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
	new_playing_audio._Player.SetPlaybackRate(static_cast<float32>(request._PlayAudio2DData._Request._Asset->_AudioStream.GetSampleRate()) / _SampleRate.Load());
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
#define PROFILE_MIX_BUFFER (0)

#if PROFILE_MIX_BUFFER
	TimePoint time_point;
#endif

	//Set the sample rate/beats per minute on all active effects.
	{
		const float32 sample_rate{ _SampleRate.Load() };
		const float32 beats_per_minute{ _BeatsPerMinute.Load() };

		for (AudioTrack &audio_track : _MixThreadAudioTracks)
		{
			for (AudioEffect *const RESTRICT effect : audio_track._Effects)
			{
				effect->SetSampleRate(sample_rate);
				effect->SetBeatsPerMinute(beats_per_minute);
			}
		}
	}

	//Calculate the start input channel index.
	uint32 start_input_channel_index{ UINT32_MAXIMUM };

	for (const AudioTrack &audio_track : _MixThreadAudioTracks)
	{
		if (audio_track._Information._NumberOfInputChannels > 0)
		{
			start_input_channel_index = BaseMath::Minimum<uint32>(start_input_channel_index, audio_track._Information._StartChannelIndex);
		}
	}

	//Cache the mix buffer.
	MixBuffer &mix_buffer{ _MixBuffers[request._MixBufferData._BufferIndex] };

	//Cache the number of channels.
	const uint8 number_of_channels{ static_cast<uint8>(mix_buffer._Outputs.Size()) };

	//Cache the number of samples.
	const uint32 number_of_samples{ static_cast<uint32>(mix_buffer._Outputs[0].Size()) };

	//Start with all outputs at zero.
	for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
	{
		Memory::Set(mix_buffer._Outputs[channel_index].Data(), 0, sizeof(float32) * number_of_samples);
	}

	//Set up the audio process context.
	AudioProcessContext audio_process_context;

	audio_process_context._WasTimelineRunning = true;
	audio_process_context._IsTimelineRunning = true;

	//Mix all tracks into the output.
	for (uint64 track_index{ 1 }; track_index < _MixThreadAudioTracks.Size(); ++track_index)
	{
		//Cache the track.
		AudioTrack &track{ _MixThreadAudioTracks[track_index] };

		//Fill in the inputs.
		ASSERT(track._Information._StartChannelIndex != UINT32_MAXIMUM && track._Information._NumberOfInputChannels != UINT32_MAXIMUM, "Currently there's no reason for an audio track to exist without any input channels!");
	
		track._Samples.Resize<true>(mix_buffer._Outputs.Size());

		for (uint64 input_channel_index{ 0 }; input_channel_index < track._Samples.Size(); ++input_channel_index)
		{
			track._Samples[input_channel_index].Resize<false>(number_of_samples);

			uint32 _input_channel_index{ track._Information._StartChannelIndex + static_cast<uint32>(input_channel_index) };
			_input_channel_index = track._Information._StartChannelIndex + (_input_channel_index - track._Information._StartChannelIndex) % track._Information._NumberOfInputChannels;
			_input_channel_index -= start_input_channel_index;

			if (_input_channel_index < mix_buffer._Inputs.Size())
			{
				Memory::Copy(track._Samples[input_channel_index].Data(), mix_buffer._Inputs[_input_channel_index].Data(), sizeof(float32) * number_of_samples);
			}

			else
			{
				Memory::Set(track._Samples[input_channel_index].Data(), 0, sizeof(float32) * number_of_samples);
			}
		}

		//Apply the audio effects.
		for (AudioEffect *const RESTRICT effect : track._Effects)
		{
			effect->Process
			(
				audio_process_context,
				track._Samples,
				&track._Samples,
				number_of_channels,
				number_of_samples
			);
		}

		//Add to the output.
		for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
		{
			SIMD::Add(mix_buffer._Outputs[channel_index].Data(), track._Samples[channel_index].Data(), number_of_samples);
		}
	}

	//Add all playing audio 2D to the output.
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
			mix_buffer._Outputs[channel_index][sample_index] += sample;
		}

		//Advance all playing audio.
		for (PlayingAudio2D &playing_audio : _PlayingAudio2D)
		{
			playing_audio._Player.Advance();
		}
	}

	//Remove all non-active playing audio.
	for (uint64 i{ 0 }; i < _PlayingAudio2D.Size();)
	{
		if (_PlayingAudio2D[i]._Player.IsActive())
		{
			++i;
		}

		else
		{
			_PlayingAudio2D.EraseAt<false>(i);
		}
	}

	//Process the master track effects, if there are any.
	if (!_MixThreadAudioTracks[0]._Effects.Empty())
	{
		for (AudioEffect *const RESTRICT master_effect : _MixThreadAudioTracks[0]._Effects)
		{
			master_effect->Process
			(
				audio_process_context,
				mix_buffer._Outputs,
				&mix_buffer._Outputs,
				number_of_channels,
				number_of_samples
			);
		}
	}

	//The mix buffer is now mixed!
	mix_buffer._IsMixed.Set();

#if PROFILE_MIX_BUFFER
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

			const float64 deadline_duration{ static_cast<float64>(number_of_samples) / static_cast<float64>(_SampleRate.Load()) };

			LOG_INFORMATION("Mix buffer profiling: Max: %f ms - Average: %f ms / Deadline: %f ms", maximum_duration * 1'000.0, accumulated_duration * 1'000.0, deadline_duration * 1'000.0);
		}
	}
#endif

#undef PROFILE_MIX_BUFFER
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