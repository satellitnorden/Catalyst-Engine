#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Resources.
#include <Resources/Core/LevelResource.h>
#include <Resources/Core/ResourcePointer.h>

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
		const ResourcePointer<LevelResource> level_resource,
		DynamicArray<LevelEntry> *const RESTRICT level_entries
	) NOEXCEPT;

};