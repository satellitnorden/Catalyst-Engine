#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

class EntityPreprocessingParameters
{

public:

	//Denotes whether or not this entity should preprocess.
	bool _ShouldPreprocess{ false };

	//Denotes whether or not this entity can preprocess on the main thread.
	bool _CanPreprocessOnMainThread{ false };

};