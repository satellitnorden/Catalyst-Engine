#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/StaticString.h>

//Third party.
#include <VST3/pluginterfaces/base/funknown.h>
#include <VST3/pluginterfaces/vst/vsttypes.h>

namespace VST3
{

	namespace Utilities
	{

		/*
		*	Converts a Steinberg::Vst::String128 to a StaticString<128>.
		*/
		FORCE_INLINE void ConvertString128(const Steinberg::Vst::String128 &input_string, StaticString<128> *const RESTRICT output_string) NOEXCEPT
		{
			uint32 current_index{ 0 };

			while (input_string[current_index] != '\0')
			{
				(*output_string)[current_index] = static_cast<char>(input_string[current_index]);
				++current_index;
			}

			(*output_string)[current_index] = '\0';
		}

		/*
		*	Returns the string for the given result.
		*/
		FORCE_INLINE constexpr NO_DISCARD const char *const RESTRICT ResultString(const Steinberg::tresult result) NOEXCEPT
		{
			switch (result)
			{
				case Steinberg::kNoInterface:
				{
					return "kNoInterface";
				}

				case Steinberg::kResultOk:
				{
					return "kResultOk";
				}

				case Steinberg::kResultFalse:
				{
					return "kResultFalse";
				}

				case Steinberg::kInvalidArgument:
				{
					return "kInvalidArgument";
				}

				case Steinberg::kNotImplemented:
				{
					return "kNotImplemented";
				}

				case Steinberg::kInternalError:
				{
					return "kInternalError";
				}

				case Steinberg::kNotInitialized:
				{
					return "kNotInitialized";
				}

				case Steinberg::kOutOfMemory:
				{
					return "kOutOfMemory";
				}

				default:
				{
					ASSERT(false, "Invalid case!");

					return "UNKNOWN";
				}
			}
		}

	}

}