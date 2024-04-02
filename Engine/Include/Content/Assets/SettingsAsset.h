#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StreamArchive.h>

//Content.
#include <Content/Core/Asset.h>

class SettingsAsset final : public Asset
{

public:

	//The type identifier.
	static HashString TYPE_IDENTIFIER;

	//The number of objects.
	uint64 _NumberOfObjects;

	//The stream archive.
	StreamArchive _StreamArchive;

};