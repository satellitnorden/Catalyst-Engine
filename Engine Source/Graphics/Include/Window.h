#pragma once

//Engine core.
#include <EngineCore.h>

//Forward declarations.
struct GLFWwindow;

class Window final
{

public:

	/*
	*	Default constructor.
	*/
	Window() CATALYST_NOEXCEPT;

	/*
	*	Default destructor.
	*/
	~Window() CATALYST_NOEXCEPT;

	/*
	*	Returns the underlying GLFW window, const version.
	*/
	CATALYST_RESTRICTED const GLFWwindow* const Get() const CATALYST_NOEXCEPT { return window; }

	/*
	*	Returns the underlying GLFW window, non-const version.
	*/
	CATALYST_RESTRICTED GLFWwindow* Get() CATALYST_NOEXCEPT { return window; }

	/*
	*	Initializes this window.
	*/
	void Initialize() CATALYST_NOEXCEPT;

	/*
	*	Updates this window.
	*/
	void Update() const CATALYST_NOEXCEPT;

	/*
	*	Releases this window.
	*/
	void Release() CATALYST_NOEXCEPT;

	/*
	*	Returns whether or not this window should close.
	*/
	bool ShouldClose() const CATALYST_NOEXCEPT;

private:

	//The underlying GLFW window.
	GLFWwindow *CATALYST_RESTRICT window;

	//The width of the window.
	uint16 width;

	//The height of the window.
	uint16 height;

};