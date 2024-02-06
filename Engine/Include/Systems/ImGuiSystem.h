#if !defined(CATALYST_CONFIGURATION_FINAL)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

class ALIGN(8) ImGuiSystem final
{

public:

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

};
#endif