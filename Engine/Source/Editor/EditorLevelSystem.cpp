#if defined(CATALYST_EDITOR)
//Header file.
#include <Editor/EditorLevelSystem.h>

//Components.
#include <Components/Components/WorldTransformComponent.h>

//Systems.
#include <Systems/EntitySystem.h>

//Third party.
#include <ThirdParty/ImGui/imgui.h>

/*
*	Updates the editor level system.
*/
void EditorLevelSystem::Update() NOEXCEPT
{
	
}

/*
*	Starts the game.
*/
void EditorLevelSystem::StartGame() NOEXCEPT
{
	
}

/*
*	Ends the game.
*/
void EditorLevelSystem::EndGame() NOEXCEPT
{
	
}

/*
*	Creates an entity.
*/
void EditorLevelSystem::CreateEntity() NOEXCEPT
{
	//Add a new level entry.
	_LevelEntries.Emplace();
	LevelEntry &new_level_entry{ _LevelEntries.Back() };

	//Generate a name.
	{
		char buffer[128];
		sprintf_s(buffer, "Entity_%u", _NameCounter++);

		new_level_entry._Name = buffer;
	}

	//Add components (entities created in the editor always have a world transform component, for now).
	StaticArray<ComponentInitializationData *RESTRICT, 1> component_configurations;

	{
		WorldTransformInitializationData *const RESTRICT data{ WorldTransformComponent::Instance->AllocateInitializationData() };

		data->_WorldTransform = WorldTransform();

		component_configurations[0] = data;
	}

	//Create the entity!
	new_level_entry._Entity = EntitySystem::Instance->CreateEntity(ArrayProxy<ComponentInitializationData *RESTRICT>(component_configurations));
}
#endif