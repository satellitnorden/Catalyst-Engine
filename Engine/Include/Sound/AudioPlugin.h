#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/DynamicString.h>

/*
*	This is a base class for audio plugins, meant to represent some generic processing unit that can be reused in different contexts.
*/
class AudioPlugin
{

public:

	/*
	*	Parameters class definition.
	*/
	class Parameters final
	{

	public:

		//The directory that this plugin operates in.
		DynamicString _Directory;

	};

	/*
	*	Default destructor.
	*/
	FORCE_INLINE virtual ~AudioPlugin() NOEXCEPT
	{

	}

	/*
	*	Initializes this audio plugin.
	*/
	FORCE_INLINE virtual void Initialize(const Parameters &parameters) NOEXCEPT
	{
		//Store the parameters.
		_Parameters = parameters;
	}

protected:

	//The parameters.
	Parameters _Parameters;

};