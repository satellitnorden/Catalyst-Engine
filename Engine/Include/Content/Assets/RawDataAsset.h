#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Content.
#include <Content/Core/Asset.h>

class RawDataAsset final : public Asset
{

public:

	//The type identifier.
	static HashString TYPE_IDENTIFIER;

	//The data.
	DynamicArray<byte> _Data;

};