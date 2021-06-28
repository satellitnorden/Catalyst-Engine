#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Entities.
#include <Entities/Types/Entity.h>

//Resources.
#include <Resources/Core/LevelResource.h>
#include <Resources/Core/ResourcePointer.h>

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
	*	Loads a level.
	*/
	void LoadLevel(const ResourcePointer<LevelResource> resource) NOEXCEPT;

	/*
	*	Returns the current level.
	*/
	NO_DISCARD ResourcePointer<LevelResource> GetCurrentLevel() const NOEXCEPT
	{
		return _CurrentLevel;
	}

	/*
	*	Resets the current level.
	*/
	void ResetCurrentLevel() NOEXCEPT;

private:

	//The current level.
	ResourcePointer<LevelResource> _CurrentLevel;

};