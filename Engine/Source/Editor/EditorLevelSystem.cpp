#if defined(CATALYST_EDITOR)
//Header file.
#include <Editor/EditorLevelSystem.h>

//Editor.
#include <Editor/EditorCore.h>
#include <Editor/EditorUtilities.h>

//File.
#include <File/Core/FileCore.h>

//Systems.
#include <Systems/CatalystEditorSystem.h>
#include <Systems/LevelSystem.h>
#include <Systems/ResourceSystem.h>

//Third party.
#include <ThirdParty/ImGui/imgui.h>

/*
*	Updates the editor level system.
*/
void EditorLevelSystem::Update() NOEXCEPT
{
	//Add the contextual window.
	AddContextualWindow();

	//Add the current level window.
	AddCurrentLevelWindow();
}

/*
*	Starts the game.
*/
void EditorLevelSystem::StartGame() NOEXCEPT
{
	//Gather the current level entries.
	GatherLevelEntries(&_CurrentLevelEntries);
}

/*
*	Ends the game.
*/
void EditorLevelSystem::EndGame() NOEXCEPT
{
	//Load the level that was saved before entering the game.
	LevelResource level_resource;
	level_resource._LevelEntries = _CurrentLevelEntries;

	LevelSystem::Instance->LoadLevel(ResourcePointer<LevelResource>(&level_resource));
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

	//Check for the ".cr" extension. Remove it if it exists.
	if (chosen_file.Find(".cr"))
	{
		chosen_file.SetLength(chosen_file.Length() - 3);
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

	//Load the level.
	LevelSystem::Instance->LoadLevel(ResourceSystem::Instance->GetLevelResource(HashString(identifier.Data())));

	//Set the current level name.
	_CurrentLevelName = identifier;
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

	//The user might already have chosen an existing level, so remove the ".cr" extension if that's the case.
	if (chosen_file.Find(".cr"))
	{
		chosen_file.SetLength(chosen_file.Length() - 3);
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

	//Build the level resource.
	{
		LevelBuildParameters parameters;

		parameters._OutputFilePath = chosen_file.Data();
		parameters._Identifier = identifier.Data();

		GatherLevelEntries(&parameters._LevelEntries);

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildLevel(parameters);
	}

	//The resource building system automatically adds the ".cr" extension to Catalyst resources, so add it to the chosen file path.
	chosen_file += ".cr";

	//Now load the resource into memory!
	ResourceSystem::Instance->LoadResource(chosen_file.Data());

	//Load the level!
	LevelSystem::Instance->LoadLevel(ResourceSystem::Instance->GetLevelResource(HashString(identifier.Data())));

	//Set the current level name.
	_CurrentLevelName = identifier;
}

/*
*	Adds the contextual window.
*/
void EditorLevelSystem::AddContextualWindow()
{
	//Is the current contextual window LEVEL?
	if (CatalystEditorSystem::Instance->GetCurrentContextualWindow() != CatalystEditorSystem::ContextualWindow::LEVEL)
	{
		return;
	}

	//Add the level window.
	ImGui::Begin("Level", nullptr, EditorConstants::WINDOW_FLAGS);
	EditorUtilities::SetWindowPositionAndSize(WindowAnchor::BOTTOM_LEFT, Vector2<float32>(0.0f, 0.0f), Vector2<float32>(EditorConstants::GENERAL_WINDOW_WIDTH, 0.5f));

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
		ImGui::Begin("Open Level", nullptr, EditorConstants::WINDOW_FLAGS);
		EditorUtilities::SetWindowPositionAndSize(WindowAnchor::BOTTOM_LEFT, Vector2<float32>(EditorConstants::GENERAL_WINDOW_WIDTH, 0.0f), Vector2<float32>(EditorConstants::GENERAL_WINDOW_WIDTH, 0.5f));

		const HashTable<HashString, LevelResource *RESTRICT> &all_level_resources{ ResourceSystem::Instance->GetAllLevelResources() };

		for (const LevelResource *const RESTRICT level_resource : all_level_resources.ValueIterator())
		{
			if (ImGui::Button(level_resource->_Header._ResourceName.Data()))
			{
				LevelSystem::Instance->LoadLevel(ResourceSystem::Instance->GetLevelResource(level_resource->_Header._ResourceIdentifier));

				_IsCurrentlyOpeningLevel = false;

				_CurrentLevelName = ResourceSystem::Instance->GetLevelResource(level_resource->_Header._ResourceIdentifier)->_Header._ResourceName.Data();

				break;
			}
		}

		ImGui::End();
	}
}

/*
*	Adds the current level window.
*/
void EditorLevelSystem::AddCurrentLevelWindow() NOEXCEPT
{
	//Add the level window.
	{
		char buffer[128];

		if (_CurrentLevelName)
		{
			sprintf_s(buffer, "Current Level: %s", _CurrentLevelName.Data());
		}

		else
		{
			sprintf_s(buffer, "Current Level: None");
		}

		ImGui::Begin(buffer, nullptr, EditorConstants::WINDOW_FLAGS);
	}

	EditorUtilities::SetWindowPositionAndSize(WindowAnchor::TOP_RIGHT, Vector2<float32>(-EditorConstants::GENERAL_WINDOW_WIDTH, -0.5f), Vector2<float32>(EditorConstants::GENERAL_WINDOW_WIDTH, 0.5f));

	/*
	//List all dynamic model entities.
	{
		const uint64 number_of_components{ ComponentManager::GetNumberOfDynamicModelComponents() };

		for (uint64 i{ 0 }; i < number_of_components; ++i)
		{
			char buffer[64];

			sprintf_s(buffer, "Dynamic Model Entity #%llu", i + 1);

			if (ImGui::Button(buffer))
			{
				CatalystEditorSystem::Instance->GetEditorSelectionSystem()->SetCurrentlySelectedEntity(ComponentManager::GetDynamicModelEntities()->At(i));
			}
		}
	}

	//List all light entities.
	{
		const uint64 number_of_components{ ComponentManager::GetNumberOfLightComponents() };

		for (uint64 i{ 0 }; i < number_of_components; ++i)
		{
			char buffer[64];

			sprintf_s(buffer, "Light Entity #%llu", i + 1);

			if (ImGui::Button(buffer))
			{
				CatalystEditorSystem::Instance->GetEditorSelectionSystem()->SetCurrentlySelectedEntity(ComponentManager::GetLightEntities()->At(i));
			}
		}
	}

	//List all static model entities.
	{
		const uint64 number_of_components{ ComponentManager::GetNumberOfStaticModelComponents() };

		for (uint64 i{ 0 }; i < number_of_components; ++i)
		{
			char buffer[64];

			sprintf_s(buffer, "Static Model Entity #%llu", i + 1);

			if (ImGui::Button(buffer))
			{
				CatalystEditorSystem::Instance->GetEditorSelectionSystem()->SetCurrentlySelectedEntity(ComponentManager::GetStaticModelEntities()->At(i));
			}
		}
	}

	//List all user interface entities.
	{
		const uint64 number_of_components{ ComponentManager::GetNumberOfUserInterfaceComponents() };

		for (uint64 i{ 0 }; i < number_of_components; ++i)
		{
			char buffer[64];

			sprintf_s(buffer, "User Interface Entity #%llu", i + 1);

			if (ImGui::Button(buffer))
			{
				CatalystEditorSystem::Instance->GetEditorSelectionSystem()->SetCurrentlySelectedEntity(ComponentManager::GetUserInterfaceEntities()->At(i));
			}
		}
	}
	*/

	ImGui::End();
}

/*
*	Gathers the level entries.
*/
void EditorLevelSystem::GatherLevelEntries(DynamicArray<LevelEntry> *const RESTRICT output) const NOEXCEPT
{
	/*
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

			for (uint64 i{ 0 }; i < RenderingConstants::MAXIMUM_NUMBER_OF_MESHES_PER_MODEL; ++i)
			{
				if (!component->_MaterialResources[i])
				{
					break;
				}

				level_entry._DynamicModelData._MaterialResourceIdentifiers.Emplace(component->_MaterialResources[i]->_Header._ResourceIdentifier);
			}

			level_entry._DynamicModelData._ModelCollisionConfiguration = component->_ModelCollisionConfiguration;
			level_entry._DynamicModelData._ModelSimulationConfiguration = component->_ModelSimulationConfiguration;

			output->Emplace(level_entry);
		}
	}

	//Add all light entities.
	{
		const uint64 number_of_components{ ComponentManager::GetNumberOfLightComponents() };
		const LightComponent *RESTRICT component{ ComponentManager::GetLightLightComponents() };

		for (uint64 i{ 0 }; i < number_of_components; ++i, ++component)
		{
			LevelEntry level_entry;

			level_entry._Type = LevelEntry::Type::LIGHT;

			switch (component->_LightType)
			{
				case LightType::DIRECTIONAL:
				{
					level_entry._LightData._Rotation = component->_Rotation;

					break;
				}

				case LightType::POINT:
				{
					level_entry._LightData._WorldPosition = component->_WorldPosition;

					break;
				}

				default:
				{
					ASSERT(false, "Invalid case!");

					break;
				}
			}

			level_entry._LightData._Color = component->_Color;
			level_entry._LightData._LightType = component->_LightType;
			level_entry._LightData._LightProperties = component->_LightProperties;
			level_entry._LightData._Intensity = component->_Intensity;
			level_entry._LightData._Radius = component->_Radius;
			level_entry._LightData._Size = component->_Size;

			output->Emplace(level_entry);
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

			for (uint64 i{ 0 }; i < RenderingConstants::MAXIMUM_NUMBER_OF_MESHES_PER_MODEL; ++i)
			{
				if (!component->_MaterialResources[i])
				{
					break;
				}

				level_entry._StaticModelData._MaterialResourceIdentifiers.Emplace(component->_MaterialResources[i]->_Header._ResourceIdentifier);
			}

			level_entry._StaticModelData._ModelCollisionConfiguration = component->_ModelCollisionConfiguration;

			output->Emplace(level_entry);
		}
	}

	//Add all user interface entities.
	{
		const uint64 number_of_components{ ComponentManager::GetNumberOfUserInterfaceComponents() };
		const UserInterfaceComponent *RESTRICT component{ ComponentManager::GetUserInterfaceUserInterfaceComponents() };

		for (uint64 i{ 0 }; i < number_of_components; ++i, ++component)
		{
			LevelEntry level_entry;

			level_entry._Type = LevelEntry::Type::USER_INTERFACE;
			level_entry._UserInterfaceData._UserInterfaceSceneIdentifier = component->_UserInterfaceScene ? component->_UserInterfaceScene->GetIdentifier() : HashString ("");
			level_entry._UserInterfaceData._WorldPosition = component->_WorldPosition;
			level_entry._UserInterfaceData._Rotation = component->_Rotation;
			level_entry._UserInterfaceData._Scale = component->_Scale;
			level_entry._UserInterfaceData._Roughness = component->_Roughness;
			level_entry._UserInterfaceData._Metallic = component->_Metallic;
			level_entry._UserInterfaceData._AmbientOcclusion = component->_AmbientOcclusion;
			level_entry._UserInterfaceData._EmissiveMultiplier = component->_EmissiveMultiplier;

			output->Emplace(level_entry);
		}
	}
	*/
}
#endif