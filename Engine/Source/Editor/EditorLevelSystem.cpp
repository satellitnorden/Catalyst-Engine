#if defined(CATALYST_EDITOR)
//Header file.
#include <Editor/EditorLevelSystem.h>

//Components.
#include <Components/Components/WorldTransformComponent.h>

//Systems.
#include <Systems/CatalystEditorSystem.h>
#include <Systems/EntitySystem.h>
#include <Systems/ImGuiSystem.h>

//Third party.
#include <ThirdParty/ImGui/imgui.h>

/*
*	Post initializes the editor level system.
*/
void EditorLevelSystem::PostInitialize() NOEXCEPT
{
	//Register for the top right editor window.
	ImGuiSystem::Instance->RegisterEditorWindow
	(
		ImGuiSystem::EditorWindow::TOP_RIGHT,
		[](const Vector2<float32> minimum, const Vector2<float32> maximum)
		{
			return CatalystEditorSystem::Instance->GetEditorLevelSystem()->TopRightWindowUpdate(minimum, maximum);
		}
	);

	//Register for the bottom right editor window.
	ImGuiSystem::Instance->RegisterEditorWindow
	(
		ImGuiSystem::EditorWindow::BOTTOM_RIGHT,
		[](const Vector2<float32> minimum, const Vector2<float32> maximum)
		{
			return CatalystEditorSystem::Instance->GetEditorLevelSystem()->BottomRightWindowUpdate(minimum, maximum);
		}
	);
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
		WorldTransformInitializationData *const RESTRICT data{ WorldTransformComponent::Instance->AllocateDerivedInitializationData() };

		data->_WorldTransform = WorldTransform();

		component_configurations[0] = data;
	}

	//Create the entity!
	new_level_entry._Entity = EntitySystem::Instance->CreateEntity(ArrayProxy<ComponentInitializationData *RESTRICT>(component_configurations));
}

/*
*	The top right window update.
*/
NO_DISCARD bool EditorLevelSystem::TopRightWindowUpdate(const Vector2<float32> minimum, const Vector2<float32> maximum) NOEXCEPT
{
	//Begin the window.
	ImGuiSystem::Instance->BeginWindow("Editor Level Top Right", minimum, maximum, false, false);

	//List all level entries.
	for (uint64 i{ 0 }; i < _LevelEntries.Size(); ++i)
	{
		const LevelEntry &level_entry{ _LevelEntries[i] };

		const uint64 name_length{ StringUtilities::StringLength(level_entry._Name.Data()) };

		if (ImGui::Selectable(name_length == 0 ? "EMPTY" : level_entry._Name.Data(), _SelectedLevelEntryIndex == i))
		{
			_SelectedLevelEntryIndex = i;
		}
	}

	//End the window.
	ImGui::End();

	//This window should always be shown.
	return true;
}

/*
*	The bottom right window update.
*/
NO_DISCARD bool EditorLevelSystem::BottomRightWindowUpdate(const Vector2<float32> minimum, const Vector2<float32> maximum) NOEXCEPT
{
	//Begin the window.
	ImGuiSystem::Instance->BeginWindow("Editor Level Bottom Right", minimum, maximum, false, true);

	//Set up stuff for the selected level entry.
	if (_SelectedLevelEntryIndex != UINT64_MAXIMUM)
	{
		//Cache the selected level entry.
		LevelEntry &selected_level_entry{ _LevelEntries[_SelectedLevelEntryIndex] };

		//Add the menu bar.
		{
			ImGui::BeginMenuBar();

			if (ImGui::BeginMenu("Add"))
			{
				for (Component *const RESTRICT component : AllComponents())
				{
					if (component->Has(selected_level_entry._Entity))
					{
						continue;
					}

					if (ImGui::MenuItem(component->Name()))
					{
						ComponentInitializationData *const RESTRICT initialization_data{ component->AllocateInitializationData() };
						EntitySystem::Instance->AddComponentToEntity(selected_level_entry._Entity, initialization_data);
					}
				}
				
				ImGui::EndMenu();
			}

			//End the menu.
			ImGui::EndMenuBar();
		}

		//Add a text input for the name.
		ImGui::InputText("Name", selected_level_entry._Name.Data(), selected_level_entry._Name.Size());

		/*
		*	Alright, this is crude, but right now, just iterate over all components,
		*	check if this entity has that, and expose the editable fields.
		*/
		for (Component *const RESTRICT component : AllComponents())
		{
			if (component->Has(selected_level_entry._Entity))
			{
				if (ImGui::CollapsingHeader(component->Name()))
				{
					//Add widgets for all editable fields.
					for (const ComponentEditableField &editable_field : component->_EditableFields)
					{
						switch (editable_field._Type)
						{
							case ComponentEditableField::Type::WORLD_TRANSFORM:
							{
								WorldTransform *const RESTRICT world_transform{ component->EditableFieldData<WorldTransform>(selected_level_entry._Entity, editable_field) };

								//Add a header.
								ImGui::Text("World Transform");

								//Add position widget.
								{
									Vector3<float32> position{ world_transform->GetAbsolutePosition() };

									if (ImGui::DragFloat3("Position", position._Data))
									{
										world_transform->SetAbsolutePosition(position);
									}
								}

								//Add rotation widget.
								{
									EulerAngles rotation{ world_transform->GetRotation().ToEulerAngles()};

									if (ImGui::DragFloat3("Rotation", rotation._Data))
									{
										world_transform->SetRotation(rotation);
									}
								}

								//Add scale widget.
								{
									float32 scale{ world_transform->GetScale()};

									if (ImGui::DragFloat("Scale", &scale))
									{
										world_transform->SetScale(scale);
									}
								}

								break;
							}

							default:
							{
								ASSERT(false, "Invalid case!");

								break;
							}
						}
					}
				}
			}
		}
	}

	//End the window.
	ImGui::End();

	//This window should always be shown.
	return true;
}
#endif