#if !defined(CATALYST_CONFIGURATION_FINAL)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Math.
#include <Math/General/Vector.h>

//Systems.
#include <Systems/System.h>

//Type aliases.
using WindowCallback = bool(*)(const Vector2<float32> minimum, const Vector2<float32> maximum);

class ImGuiSystem final
{

public:

	//Enumeration covering all editor windows.
	enum class EditorWindow : uint8
	{
		VIEWPORT,
		TOP_BAR,
		TOP_RIGHT,
		BOTTOM_RIGHT,
		FLOATING,

		NUMBER_OF_EDITOR_WINDOWS
	};

	//Enumeration covering all game windows.
	enum class GameWindow : uint8
	{
		LEFT,
		RIGHT,

		NUMBER_OF_GAME_WINDOWS
	};

	/*
	*	Begin window parameters class definition.
	*/
	class BeginWindowParameters final
	{

	public:

		//The name.
		const char *RESTRICT _Name{ "[WINDOW_NAME]" };

		//The minimum.
		Vector2<float32> _Minimum{ 0.0f, 0.0f };

		//The maximum.
		Vector2<float32> _Maximum{ 1.0f, 1.0f };

		//Denotes whether or not to show title bar.
		bool _ShowTitleBar{ true };

		//Denotes whether or not to enable resize.
		bool _EnableResize{ false };

		//Denotes whether or not to enable move.
		bool _EnableMove{ false };

		//Denotes whether or not to enable menu bar.
		bool _EnableMenuBar{ false };

		//Denotes whether or not this window is closable.
		bool _Closable{ false };

	};

	//System declaration.
	CATALYST_SYSTEM
	(
		ImGuiSystem,
		SYSTEM_NOT_DEFINED_REQUIREMENT(CATALYST_CONFIGURATION_FINAL)
		SYSTEM_INITIALIZE()
		SYSTEM_UPDATE(RANGE(USER_INTERFACE, PRE_RENDER))
		SYSTEM_POST_TERMINATE()
	);

	/*
	*	Default constructor.
	*/
	ImGuiSystem() NOEXCEPT
	{

	}

	/*
	*	Callback for when input is available.
	*/
	void OnInputAvailable() NOEXCEPT;

	/*
	*	Registers an editor window.
	*/
	void RegisterEditorWindow(const EditorWindow window, const WindowCallback callback) NOEXCEPT;

	/*
	*	Registers a game window.
	*/
	void RegisterGameWindow(const GameWindow window, const WindowCallback callback) NOEXCEPT;

	/*
	*	Utility function for beginning an ImGui window.
	*	Returns if the window should remain opened.
	*/
	bool BeginWindow(const BeginWindowParameters &parameters) NOEXCEPT;

private:
	
	/*
	*	Window data class definition.
	*/
	class WindowData final
	{

	public:

		//The minimum.
		Vector2<float32> _Minimum;

		//The maximum.
		Vector2<float32> _Maximum;

		//The window callback.
		WindowCallback _WindowCallback;

	};

	//The editor window data.
	StaticArray<WindowData, UNDERLYING(EditorWindow::NUMBER_OF_EDITOR_WINDOWS)> _EditorWindowData;

	//The game window data.
	StaticArray<WindowData, UNDERLYING(GameWindow::NUMBER_OF_GAME_WINDOWS)> _GameWindowData;

};
#endif