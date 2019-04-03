#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

class Pipeline
{

public:

	//Enumeration covering all pipeline types.
	enum class Type : uint8
	{
		Graphics
	};

	/*
	*	Returns the pipeline type.
	*/
	NO_DISCARD Type GetType() const NOEXCEPT
	{
		return _Type;
	}

protected:

	//The pipeline type.
	Type _Type;

};