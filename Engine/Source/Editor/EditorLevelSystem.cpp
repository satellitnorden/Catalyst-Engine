#if defined(CATALYST_EDITOR)
//Header file.
#include <Editor/EditorLevelSystem.h>

//Components.
#include <Components/Core/ComponentManager.h>

//File.
#include <File/Core/FileCore.h>

//Systems.
#include <Systems/CatalystEditorSystem.h>
#include <Systems/LevelSystem.h>
#include <Systems/ResourceSystem.h>

//Third party.
#include <ThirdParty/imgui.h>

/*
*	Updates the editor level system.
*/
void EditorLevelSystem::Update() NOEXCEPT
{
	//Is the current contextual window LEVEL?
	if (CatalystEditorSystem::Instance->GetCurrentContextualWindow() != CatalystEditorSystem::ContextualWindow::LEVEL)
	{
		return;
	}

	//Add the level window.
	ImGui::Begin("Level", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);
	ImGui::SetWindowPos(ImVec2(0.0f, 256.0f));
	ImGui::SetWindowSize(ImVec2(256.0f, 512.0f));

	//Add the button for creating a new level.
	if (ImGui::Button("New Level"))
	{
		NewLevel();
	}

	//Add the button for opening a level.
	if (ImGui::Button("Open Level"))
	{
		OpenLevel();
	}

	//Add the button for saving the current level.
	if (ImGui::Button("Save Level"))
	{
		SaveLevel();
	}

	ImGui::End();

	//If the user is opening a level, open a new window and display the options.
	if (_IsCurrentlyOpeningLevel)
	{
		//Add the level window.
		ImGui::Begin("Open Level", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);
		ImGui::SetWindowPos(ImVec2(256.0f, 256.0f));
		ImGui::SetWindowSize(ImVec2(256.0f, 512.0f));

		const HashTable<HashString, LevelResource *RESTRICT> &all_level_resources{ ResourceSystem::Instance->GetAllLevelResources() };

		for (const LevelResource *const RESTRICT level_resource : all_level_resources.ValueIterator())
		{
			if (ImGui::Button(level_resource->_Header._ResourceName.Data()))
			{
				LevelSystem::Instance->DespawnAllLevels();
				LevelSystem::Instance->SpawnLevel(ResourceSystem::Instance->GetLevelResource(level_resource->_Header._ResourceIdentifier));

				_IsCurrentlyOpeningLevel = false;

				break;
			}
		}

		ImGui::End();
	}
}

/*
*	Called then the "New Level" button is pressed.
*/
void EditorLevelSystem::NewLevel() NOEXCEPT
{
	//Have the user pick a file.
	DynamicString chosen_file;

	if (!File::BrowseForFile(true, &chosen_file))
	{
		return;
	}

	//Retrieve the identifier.
	DynamicString identifier;

	for (int64 i{ static_cast<int64>(chosen_file.Length()) - 1 }; i >= 0; --i)
	{
		if (chosen_file[i] == '\\')
		{
			identifier = &chosen_file[i + 1];

			break;
		}
	}

	//Build the empty level resource.
	{
		//Build the level.
		LevelBuildParameters parameters;

		parameters._OutputFilePath = chosen_file.Data();
		parameters._Identifier = identifier.Data();

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildLevel(parameters);
	}

	//The resource building system automatically adds the ".cr" extension to Catalyst resources, so add it to the chosen file path.
	chosen_file += ".cr";

	//Now load the resource into memory!
	ResourceSystem::Instance->LoadResource(chosen_file.Data());
}

/*
*	Called then the "Open Level" button is pressed.
*/
void EditorLevelSystem::OpenLevel() NOEXCEPT
{
	//The user wants to open a level (or stop opening a level).
	_IsCurrentlyOpeningLevel = !_IsCurrentlyOpeningLevel;
}

/*
*	Called then the "Save Level" button is pressed.
*/
void EditorLevelSystem::SaveLevel() NOEXCEPT
{
	//Have the user pick the file to save it to.
	DynamicString chosen_file;

	if (!File::BrowseForFile(true, &chosen_file))
	{
		return;
	}

	ASSERT(File::GetExtension(chosen_file.Data()) == File::Extension::CR, "The file must be a Catalast resource!");

	//The user might already have chosen an existing level, so remove the ".cr" extension if that's the case.
	chosen_file[chosen_file.Length() - 3] = '\0';

	//Retrieve the identifier.
	DynamicString identifier;

	for (int64 i{ static_cast<int64>(chosen_file.Length()) - 1 }; i >= 0; --i)
	{
		if (chosen_file[i] == '\\')
		{
			identifier = &chosen_file[i + 1];

			break;
		}
	}

	//Build the empty level resource.
	{
		LevelBuildParameters parameters;

		parameters._OutputFilePath = chosen_file.Data();
		parameters._Identifier = identifier.Data();

		//Add all dynamic model entities.
		{
			const uint64 number_of_components{ ComponentManager::GetNumberOfDynamicModelComponents() };
			const DynamicModelComponent *RESTRICT component{ ComponentManager::GetDynamicModelDynamicModelComponents() };

			for (uint64 i{ 0 }; i < number_of_components; ++i, ++component)
			{
				LevelEntry level_entry;

				level_entry._Type = LevelEntry::Type::DYNAMIC_MODEL;
				level_entry._DynamicModelData._WorldTransform = component->_CurrentWorldTransform;
				level_entry._DynamicModelData._ModelResourceIdentifier = component->_ModelResource->_Header._ResourceIdentifier;

				for (uint8 i{ 0 }; i < RenderingConstants::MAXIMUM_NUMBER_OF_MESHES_PER_MODEL; ++i)
				{
					level_entry._DynamicModelData._MaterialResourceIdentifiers[i] = component->_MaterialResources[i] ? component->_MaterialResources[i]->_Header._ResourceIdentifier : HashString("");
				}

				level_entry._DynamicModelData._ModelCollisionConfiguration._Type = ModelCollisionType::AXIS_ALIGNED_BOUNDING_BOX;
				level_entry._DynamicModelData._SimulatePhysics = false;

				parameters._LevelEntries.Emplace(level_entry);
			}
		}

		//Add all static model entities.
		{
			const uint64 number_of_components{ ComponentManager::GetNumberOfStaticModelComponents() };
			const StaticModelComponent *RESTRICT component{ ComponentManager::GetStaticModelStaticModelComponents() };

			for (uint64 i{ 0 }; i < number_of_components; ++i, ++component)
			{
				LevelEntry level_entry;

				level_entry._Type = LevelEntry::Type::STATIC_MODEL;
				level_entry._StaticModelData._WorldTransform = component->_WorldTransform;
				level_entry._StaticModelData._ModelResourceIdentifier = component->_ModelResource->_Header._ResourceIdentifier;

				for (uint8 i{ 0 }; i < RenderingConstants::MAXIMUM_NUMBER_OF_MESHES_PER_MODEL; ++i)
				{
					level_entry._StaticModelData._MaterialResourceIdentifiers[i] = component->_MaterialResources[i] ? component->_MaterialResources[i]->_Header._ResourceIdentifier : HashString("");
				}

				level_entry._StaticModelData._ModelCollisionConfiguration._Type = ModelCollisionType::AXIS_ALIGNED_BOUNDING_BOX;

				parameters._LevelEntries.Emplace(level_entry);
			}
		}

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildLevel(parameters);
	}

	//The resource building system automatically adds the ".cr" extension to Catalyst resources, so add it to the chosen file path.
	chosen_file[chosen_file.Length() - 3] = '.';

	//Now load the resource into memory!
	ResourceSystem::Instance->LoadResource(chosen_file.Data());
}
#endif