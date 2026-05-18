#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Utilities/StringUtilities.h>

//Audio.
#include <Audio/VST3/ReferenceCounting.h>

//Third party.
#include <VST3/pluginterfaces/vst/ivsthostapplication.h>

namespace VST3
{

	class HostApplication final : public Steinberg::Vst::IHostApplication
	{

	public:

		VST3_REFERENCE_COUNTING();

		/*
		*	Queries an instance.
		*/
		FORCE_INLINE Steinberg::tresult queryInterface(const Steinberg::TUID _iid, void **obj) override
		{
			//Check that we have valid arguments.
			if (!obj)
			{
				return Steinberg::kInvalidArgument;
			}

			//Reset the object.
			*obj = nullptr;

			//Are they asking for an unknown?
			if (Steinberg::FUnknownPrivate::iidEqual(_iid, Steinberg::FUnknown_iid))
			{
				*obj = static_cast<Steinberg::FUnknown*>(this);
			}

			//Are they asking for a host application?
			else if (Steinberg::FUnknownPrivate::iidEqual(_iid, Steinberg::Vst::IHostApplication_iid))
			{
				*obj = static_cast<Steinberg::Vst::IHostApplication*>(this);
			}

			//Add a reference if we set the object.
			if (*obj)
			{
				addRef();
				return Steinberg::kResultOk;
			}

			//Otherwise, return that we don't have this interface.
			else
			{
				return Steinberg::kNoInterface;
			}
		}

		/*
		*	Returns the name.
		*/
		FORCE_INLINE Steinberg::tresult getName(Steinberg::Vst::String128 name) override
		{
			constexpr const char *const RESTRICT NAME{ "Catalyst Engine" };
			constexpr uint64 NAME_LENGTH{ StringUtilities::StringLength(NAME) };

			for (uint64 i{ 0 }; i < NAME_LENGTH; ++i)
			{
				name[i] = static_cast<Steinberg::Vst::TChar>(NAME[i]);
			}

			name[NAME_LENGTH] = static_cast<Steinberg::Vst::TChar>('\0');

			return Steinberg::kResultOk;
		}

		/*
		*	Creates an instance.
		*/
		FORCE_INLINE Steinberg::tresult createInstance(Steinberg::TUID cid, Steinberg::TUID _iid, void **obj) override
		{
			if (!obj)
			{
				return Steinberg::kInvalidArgument;
			}

			*obj = nullptr;

			return Steinberg::kNoInterface;
		}

	};

}