#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Resources.
#include <Resources/Core/ResourcesCore.h>

class Resource
{

public:

	//The reference count.
	uint32 _ReferenceCount{ 0 };

	//The load state.
	ResourceLoadState _LoadState{ ResourceLoadState::NOT_LOADED };

};