#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Content.
#include <Content/Core/AssetHeader.h>

class Asset
{

public:

	//The header.
	AssetHeader _Header;

	//The reference count.
	uint32 _ReferenceCount{ 0 };

};