//Header file.
#include <Systems/SoundSystem.h>

//File.
#include <File/Writers/WAVWriter.h>

//Profiling.
#include <Profiling/Profiling.h>

//Sound.
#include <Sound/SoundAssetPlayer.h>
#include <Sound/SoundSubSystemASIO.h>
#include <Sound/SoundSubSystemWASAPI.h>

//Systems.
#include <Systems/CatalystEngineSystem.h>

/*
*	Queued play sound request class definition.
*/
class QueuedPlaySoundRequest final
{

public:

	//The sound asset.
	AssetPointer<SoundAsset> _SoundAsset;

	//The gain.
	float32 _Gain;

	//The pan.
	float32 _Pan;

	//The start time.
	float32 _StartTime;

	//Denotes the playback rate.
	float32 _PlaybackRate;

	//Denotes if the sound is looping.
	bool _IsLooping;

	//The attack time.
	float32 _AttackTime;

	//The decay time.
	float32 _DecayTime;

	//The sustain gain.
	float32 _SustainGain;

	//The release time.
	float32 _ReleaseTime;

	//The sound instance handle.
	SoundInstanceHandle _SoundInstanceHandle;

	//The sound started callback.
	SoundStartedCallback _SoundStartedCallback;

	//The sound stopped callback.
	SoundStoppedCallback _SoundStoppedCallback;

	//The audio time tracker.
	Atomic<float64> *RESTRICT _AudioTimeTracker{ nullptr };

};

/*
*	Queued stop sound request class definition.
*/
class QueuedStopSoundRequest final
{

public:

	//The sound instance handle.
	SoundInstanceHandle _SoundInstanceHandle;

};

/*
*	Playing sound class definition.
*/
class PlayingSound final
{

public:

	//The sound asset players.
	SoundAssetPlayer _SoundAssetPlayer;

	//The sound instance handle.
	SoundInstanceHandle _SoundInstanceHandle;

	//The sound stopped callback.
	SoundStoppedCallback _SoundStoppedCallback;

	//The audio time tracker.
	Atomic<float64> *RESTRICT _AudioTimeTracker{ nullptr };

};

//Sound system constants.
namespace SoundSystemConstants
{

	//The maximum number of queued master channel mix components.
	constexpr uint32 MAXIMUM_NUMBER_OF_QUEUED_MASTER_CHANNEL_MIX_COMPONENTS{ 8 };

	//The maximum number of queued play sounds.
	constexpr uint32 MAXIMUM_NUMBER_OF_QUEUED_PLAY_SOUNDS{ 128 };

	//The maximum number of queued stop sounds.
	constexpr uint32 MAXIMUM_NUMBER_OF_QUEUED_STOP_SOUNDS{ 128 };

	//The maximum number of playing sounds.
	constexpr uint64 MAXIMUM_NUMBER_OF_PLAYING_SOUNDS{ 1'024 };

}

//Sound system data.
namespace SoundSystemData
{

	//The queued add master channel mix components.
	AtomicQueue<SoundMixComponent, SoundSystemConstants::MAXIMUM_NUMBER_OF_QUEUED_MASTER_CHANNEL_MIX_COMPONENTS, AtomicQueueMode::MULTIPLE, AtomicQueueMode::SINGLE> _QueuedAddMasterChannelSoundMixComponents;

	//The queued remove master channel mix components.
	AtomicQueue<uint64, SoundSystemConstants::MAXIMUM_NUMBER_OF_QUEUED_MASTER_CHANNEL_MIX_COMPONENTS, AtomicQueueMode::MULTIPLE, AtomicQueueMode::SINGLE> _QueuedRemoveMasterChannelSoundMixComponents;

	//The queued play sound requests.
	AtomicQueue<QueuedPlaySoundRequest, SoundSystemConstants::MAXIMUM_NUMBER_OF_QUEUED_PLAY_SOUNDS, AtomicQueueMode::MULTIPLE, AtomicQueueMode::SINGLE> _QueuedPlaySoundRequests;

	//The queued stop sounds.
	AtomicQueue<QueuedStopSoundRequest, SoundSystemConstants::MAXIMUM_NUMBER_OF_QUEUED_STOP_SOUNDS, AtomicQueueMode::MULTIPLE, AtomicQueueMode::SINGLE> _QueuedStopSoundRequests;

	//The playing sounds.
	DynamicArray<PlayingSound> _PlayingSounds;

}

/*
*	Returns the audio latency.
*	That is, the time between a sound is requested to be played until it is heard.
*	This gives an estimate, and might be a bit unreliable on certain platforms.
*	The returned value is in milliseconds.
*/
uint32 SoundSystem::GetAudioLatency() const NOEXCEPT
{
	//Construct the final latency.
	float32 final_latency{ 0.0f };

	//Add the sub system latency.
	final_latency += _SubSystem->GetAudioLatency();

	//Add the buffer size delay.
	final_latency += static_cast<float32>(_SubSystem->GetBufferSize());

	//Lastly, add the time it takes for the sound system to process sounds.
	final_latency += (static_cast<float32>(DEFAULT_NUMBER_OF_SAMPLES_PER_MIXING_BUFFER) * static_cast<float32>(DEFAULT_NUMBER_OF_MIXING_BUFFERS));

	//Return the final latency.
	return BaseMath::Round<uint32>(final_latency);
}