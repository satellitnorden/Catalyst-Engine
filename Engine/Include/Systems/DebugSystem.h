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
	*	Post-terminates the debug system.
	*/
	void PostTerminate() NOEXCEPT;

	/*
	*	Registers a debug window.
	*/
	void RegisterDebugWindow
	(
		const char *const RESTRICT name,
		DebugCommandFunction function,
		void* const RESTRICT user_data
	) NOEXCEPT;

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

	/*
	*	Debug category class definition.
	*/
	class DebugCategory final
	{

	public:

		//The name.
		DynamicString _Name;

		//The sub categories.
		DynamicArray<DebugCategory> _SubCategories;

		//The debug commands.
		DynamicArray<DebugCommand> _DebugCommands;

	};

	//Denotes if the debug window should be displayed.
	bool _DisplayDebugWindow{ false };

	//The root debug category.
	DebugCategory _RootDebugCategory;

	/*
	*	Updates the debug system.
	*/
	void Update() NOEXCEPT;

	/*
	*	Updates IO.
	*/
	void UpdateIO() NOEXCEPT;

	/*
	*	Draws the debug window.
	*/
	void DrawDebugWindow() NOEXCEPT;

	/*
	*	Draws a debug category.
	*/
	void DrawDebugCategory(DebugCategory &debug_category, const bool is_root) NOEXCEPT;

};
#endif
