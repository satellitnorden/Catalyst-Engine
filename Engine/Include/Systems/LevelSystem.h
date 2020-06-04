#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Entities.
#include <Entities/Types/Entity.h>

//Resources.
#include <Resources/Core/LevelResource.h>
#include <Resources/Core/ResourcePointer.h>

class LevelSystem final
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
	void SpawnLevel(const ResourcePointer<LevelResource> resource) NOEXCEPT;

	/*
	*	Despawns all levels.
	*/
	void DespawnAllLevels() NOEXCEPT;

private:

	/*
	*	Spawned level class definition.
	*/
	class SpawnedLevel final
	{

	public:

		//The level resource.
		ResourcePointer<LevelResource> _LevelResource;

		//The entities.
		DynamicArray<Entity *RESTRICT> _Entities;

	};

	//The spawned levels.
	DynamicArray<SpawnedLevel> _SpawnedLevels;

};