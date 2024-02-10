#if !defined(CATALYST_CONFIGURATION_FINAL)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Math.
#include <Math/General/Vector.h>

//Type aliases.
using WindowCallback = bool(*)(const Vector2<float32> minimum, const Vector2<float32> maximum);

class ALIGN(8) ImGuiSystem final
{

public:

	//Enumeration covering all editor windows.
	enum class EditorWindow : uint8
	{
		TOP_RIGHT,
		BOTTOM_RIGHT,

		NUMBER_OF_EDITOR_WINDOWS
	};

	//Enumeration covering all game windows.
	enum class GameWindow : uint8
	{
		RIGHT,

		NUMBER_OF_GAME_WINDOWS
	};

	//Singleton declaration.
	DECLARE_SINGLETON(ImGuiSystem);

	/*
	*	Default constructor.
	*/
	ImGuiSystem() NOEXCEPT
	{

	}

	/*
	*	Initializes the ImGui system.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Post-terminates the ImGui system.
	*/
	void PostTerminate() NOEXCEPT;

	/*
	*	Callback for when input is available.
	*/
	void OnInputAvailable() NOEXCEPT;

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

	/*
	*	Updates during the USER_INTERFACE update phase.
	*/
	void UserInterfaceUpdate() NOEXCEPT;

	/*
	*	Empty window callback.
	*/
	void EmptyWindowCallback(const Vector2<float32> minimum, const Vector2<float32> maximum) NOEXCEPT;

};
#endif