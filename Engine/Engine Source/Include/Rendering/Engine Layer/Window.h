#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Forward declarations.
struct GLFWwindow;

class Window final
{

public:

	/*
	*	Default constructor.
	*/
	Window() NOEXCEPT;

	/*
	*	Default destructor.
	*/
	~Window() NOEXCEPT;

	/*
	*	Returns the underlying GLFW window, const version.
	*/
	RESTRICTED const GLFWwindow* const Get() const NOEXCEPT { return window; }

	/*
	*	Returns the underlying GLFW window, non-const version.
	*/
	RESTRICTED GLFWwindow* Get() NOEXCEPT { return window; }

	/*
	*	Initializes this window.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Updates this window.
	*/
	void Update() const NOEXCEPT;

	/*
	*	Releases this window.
	*/
	void Release() NOEXCEPT;

	/*
	*	Returns whether or not this window should close.
	*/
	bool ShouldClose() const NOEXCEPT;

private:

	//The underlying GLFW window.
	GLFWwindow *RESTRICT window;

	//The width of the window.
	uint16 width;

	//The height of the window.
	uint16 height;

};