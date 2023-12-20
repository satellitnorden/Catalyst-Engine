#if !defined(CATALYST_CONFIGURATION_FINAL)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/General/DynamicString.h>

//Type aliases.
using DebugCommandFunction = void(*)(class DebugCommand *const RESTRICT debug_command, void *const RESTRICT user_data);

class DebugCommand final
{

public:

	//The name.
	DynamicString _Name;

	//The function.
	DebugCommandFunction _Function;

	//The user data.
	void *RESTRICT _UserData;

};

class ALIGN(8) DebugSystem final
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(DebugSystem);

	/*
	*	Default constructor.
	*/
	FORCE_INLINE DebugSystem() NOEXCEPT
	{

	}

	/*
	*	Initializes the debug system.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Terminates the debug system.
	*/
	void Terminate() NOEXCEPT;

	/*
	*	Registers a debug command.
	*/
	void RegisterDebugCommand
	(
		const char *const RESTRICT name,
		DebugCommandFunction function,
		void *const RESTRICT user_data
	) NOEXCEPT;

private:

	//Denotes if the debug window should be displayed.
	bool _DisplayDebugWindow{ false };

	//The debug commands.
	DynamicArray<DebugCommand> _DebugCommands;

	/*
	*	Updates the debug system.
	*/
	void Update() NOEXCEPT;

	/*
	*	Updates IO.
	*/
	void UpdateIO() NOEXCEPT;

};
#endif
