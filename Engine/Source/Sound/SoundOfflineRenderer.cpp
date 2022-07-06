//Header file.
#include <Sound/SoundOfflineRenderer.h>

//File.
#include <File/Core/FileCore.h>
#include <File/Writers/OGGWriter.h>
#include <File/Writers/WAVWriter.h>

/*
*	Initializes this sound offline renderer.
*/
void SoundOfflineRenderer::Initialize(const Parameters &parameters) NOEXCEPT
{
	//Set the output file path.
	_OutputFilePath = parameters._OutputFilePath;

	//Set the sample rate.
	_SampleRate = parameters._SampleRate;

	//Set the number of channels.
	_NumberOfChannels = parameters._NumberOfChannels;

	//Set the callback.
	_Callback = parameters._Callback;

	//Set the sound mix components.
	_SoundMixComponents = parameters._SoundMixComponents;

	//Set up the sound resource.
	_SoundResource._SampleRate = _SampleRate;
	_SoundResource._NumberOfChannels = _NumberOfChannels;
	_SoundResource._Samples.Upsize<true>(_NumberOfChannels);

	for (DynamicArray<int16> &samples : _SoundResource._Samples)
	{
		samples.Reserve(static_cast<uint64>(_SoundResource._SampleRate * parameters._RequestedLength));
	}
}

/*
*	Updates this sound offline renderer. Returns the current progress. It is done when it is >= 1.0f.
*/
NO_DISCARD float32 SoundOfflineRenderer::Update() NOEXCEPT
{
	//Call the callback.
	_Callback(this);

	//Write the sample.
	for (uint8 channel_index{ 0 }; channel_index < _NumberOfChannels; ++channel_index)
	{
		//Calculate the current sample.
		float32 current_sample{ 0.0f };

		//Add all playing sounds to the current sample.
		for (PlayingSound &playing_sound : _PlayingSounds)
		{
			current_sample += playing_sound._SoundResourcePlayer.NextSample(channel_index);
		}

		for (SoundMixComponent &component : _SoundMixComponents)
		{
			component.Process(&current_sample);
		}

		//Write the current value.
		_SoundResource._Samples[channel_index].Emplace(static_cast<int16>(current_sample * static_cast<float32>(INT16_MAXIMUM)));
	}

	//Advance all playing sounds.
	for (PlayingSound &playing_sound : _PlayingSounds)
	{
		playing_sound._SoundResourcePlayer.Advance();
	}

	//Remove any inactive sounds.
	for (uint64 i{ 0 }; i < _PlayingSounds.Size();)
	{
		if (!_PlayingSounds[i]._SoundResourcePlayer.IsActive())
		{
			if (_PlayingSounds[i]._SoundStoppedCallback)
			{
				_PlayingSounds[i]._SoundStoppedCallback(_PlayingSounds[i]._SoundInstanceHandle);
			}

			_PlayingSounds.EraseAt<false>(i);
		}

		else
		{
			++i;
		}
	}

	//Return the current progress.
	return static_cast<float32>(_SoundResource._Samples[0].Size()) / static_cast<float32>(_SoundResource._Samples[0].Capacity());
}

/*
*	Terminates this sound offline renderer.
*/
void SoundOfflineRenderer::Terminate() NOEXCEPT
{
	//Write the .wav file.
	switch (File::GetExtension(_OutputFilePath.Data()))
	{

		case File::Extension::OGG:
		{
			OGGWriter::Write(_OutputFilePath.Data(), _SoundResource);

			break;
		}

		case File::Extension::WAV:
		{
			WAVWriter::Write(_OutputFilePath.Data(), _SoundResource);

			break;
		}

		default:
		{
			ASSERT(false, "Unsupported file extension!");

			break;
		}
	}
}

/*
*	Plays a sound.
*/
void SoundOfflineRenderer::PlaySound(const PlaySoundRequest &request) NOEXCEPT
{
	//Calculate the sound instance handle.
	SoundInstanceHandle sound_instance_handle{ _SoundInstanceCounter++ };

	//Add the new playing sound.
	PlayingSound new_playing_sound;

	new_playing_sound._SoundResourcePlayer.SetSoundResource(request._SoundResource);
	new_playing_sound._SoundResourcePlayer.SetGain(request._Gain);
	new_playing_sound._SoundResourcePlayer.SetPan(request._Pan);
	new_playing_sound._SoundResourcePlayer.SetPlaybackSpeed(request._SoundResource->_SampleRate / _SampleRate * request._PlaybackRate);
	new_playing_sound._SoundResourcePlayer.SetIsLooping(request._IsLooping);
	new_playing_sound._SoundResourcePlayer.GetADSREnvelope().SetSampleRate(_SampleRate);
	new_playing_sound._SoundResourcePlayer.GetADSREnvelope().SetStageValues(request._AttackTime,
																			request._DecayTime,
																			request._SustainGain,
																			request._ReleaseTime);
	new_playing_sound._SoundResourcePlayer.GetADSREnvelope().EnterAttackStage();
	new_playing_sound._SoundResourcePlayer.SetCurrentSample(static_cast<int64>(request._StartTime * request._SoundResource->_SampleRate));
	new_playing_sound._SoundInstanceHandle = sound_instance_handle;
	new_playing_sound._SoundStoppedCallback = request._SoundStoppedCallback;

	_PlayingSounds.Emplace(new_playing_sound);

	//Set the sound instance handle on the request.
	if (request._SoundInstance)
	{
		*request._SoundInstance = sound_instance_handle;
	}
}

/*
*	Stops a sound.
*/
void SoundOfflineRenderer::StopSound(const SoundInstanceHandle handle) NOEXCEPT
{
	for (uint64 i{ 0 }, size{ _PlayingSounds.Size() }; i < size; ++i)
	{
		if (_PlayingSounds[i]._SoundInstanceHandle == handle)
		{
			_PlayingSounds[i]._SoundResourcePlayer.Stop();

			break;
		}
	}
}