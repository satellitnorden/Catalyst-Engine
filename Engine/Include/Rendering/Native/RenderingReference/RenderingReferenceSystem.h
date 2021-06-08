#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

class RenderingReferenceSystem final
{

public:

	/*
	*	Returns whether or not rendering reference is in progress.
	*/
	NO_DISCARD bool IsRenderingReferenceInProgress() const NOEXCEPT;

	/*
	*	Starts the rendering reference.
	*/
	void StartRenderingReference() NOEXCEPT;

	/*
	*	Stops the rendering reference. Takes an optional file path to output the image.
	*/
	void StopRenderingReference(const char *const RESTRICT file_path = nullptr) NOEXCEPT;

};