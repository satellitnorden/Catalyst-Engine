#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Utilities/StringUtilities.h>

//Third party.
#include <uuid/uuid.h>
#include <VST3/pluginterfaces/base/funknown.h>

namespace VST3UUID
{

	/*
	*	Generates a UUID from the given string.
	*/
	FORCE_INLINE void Generate(const char *const RESTRICT name, Steinberg::TUID *const RESTRICT UUID) NOEXCEPT
	{
		//This defines the namespace - It was generated once with UUID v4, and should stay constant _forever_.
		constexpr uuid_t NAMESPACE
		{
			68,		221,	94,		127,
			244,	144,	72,		71,
			163,	29,		228,	215,
			57,		4,		31,		233
		};

		//Generate the UUID.
		uuid_t _UUID;

		uuid_v5(&NAMESPACE, name, StringUtilities::StringLength(name), &_UUID);

		//Copy it over to the class ID.
		Memory::Copy(*UUID, _UUID.bytes, sizeof(Steinberg::TUID));
	}

}