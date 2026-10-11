#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Audio.
#include <Audio/VST3/ReferenceCounting.h>

//Third party.
#include <VST3/pluginterfaces/vst/ivstevents.h>

namespace VST3
{

	/*
	*	Implementation of VST3 bit streams.
	*/
	class EventList final : public Steinberg::Vst::IEventList
	{

	public:

		VST3_REFERENCE_COUNTING();

		/*
		*	Queries an instance.
		*/
		FORCE_INLINE Steinberg::tresult queryInterface(const Steinberg::TUID _iid, void **obj) override
		{
			ASSERT(false, "Check if this ever happens!");
			return Steinberg::kNoInterface;
		}

		/*
		*	Returns the event count.
		*/
		FORCE_INLINE int32 getEventCount() override
		{
			return static_cast<int32>(_Events.Size());
		}

		/*
		*	Returns the event at the given index.
		*/
		FORCE_INLINE Steinberg::tresult PLUGIN_API getEvent(int32 index, Steinberg::Vst::Event &event) override
		{
			event = _Events[index];
			return Steinberg::kResultOk;
		}

		/*
		*	Adds a new event.
		*/
		FORCE_INLINE Steinberg::tresult PLUGIN_API addEvent(Steinberg::Vst::Event &event) override
		{
			_Events.Emplace(event);
			return Steinberg::kResultOk;
		}

		/*
		*	Clears this event list.
		*/
		FORCE_INLINE void Clear() NOEXCEPT
		{
			_Events.Clear();
		}

	private:

		//The events.
		DynamicArray<Steinberg::Vst::Event> _Events;

	};

}