#if !defined(CATALYST_CONFIGURATION_FINAL)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/General/DynamicString.h>

//Systems.
#include <Systems/System.h>

//Type aliases.
using DebugCommandFunction = void(*)(class DebugCommand *const RESTRICT command, void *const RESTRICT user_data);

class DebugCommandState final
{

public:

	union
	{
		struct
		{
			//Denotes whether or not this checkbox is checked.
			bool _IsChecked;
		} _CheckboxState;
	};

	/*
	*	Default constructor.
	*/
	FORCE_INLINE DebugCommandState() NOEXCEPT
	{
		Memory::Set(this, 0, sizeof(DebugCommandState));
	}

};

class DebugCommand final
{

public:

	//Enumeration covering all types.
	enum class Type : uint8
	{
		BUTTON,
		CHECKBOX
	};

	//The full name.
	DynamicString _FullName;

	//The name.
	DynamicString _Name;

	//The type.
	Type _Type;

	//The state.
	DebugCommandState _State;

	//The function.
	DebugCommandFunction _Function;

	//The user data.
	void *RESTRICT _UserData;

};

class DebugSystem final
{

public:

	//System declaration.
	CATALYST_SYSTEM
	(
		DebugSystem,
		NOT_DEFINED_REQUIREMENT(CATALYST_CONFIGURATION_FINAL)
	);

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
	*	Registers a button debug command.
	*/
	void RegisterButtonDebugCommand
	(
		const char *const RESTRICT name,
		DebugCommandFunction function,
		void *const RESTRICT user_data
	) NOEXCEPT;

	/*
	*	Registers a checkbox debug command.
	*/
	void RegisterCheckboxDebugCommand
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
	*	Draws the debug window.
	*/
	void DrawDebugWindow() NOEXCEPT;

	/*
	*	Draws a debug category.
	*/
	void DrawDebugCategory(DebugCategory &debug_category, const bool is_root) NOEXCEPT;

};
#endif
