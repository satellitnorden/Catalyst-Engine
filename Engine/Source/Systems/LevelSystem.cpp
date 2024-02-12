//Header file.
#include <Systems/LevelSystem.h>

//Systems.
#include <Systems/EntitySystem.h>
#include <Systems/ResourceSystem.h>

//Singleton definition.
DEFINE_SINGLETON(LevelSystem);

/*
*	Loads a level.
*/
void LevelSystem::LoadLevel(const ResourcePointer<LevelResource> resource) NOEXCEPT
{
	/*
	//Destroy all entities.
	//EntitySystem::Instance->DestroyAllEntities();

	//Set the current level.
	_CurrentLevel = resource;

	//Process all level entries.
	for (const LevelEntry &level_entry : resource->_LevelEntries)
	{
		switch (level_entry._Type)
		{
			default:
			{
				ASSERT(false, "Invalid case!");

				break;
			}
		}
	}
	*/
}

/*
*	Resets the current level.
*/
void LevelSystem::ResetCurrentLevel() NOEXCEPT
{
	if (!_CurrentLevel)
	{
		return;
	}

	//Simply re-load the level.
	LoadLevel(_CurrentLevel);
}