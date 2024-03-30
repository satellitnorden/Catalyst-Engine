#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StreamArchive.h>

//Content.
#include <Content/Core/Asset.h>

//Entities.
#include <Entities/Core/EntityStatistics.h>

class EntityAsset final : public Asset
{

public:

	//The type identifier.
	static HashString TYPE_IDENTIFIER;

	//The entity statistics.
	EntityStatistics _EntityStatistics;

	//The stream archive.
	StreamArchive _StreamArchive;

};