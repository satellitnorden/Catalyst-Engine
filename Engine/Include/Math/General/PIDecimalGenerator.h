#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Systems.
#include <Systems/ResourceSystem.h>

class PIDecimalGenerator final
{

public:

	/*
	*	Get the next decimal.
	*/
	FORCE_INLINE NO_DISCARD uint8 NextDecimal() NOEXCEPT
	{
		//Cache the PI decimal lookup resource, if it hasn't yet.
		if (!_PIDecimalLookupResource)
		{
			_PIDecimalLookupResource = ResourceSystem::Instance->GetRawDataResource(HashString("PI_Decimal_Lookup"));
		}

		//Something went wrong, just return zero.
		if (!_PIDecimalLookupResource)
		{
			return 0;
		}

		//Has the current decimal index exceeded the lookup?
		if (_CurrentDecimalIndex >= _PIDecimalLookupResource->_Data.LastIndex())
		{
			return 0;
		}

		//Lookup!
		return _PIDecimalLookupResource->_Data[_CurrentDecimalIndex++];
	}

private:

	//The PI decimal lookup resources.
	ResourcePointer<RawDataResource> _PIDecimalLookupResource;

	//The current decimal index.
	int32 _CurrentDecimalIndex{ 0 };

};