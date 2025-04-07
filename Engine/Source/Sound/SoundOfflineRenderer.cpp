//Header file.
#include <Sound/SoundOfflineRenderer.h>

//File.
#include <File/Core/File.h>
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

	//Set up the sound asset.
	_SoundAsset._SampleRate = _SampleRate;
	_SoundAsset._NumberOfChannels = _NumberOfChannels;
	_SoundAsset._Samples.Upsize<true>(_NumberOfChannels);

	for (DynamicArray<int16> &samples : _SoundAsset._Samples)
	{
		samples.Reserve(static_cast<uint64>(_SoundAsset._SampleRate * parameters._RequestedLength));
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
			current_sample += playing_sound._SoundAssetPlayer.NextSample(channel_index);
			playing_sound._SoundAssetPlayer.Advance(channel_index);
		}

		for (SoundMixComponent &component : _SoundMixComponents)
		{
			component.Process(&current_sample);
		}

		//Write the current value.
		_SoundAsset._Samples[channel_index].Emplace(static_cast<int16>(current_sample * static_cast<float32>(INT16_MAXIMUM)));
	}

	//Remove any inactive sounds.
	for (uint64 i{ 0 }; i < _PlayingSounds.Size();)
	{
		if (!_PlayingSounds[i]._SoundAssetPlayer.IsActive())
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
	return static_cast<float32>(_SoundAsset._Samples[0].Size()) / static_cast<float32>(_SoundAsset._Samples[0].Capacity());
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
			OGGWriter::Write(_OutputFilePath.Data(), _SoundAsset);

			break;
		}

		case File::Extension::WAV:
		{
			WAVWriter::Write(_OutputFilePath.Data(), _SoundAsset);

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
void SoundOfflineRenderer::PlaySound(const PlaySound2DRequest &request) NOEXCEPT
{
	//Calculate the sound instance handle.
	SoundInstanceHandle sound_instance_handle{ _SoundInstanceCounter++ };

	//Add the new playing sound.
	PlayingSound new_playing_sound;

	new_playing_sound._SoundAssetPlayer.SetSoundAsset(request._SoundAsset);
	new_playing_sound._SoundAssetPlayer.SetGain(request._Gain);
	new_playing_sound._SoundAssetPlayer.SetPan(request._Pan);
	new_playing_sound._SoundAssetPlayer.SetPlaybackSpeed(request._SoundAsset->_SampleRate / _SampleRate * request._PlaybackRate);
	new_playing_sound._SoundAssetPlayer.SetIsLooping(request._IsLooping);

	for (uint8 channel_index{ 0 }; channel_index < _NumberOfChannels; ++channel_index)
	{
		new_playing_sound._SoundAssetPlayer.GetADSREnvelope(channel_index).SetSampleRate(_SampleRate);
		new_playing_sound._SoundAssetPlayer.GetADSREnvelope(channel_index).SetStageValues(	request._AttackTime,
																								request._DecayTime,
																								request._SustainGain,
																								request._ReleaseTime);
		new_playing_sound._SoundAssetPlayer.GetADSREnvelope(channel_index).EnterAttackStage();
	}
	
	new_playing_sound._SoundAssetPlayer.SetCurrentSample(static_cast<int64>(request._StartTime * request._SoundAsset->_SampleRate));
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
			_PlayingSounds[i]._SoundAssetPlayer.Stop();

			break;
		}
	}
}