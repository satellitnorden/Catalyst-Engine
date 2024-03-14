#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StreamArchive.h>

//Content.
#include <Content/Core/Asset.h>

class LevelAsset final : public Asset
{

public:

	//The type identifier.
	static HashString TYPE_IDENTIFIER;

	//The stream archive.
	StreamArchive _StreamArchive;

};