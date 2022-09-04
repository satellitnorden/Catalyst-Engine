#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Resources.
#include <Resources/Core/ResourcesCore.h>
#include <Resources/Core/ResourceHeader.h>

class Resource
{

public:

	//The header.
	ResourceHeader _Header;

	//The file path from which this resource was loaded from.
	const char *RESTRICT _FilePath;

	//The file offset from which this resource was loaded from.
	uint64 _FileOffset;

	//The reference count.
	uint32 _ReferenceCount{ 0 };

	//The load state.
	ResourceLoadState _LoadState{ ResourceLoadState::NOT_LOADED };

};