#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

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
	*	Loads a level.
	*/
	void LoadLevel(const ResourcePointer<LevelResource> resource) NOEXCEPT;

};