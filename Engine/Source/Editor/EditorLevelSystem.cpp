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
#include <ThirdParty/ImGui/imgui_internal.h>

/*
*	Creates a custom text input widget. Returns if there was a change.
*/
FORCE_INLINE NO_DISCARD bool TextInputWidget(const char *const RESTRICT label, char *const RESTRICT text, const uint64 text_size) NOEXCEPT
{
	//Push the ID.
	ImGui::PushID(label);

	//Set up the widget. Keep track of if anything changed.
	bool changed{ false };

	ImGui::Columns(2);

	ImGui::SetColumnWidth(0, 64.0f);
	ImGui::Text(label);
	ImGui::NextColumn();

	ImGui::PushMultiItemsWidths(1, 256.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));

	if (ImGui::InputText("##TEXT_INPUT", text, text_size))
	{
		changed = true;
	}

	ImGui::PopItemWidth();

	ImGui::PopStyleVar();
	ImGui::Columns(1);

	//Pop the ID:
	ImGui::PopID();

	//Return if there was a change!
	return changed;
}

/*
*	Creates a custom widget for stuff like position/rotation/scale. Returns if there was a change.
*/
FORCE_INLINE NO_DISCARD bool TransformWidget
(
	const char *const RESTRICT label,
	float32 *const RESTRICT data,
	const uint64 data_size,
	const float32 reset_value,
	const float32 drag_speed
) NOEXCEPT
{
	//Push the ID.
	ImGui::PushID(label);

	//Set up the widget. Keep track of if anything changed.
	bool changed{ false };

	ImGui::Columns(2);

	ImGui::SetColumnWidth(0, 64.0f);
	ImGui::Text(label);
	ImGui::NextColumn();

	ImGui::PushMultiItemsWidths(data_size, ImGui::CalcItemWidth());
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));

	const float32 line_height{ GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f };
	const ImVec2 button_size{ line_height + 3.0f, line_height };

	if (data_size >= 1)
	{
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.1f, 0.15f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.2f, 0.2f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.8f, 0.1f, 0.15f, 1.0f));
		if (ImGui::Button("X", button_size))
		{
			data[0] = reset_value;

			changed = true;
		}
		ImGui::PopStyleColor(3);

		ImGui::SameLine();

		if (ImGui::DragFloat("##X", &data[0], drag_speed))
		{
			changed = true;
		}

		ImGui::PopItemWidth();
		ImGui::SameLine();
	}
	
	if (data_size >= 2)
	{
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.7f, 0.3f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.8f, 0.3f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.7f, 0.2f, 1.0f));
		if (ImGui::Button("Y", button_size))
		{
			data[1] = reset_value;

			changed = true;
		}
		ImGui::PopStyleColor(3);

		ImGui::SameLine();

		if (ImGui::DragFloat("##Y", &data[1], drag_speed))
		{
			changed = true;
		}

		ImGui::PopItemWidth();
		ImGui::SameLine();
	}

	if (data_size >= 3)
	{
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.25f, 0.8f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.1f, 0.35f, 0.9f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.25f, 0.8f, 1.0f));
		if (ImGui::Button("Z", button_size))
		{
			data[2] = 0.0f;

			changed = true;
		}
		ImGui::PopStyleColor(3);

		ImGui::SameLine();

		if (ImGui::DragFloat("##Z", &data[2], drag_speed))
		{
			changed = true;
		}

		ImGui::PopItemWidth();
	}

	ImGui::PopStyleVar();
	ImGui::Columns(1);

	ImGui::PopID();

	return changed;
}

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
		TextInputWidget("Name", selected_level_entry._Name.Data(), selected_level_entry._Name.Size());

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
									//Retrieve the position.
									Vector3<float32> position{ world_transform->GetAbsolutePosition() };

									//Add the transform widget.
									const bool changed{ TransformWidget("Position", &position._X, 3, 0.0f, 0.01f) };

									//Update the position if anything changed.
									if (changed)
									{
										world_transform->SetAbsolutePosition(position);
									}
								}

								//Add rotation widget.
								{
									//Retrieve the rotation.
									EulerAngles rotation{ world_transform->GetRotation().ToEulerAngles()};

									//Add the transform widget.
									const bool changed{ TransformWidget("Rotation", &rotation._Roll, 3, 0.0f, 0.01f) };

									//Update the rotation if anything changed.
									if (changed)
									{
										world_transform->SetRotation(rotation);
									}
								}

								//Add scale widget.
								{
									//Retrieve the scale.
									float32 scale{ world_transform->GetScale()};

									//Add the transform widget.
									const bool changed{ TransformWidget("Scale", &scale, 1, 1.0f, 0.01f) };

									//Update the scale if anything changed.
									if (changed)
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