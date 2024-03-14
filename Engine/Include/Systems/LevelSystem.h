#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Content.
#include <Content/Core/AssetPointer.h>
#include <Content/Assets/LevelAsset.h>

//World.
#include <World/Core/WorldTransform.h>
#include <World/Level/LevelEntry.h>

class ALIGN(8) LevelSystem final
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(LevelSystem);

	/*
	*	Default constructor.
	*/
	FORCE_INLINE LevelSystem() NOEXCEPT
	{
	
	}

	/*
	*	Spawns a level.
	*/
	void SpawnLevel
	(
		const WorldTransform &world_transform,
		const AssetPointer<LevelAsset> level_asset,
		DynamicArray<LevelEntry> *const RESTRICT level_entries
	) NOEXCEPT;

};