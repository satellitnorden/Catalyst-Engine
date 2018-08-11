#pragma once

//Core.
#include <Core/EngineCore.h>

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
	*	Returns the underlying surface handle.
	*/
	RESTRICTED void* Get() NOEXCEPT { return surface; }

	/*
	*	Sets the underlying surface handle.
	*/
	void Set(void *const RESTRICT newSurface) { surface = newSurface; }

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

	//The underlying surface handle.
	void *RESTRICT surface;

};