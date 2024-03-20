#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StreamArchive.h>

//Content.
#include <Content/Core/Asset.h>

//World.
#include <World/Level/LevelStatistics.h>

class LevelAsset final : public Asset
{

public:

	//The type identifier.
	static HashString TYPE_IDENTIFIER;

	//The level statistics.
	LevelStatistics _LevelStatistics;

	//The stream archive.
	StreamArchive _StreamArchive;

};