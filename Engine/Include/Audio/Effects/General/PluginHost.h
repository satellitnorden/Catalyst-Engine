#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/StaticString.h>

//Audio.
#include <Audio/Effects/Core/AudioEffect.h>

/*
*	Plugin host base class, effects specializing in host specific plugin formats inherits from this.
*/
class PluginHost : public AudioEffect
{

public:

	/*
	*	Parameter class definition.
	*/
	class Parameter final
	{

	public:

		//The name.
		StaticString<128> _Name;

	};

	/*
	*	Default destructor.
	*/
	FORCE_INLINE virtual ~PluginHost() NOEXCEPT
	{

	}

protected:

	//The parameters.
	DynamicArray<Parameter> _Parameters;

};