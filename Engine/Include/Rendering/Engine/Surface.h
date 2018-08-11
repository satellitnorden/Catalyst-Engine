#pragma once

//Core.
#include <Core/EngineCore.h>

//Forward declarations.
struct GLFWwindow;

class Surface final
{

public:

	/*
	*	Default constructor.
	*/
	Surface() NOEXCEPT;

	/*
	*	Default destructor.
	*/
	~Surface() NOEXCEPT;

	/*
	*	Returns the underlying GLFW window, const version.
	*/
	RESTRICTED const GLFWwindow* const Get() const NOEXCEPT { return window; }

	/*
	*	Returns the underlying GLFW window, non-const version.
	*/
	RESTRICTED GLFWwindow* Get() NOEXCEPT { return window; }

	/*
	*	Initializes this surface.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Updates this surface.
	*/
	void Update() const NOEXCEPT;

	/*
	*	Releases this surface.
	*/
	void Release() NOEXCEPT;

	/*
	*	Returns whether or not this surface should close.
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