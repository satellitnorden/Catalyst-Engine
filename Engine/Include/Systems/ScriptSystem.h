#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

class ALIGN(8) ScriptSystem final
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(ScriptSystem);

	/*
	*	Default constructor.
	*/
	FORCE_INLINE ScriptSystem() NOEXCEPT
	{

	}

	/*
	*	Initializes the script system.
	*/
	void Initialize() NOEXCEPT;

private:

	/*
	*	Updates the script system during the LOGIC update phase.
	*/
	void LogicUpdate() NOEXCEPT;

};