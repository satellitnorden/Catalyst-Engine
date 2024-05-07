#if !defined(CATALYST_CONFIGURATION_FINAL)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/General/DynamicString.h>

//Math.
#include <Math/General/Vector.h>

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

		struct
		{
			//The minimum.
			float32 _Minimum;

			//The maximum.
			float32 _Maximum;

			//The value.
			float32 _Value;
		} _SliderState;
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
		CHECKBOX,
		SLIDER
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

	/*
	*	Registers a slider debug command.
	*/
	void RegisterSliderDebugCommand
	(
		const char *const RESTRICT name,
		DebugCommandFunction function,
		void *const RESTRICT user_data,
		const float32 minimum,
		const float32 maximum,
		const float32 initial_value
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
	NO_DISCARD bool DrawDebugWindow(const Vector2<float32> minimum, const Vector2<float32> maximum) NOEXCEPT;

	/*
	*	Draws a debug category.
	*/
	void DrawDebugCategory(DebugCategory &debug_category, const bool is_root) NOEXCEPT;

};
#endif
