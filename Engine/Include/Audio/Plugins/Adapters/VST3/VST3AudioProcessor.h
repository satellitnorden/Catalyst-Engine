#if defined(AUDIO_PLUGIN_VST3)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Audio.
#include <Audio/Plugins/Adapters/VST3/VST3PluginData.h>

//Third party.
#include <VST3/pluginterfaces/vst/ivstaudioprocessor.h>
#include <VST3/pluginterfaces/vst/ivstevents.h>
#include <VST3/pluginterfaces/vst/ivstprocesscontext.h>

class VST3AudioProcessor final : public Steinberg::Vst::IAudioProcessor
{

public:

	/*
	*	Default constructor.
	*/
	FORCE_INLINE VST3AudioProcessor(VST3PluginData *const RESTRICT plugin_data) NOEXCEPT
		:
		_PluginData(plugin_data)
	{

	}

	/*
	*	Default destructor.
	*/
	FORCE_INLINE ~VST3AudioProcessor() NOEXCEPT
	{

	}

	/*
	*	Queries if this object has the given interface.
	*/
	FORCE_INLINE Steinberg::tresult PLUGIN_API queryInterface(const Steinberg::TUID _iid, void** obj) override
	{
		if (!obj)
		{
			return Steinberg::kInvalidArgument;
		}

		*obj = nullptr;

		QUERY_INTERFACE(_iid, obj, Steinberg::FUnknown_iid, VST3AudioProcessor);
		QUERY_INTERFACE(_iid, obj, Steinberg::Vst::IAudioProcessor_iid, VST3AudioProcessor);

		return Steinberg::kNoInterface;
	}

	/*
	*	Adds a reference.
	*/
	FORCE_INLINE uint32 PLUGIN_API addRef() override
	{
		return 1;
	}

	/*
	*	Releases a reference.
	*/
	FORCE_INLINE uint32 PLUGIN_API release() override
	{
		return 1;
	}

	/*
	*	Sets the bus arrangements.
	*/
	FORCE_INLINE Steinberg::tresult PLUGIN_API setBusArrangements
	(
		Steinberg::Vst::SpeakerArrangement *inputs,
		int32 numIns,
		Steinberg::Vst::SpeakerArrangement *outputs,
		int32 numOuts
	) override
	{
		//Nothing to do here. (:
		return Steinberg::kResultOk;
	}

	/*
	*	Returns the bus arrangements.
	*/
	FORCE_INLINE Steinberg::tresult PLUGIN_API getBusArrangement
	(
		Steinberg::Vst::BusDirection dir,
		int32 index,
		Steinberg::Vst::SpeakerArrangement &arr) override
	{
		//Always stereo, for now.
		arr = Steinberg::Vst::SpeakerArr::kStereo;

		//Return success.
		return Steinberg::kResultOk;
	}

	/*
	*	Returns if the audio processor can process the given sample size.
	*/
	FORCE_INLINE Steinberg::tresult PLUGIN_API canProcessSampleSize(int32 symbolicSampleSize) override
	{
		//Only do 32-bit for now.
		if (symbolicSampleSize == Steinberg::Vst::SymbolicSampleSizes::kSample32)
		{
			return Steinberg::kResultTrue;
		}

		else
		{
			return Steinberg::kResultFalse;
		}
	}

	/*
	*	Returns the latency samples.
	*/
	FORCE_INLINE uint32 PLUGIN_API getLatencySamples() override
	{
		return 0;
	}

	/*
	*	Sets up the processing.
	*/
	FORCE_INLINE Steinberg::tresult PLUGIN_API setupProcessing(Steinberg::Vst::ProcessSetup &setup) override
	{
		//Set the sample rate on the plugin.
		_PluginData->_Plugin->SetSampleRate(static_cast<float32>(setup.sampleRate));

		//Return success!
		return Steinberg::kResultOk;
	}

	/*
	*	Sets whether or not this audio processor is processing.
	*/
	FORCE_INLINE Steinberg::tresult PLUGIN_API setProcessing(Steinberg::TBool state) override
	{
		//Return success!
		return Steinberg::kResultOk;
	}

	/*
	*	Processes.
	*/
	FORCE_INLINE Steinberg::tresult PLUGIN_API process(Steinberg::Vst::ProcessData &data) override
	{
		//Set the beats per minute on the plugin.
		if (data.processContext && data.processContext->tempo > 0.0)
		{
			_PluginData->_Plugin->SetBeatsPerMinute(static_cast<float32>(data.processContext->tempo));
		}

		//Set up the context.
		AudioProcessContext context;

		context._WasTimelineRunning = true; //TODO!
		context._IsTimelineRunning = true; //TODO!

		//Set up the inputs/outputs.
		if (data.inputs)
		{
			_PluginData->_Inputs.Resize<true>(data.inputs->numChannels);

			for (int32 channel_index{ 0 }; channel_index < data.inputs->numChannels; ++channel_index)
			{
				_PluginData->_Inputs.At(channel_index).Resize<false>(data.numSamples);
				Memory::Copy(_PluginData->_Inputs.At(channel_index).Data(), data.inputs->channelBuffers32[channel_index], data.numSamples * sizeof(float32));
			}
		}

		if (data.outputs)
		{
			_PluginData->_Outputs.Resize<true>(data.outputs->numChannels);

			for (int32 channel_index{ 0 }; channel_index < data.outputs->numChannels; ++channel_index)
			{
				_PluginData->_Outputs.At(channel_index).Resize<false>(data.numSamples);
				Memory::Copy(_PluginData->_Outputs.At(channel_index).Data(), data.outputs->channelBuffers32[channel_index], data.numSamples * sizeof(float32));
			}
		}

		//Cache the number of channels.
		uint8 number_of_channels{ 0 };

		if (data.inputs)
		{
			number_of_channels = static_cast<uint8>(data.inputs->numChannels);
		}

		else if (data.outputs)
		{
			number_of_channels = static_cast<uint8>(data.outputs->numChannels);
		}

		//Cache the number of samples.
		const uint32 number_of_samples{ static_cast<uint32>(data.numSamples) };

		//Process input events.
		if (data.inputEvents)
		{
			const uint32 number_of_events{ static_cast<uint32>(data.inputEvents->getEventCount()) };

			for (uint32 event_index{ 0 }; event_index < number_of_events; ++event_index)
			{
				Steinberg::Vst::Event event;
				const Steinberg::tresult result{ data.inputEvents->getEvent(event_index, event) };

				if (result != Steinberg::kResultOk)
				{
					continue;
				}

				switch (event.type)
				{
					case Steinberg::Vst::Event::EventTypes::kNoteOnEvent:
					{
						//Construct the MIDI message.
						MIDIMessage message;

						message._Type = MIDIMessage::Type::NOTE_ON;
						message._NoteOnChannel = static_cast<uint8>(event.noteOn.channel);
						message._NoteOnNote = static_cast<uint8>(event.noteOn.pitch);
						message._NoteOnVelocity = static_cast<uint8>(event.noteOn.velocity * 127.0f);

						//Add the message!
						const uint32 sample_offset{ static_cast<uint32>(event.sampleOffset) };
						_PluginData->_Plugin->AddMIDIMessage(message, &sample_offset);

						break;
					}

					case Steinberg::Vst::Event::EventTypes::kNoteOffEvent:
					{
						//Construct the MIDI message.
						MIDIMessage message;

						message._Type = MIDIMessage::Type::NOTE_OFF;
						message._NoteOffChannel = static_cast<uint8>(event.noteOff.channel);
						message._NoteOffNote = static_cast<uint8>(event.noteOff.pitch);

						//Add the message!
						const uint32 sample_offset{ static_cast<uint32>(event.sampleOffset) };
						_PluginData->_Plugin->AddMIDIMessage(message, &sample_offset);

						break;
					}
				}
			}
		}

		//Process!
		_PluginData->_Plugin->CallProcess(context, _PluginData->_Inputs, &_PluginData->_Outputs, number_of_channels, number_of_samples);

		//Copy the outputs.
		if (data.outputs)
		{
			for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
			{
				Memory::Copy(data.outputs->channelBuffers32[channel_index], _PluginData->_Outputs.At(channel_index).Data(), number_of_samples * sizeof(float32));
			}
		}

		//Return success!
		return Steinberg::kResultOk;
	}

	/*
	*	Returns the tail samples.
	*/
	FORCE_INLINE uint32 PLUGIN_API getTailSamples() override
	{
		return 0;
	}

private:

	//The plugin data.
	VST3PluginData *RESTRICT _PluginData;

};
#endif