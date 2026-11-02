#if defined(AUDIO_PLUGIN_VST3)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Audio.
#include <Audio/Plugins/Adapters/VST3/VST3AudioProcessor.h>
#include <Audio/Plugins/Adapters/VST3/VST3EditController.h>
#include <Audio/Plugins/Adapters/VST3/VST3PluginData.h>

//Third party.
#include <VST3/pluginterfaces/base/ustring.h>
#include <VST3/pluginterfaces/vst/ivstcomponent.h>

class VST3Component final : public Steinberg::Vst::IComponent
{

public:

	/*
	*	Default constructor.
	*/
	FORCE_INLINE VST3Component() NOEXCEPT
	{

	}

	/*
	*	Default destructor.
	*/
	FORCE_INLINE ~VST3Component() NOEXCEPT
	{

	}

	/*
	*	Queries if this object has the given interface.
	*/
	FORCE_INLINE Steinberg::tresult PLUGIN_API queryInterface(const Steinberg::TUID _iid, void **obj) override
	{
		if (!obj)
		{
			return Steinberg::kInvalidArgument;
		}

		*obj = nullptr;

		QUERY_INTERFACE(_iid, obj, Steinberg::FUnknown_iid, VST3Component);
		QUERY_INTERFACE(_iid, obj, Steinberg::IPluginBase_iid, VST3Component);
		QUERY_INTERFACE(_iid, obj, Steinberg::Vst::IComponent_iid, VST3Component);

		if (Steinberg::FUnknownPrivate::iidEqual(_iid, Steinberg::Vst::IAudioProcessor_iid))
		{
			*obj = static_cast<VST3AudioProcessor*>(&_AudioProcessor);

			addRef();

			return Steinberg::kResultOk;
		}

		if (Steinberg::FUnknownPrivate::iidEqual(_iid, Steinberg::Vst::IEditController_iid))
		{
			*obj = static_cast<VST3EditController*>(&_EditController);

			addRef();

			return Steinberg::kResultOk;
		}

		return Steinberg::kNoInterface;
	}

	/*
	*	Adds a reference.
	*/
	FORCE_INLINE uint32 PLUGIN_API addRef() override
	{
		return Steinberg::FUnknownPrivate::atomicAdd(_NumberOfReferences, 1);
	}

	/*
	*	Releases a reference.
	*/
	FORCE_INLINE uint32 PLUGIN_API release() override
	{
		if (Steinberg::FUnknownPrivate::atomicAdd(_NumberOfReferences, -1) == 0)
		{
			delete this;
			return 0;
		}

		return _NumberOfReferences;
	}

	/*
	*	Initializes this component.
	*/
	FORCE_INLINE Steinberg::tresult PLUGIN_API initialize(FUnknown *context) override
	{
		//Nothing to do here yet. (:
		return Steinberg::kResultOk;
	}

	/*
	*	Terminates this component.
	*/
	FORCE_INLINE Steinberg::tresult PLUGIN_API terminate() override
	{
		//Nothing to do here yet. (:
		return Steinberg::kResultOk;
	}

	/*
	*	Returns the controll class ID.
	*/
	FORCE_INLINE Steinberg::tresult PLUGIN_API getControllerClassId(Steinberg::TUID classId) override
	{
		//Not yet implemented. (:
		return Steinberg::kNoInterface;
	}

	/*
	*	Sets the IO mode.
	*/
	FORCE_INLINE Steinberg::tresult PLUGIN_API setIoMode(Steinberg::Vst::IoMode mode) override
	{
		//Nothing to do here yet. (:
		return Steinberg::kResultOk;
	}

	/*
	*	Returns the bus count.
	*/
	FORCE_INLINE int32 PLUGIN_API getBusCount(Steinberg::Vst::MediaType type, Steinberg::Vst::BusDirection dir) override
	{
		switch (type)
		{
			case Steinberg::Vst::MediaTypes::kAudio:
			{
				switch (dir)
				{
					case Steinberg::Vst::BusDirections::kInput:
					{
						return 1;
					}

					case Steinberg::Vst::BusDirections::kOutput:
					{
						return 1;
					}

					default:
					{
						return 0;
					}
				}
			}

			case Steinberg::Vst::MediaTypes::kEvent:
			{
				switch (dir)
				{
					case Steinberg::Vst::BusDirections::kInput:
					{
						return 1;
					}

					default:
					{
						return 0;
					}
				}
			}

			default:
			{
				return 0;
			}
		}
	}

	/*
	*	Returns the bus index.
	*/
	FORCE_INLINE Steinberg::tresult PLUGIN_API getBusInfo
	(
		Steinberg::Vst::MediaType type,
		Steinberg::Vst::BusDirection dir,
		int32 index,
		Steinberg::Vst::BusInfo &bus
	) override
	{
		if (type != Steinberg::Vst::MediaTypes::kAudio && type != Steinberg::Vst::MediaTypes::kEvent)
		{
			return Steinberg::kInvalidArgument;
		}

		if (dir != Steinberg::Vst::BusDirections::kInput && dir != Steinberg::Vst::BusDirections::kOutput)
		{
			return Steinberg::kInvalidArgument;
		}

		if (index != 0)
		{
			return Steinberg::kInvalidArgument;
		}

		bus.mediaType = type;
		bus.direction = dir;
		bus.channelCount = type == Steinberg::Vst::MediaTypes::kAudio ? 2 : 16;
		
		if (type == Steinberg::Vst::MediaTypes::kAudio)
		{
			if (dir == Steinberg::Vst::BusDirections::kInput)
			{
				Steinberg::UString128("Main Input").copyTo(bus.name, 128);
			}

			else
			{
				Steinberg::UString128("Main Output").copyTo(bus.name, 128);
			}
		}
		
		else
		{
			Steinberg::UString128("Main Event Input").copyTo(bus.name, 128);
		}

		bus.busType = Steinberg::Vst::BusTypes::kMain;
		bus.flags = Steinberg::Vst::BusInfo::BusFlags::kDefaultActive;

		return Steinberg::kResultOk;
	}

	/*
	*	Returns the routing info.
	*/
	FORCE_INLINE Steinberg::tresult PLUGIN_API getRoutingInfo
	(
		Steinberg::Vst::RoutingInfo &inInfo,
		Steinberg::Vst::RoutingInfo &outInfo
	) override
	{
		inInfo.mediaType = Steinberg::Vst::MediaTypes::kAudio;
		inInfo.busIndex = 0;
		inInfo.channel = -1;

		outInfo.mediaType = Steinberg::Vst::MediaTypes::kAudio;
		outInfo.busIndex = 0;
		outInfo.channel = -1;

		return Steinberg::kResultOk;
	}

	/*
	*	Activates the bus.
	*/
	FORCE_INLINE Steinberg::tresult PLUGIN_API activateBus
	(
		Steinberg::Vst::MediaType type,
		Steinberg::Vst::BusDirection dir,
		int32 index,
		Steinberg::TBool state
	) override
	{
		//Nothing to do here yet. (:
		return Steinberg::kResultOk;
	}

	/*
	*	Sets whether or not this component is active.
	*/
	FORCE_INLINE Steinberg::tresult PLUGIN_API setActive(Steinberg::TBool state) override
	{
		//Nothing to do here yet. (:
		return Steinberg::kResultOk;
	}

	/*
	*	Sets the state of this component.
	*/
	FORCE_INLINE Steinberg::tresult PLUGIN_API setState(Steinberg::IBStream *state) override
	{
		//Nothing to do here yet. (:
		return Steinberg::kResultOk;
	}

	/*
	*	Returns the state of this component.
	*/
	FORCE_INLINE Steinberg::tresult PLUGIN_API getState(Steinberg::IBStream *state) override
	{
		//Nothing to do here yet. (:
		return Steinberg::kResultOk;
	}

private:

	//The plugin data.
	VST3PluginData _PluginData;

	//The audio processor.
	VST3AudioProcessor _AudioProcessor{ &_PluginData };

	//The edit controller.
	VST3EditController _EditController{ &_PluginData };

	//The number of references.
	Steinberg::int32 _NumberOfReferences{ 1 };

};
#endif