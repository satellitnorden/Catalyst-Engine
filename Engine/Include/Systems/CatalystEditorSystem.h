#if defined(CATALYST_EDITOR)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

class CatalystEditorSystem final
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(CatalystEditorSystem);

	/*
	*	Default constructor.
	*/
	FORCE_INLINE CatalystEditorSystem() NOEXCEPT
	{

	}

	/*
	*	Initializes the Catalyst editor system.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Terminates the Catalyst editor system.
	*/
	void Terminate() NOEXCEPT;

private:

	/*
	*	Updates the Catalyst editor system during the PRE update phase.
	*/
	void PreUpdate() NOEXCEPT;

};
#endif