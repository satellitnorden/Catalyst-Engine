#if defined(CATALYST_EDITOR)
//Header file.
#include <Editor/EditorLevelSystem.h>

//Core.
#include <Core/General/Enumeration.h>

//Components.
#include <Components/Components/WorldTransformComponent.h>

//Content.
#include <Content/Core/AssetPointer.h>
#include <Content/Assets/LevelAsset.h>
#include <Content/Assets/MaterialAsset.h>
#include <Content/Assets/ModelAsset.h>

//Entities.
#include <Entities/Core/EntitySerialization.h>

//File.
#include <File/Core/FileCore.h>

//Systems.
#include <Systems/CatalystEditorSystem.h>
#include <Systems/ContentSystem.h>
#include <Systems/EntitySystem.h>
#include <Systems/ImGuiSystem.h>
#include <Systems/InputSystem.h>
#include <Systems/LevelSystem.h>
#include <Systems/RenderingSystem.h>
#include <Systems/ResourceSystem.h>

//World.
#include <World/Level/LevelStatistics.h>

//Third party.
#include <ThirdParty/ImGui/imgui.h>
#include <ThirdParty/ImGui/imgui_internal.h>
#include <ThirdParty/ImGuizmo/ImGuizmo.h>

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
*	Creates a custom float widget.
*/
FORCE_INLINE void FloatWidget
(
	const char *const RESTRICT label,
	Component *const RESTRICT component,
	Entity *const RESTRICT entity,
	const ComponentEditableField &editable_field,
	float32 *const RESTRICT value
) NOEXCEPT
{
	ImGui::Columns(2);

	ImGui::SetColumnWidth(0, 64.0f);
	ImGui::Text(label);
	ImGui::NextColumn();

	ImGui::PushMultiItemsWidths(1, ImGui::CalcItemWidth());
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));

	const float32 line_height{ GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f };
	const ImVec2 button_size{ line_height + 3.0f, line_height };

	ImGui::SameLine();

	float32 _value{ *value };

	if (ImGui::DragFloat("##FLOAT_VALUE", &_value, 0.1f))
	{
		component->PreEditableFieldChange(entity, editable_field);
		*value = _value;
		component->PostEditableFieldChange(entity, editable_field);
	}

	ImGui::PopItemWidth();
	ImGui::SameLine();

	ImGui::PopStyleVar();
	ImGui::Columns(1);
}

/*
*	Creates a custom enumeration widget. Returns if there was a change.
*/
FORCE_INLINE void EnumerationWidget
(
	const char *const RESTRICT label,
	Component *const RESTRICT component,
	Entity *const RESTRICT entity,
	const ComponentEditableField &editable_field,
	Enumeration *const RESTRICT enumeration
) NOEXCEPT
{
	ImGui::Columns(2);

	ImGui::SetColumnWidth(0, 64.0f);
	ImGui::Text(label);
	ImGui::NextColumn();

	ImGui::PushMultiItemsWidths(1, ImGui::CalcItemWidth());
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));

	const float32 line_height{ GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f };
	const ImVec2 button_size{ line_height + 3.0f, line_height };

	ImGui::SameLine();

	if (ImGui::BeginCombo("##ENUMERATION_VALUE", enumeration->ToString(), ImGuiComboFlags_PopupAlignLeft))
	{
		for (uint64 i{ 0 }; i < enumeration->Size(); ++i)
		{
			if (ImGui::Selectable(enumeration->IndexToString(i)))
			{
				component->PreEditableFieldChange(entity, editable_field);
				enumeration->SetFromIndex(i);
				component->PostEditableFieldChange(entity, editable_field);
			}
		}

		ImGui::EndCombo();
	}

	ImGui::PopItemWidth();
	ImGui::SameLine();

	ImGui::PopStyleVar();
	ImGui::Columns(1);
}

/*
*	Creates a custom material asset widget. Returns if there was a change.
*/
FORCE_INLINE NO_DISCARD bool MaterialAssetWidget(const char *const RESTRICT label, AssetPointer<MaterialAsset> *const RESTRICT asset) NOEXCEPT
{
	char buffer[256];
	sprintf_s(buffer, "%s: %s", label, (*asset)->_Header._AssetName.Data());

	if (ImGui::Selectable(buffer))
	{
		CatalystEditorSystem::Instance->GetEditorContentBrowser()->Request
		(
			"Select Material",
			MaterialAsset::TYPE_IDENTIFIER,
			[](Asset *const RESTRICT asset, void *const RESTRICT arguments)
			{
				AssetPointer<MaterialAsset> *const RESTRICT material_asset{ static_cast<AssetPointer<MaterialAsset> *const RESTRICT>(arguments) };

				(*material_asset) = AssetPointer<MaterialAsset>((MaterialAsset *const RESTRICT)asset);
			},
			asset
		);
	}

	//Eh.
	return false;
}

/*
*	Creates a custom model asset widget. Returns if there was a change.
*/
FORCE_INLINE NO_DISCARD bool ModelAssetWidget(const char *const RESTRICT label, AssetPointer<ModelAsset> *const RESTRICT model_asset) NOEXCEPT
{
	char buffer[256];
	sprintf_s(buffer, "%s: %s", label, (*model_asset)->_Header._AssetName.Data());

	if (ImGui::Selectable(buffer))
	{
		CatalystEditorSystem::Instance->GetEditorContentBrowser()->Request
		(
			"Select Model",
			ModelAsset::TYPE_IDENTIFIER,
			[](Asset *const RESTRICT asset, void *const RESTRICT arguments)
			{
				AssetPointer<ModelAsset> *const RESTRICT model_asset{ static_cast<AssetPointer<ModelAsset> *const RESTRICT>(arguments) };
			
				(*model_asset) = AssetPointer<ModelAsset>((ModelAsset *const RESTRICT)asset);
			},
			model_asset
		);
	}

	//Eh.
	return false;
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

	ImGui::PushMultiItemsWidths(static_cast<uint32>(data_size), ImGui::CalcItemWidth());
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
	_Level._LevelEntries.Emplace();
	LevelEntry &new_level_entry{ _Level._LevelEntries.Back() };

	//Generate a name.
	GenerateEntityName(new_level_entry._EditorData._Name.Data(), new_level_entry._EditorData._Name.Size());

	//Add components (entities created in the editor always have a world transform component).
	StaticArray<ComponentInitializationData *RESTRICT, 1> component_configurations;

	{
		WorldTransformInitializationData* const RESTRICT data{ WorldTransformComponent::Instance->AllocateDerivedInitializationData() };

		data->_WorldTransform = WorldTransform();

		component_configurations[0] = data;
	}

	//Create the entity!
	new_level_entry._Entity = EntitySystem::Instance->CreateEntity(ArrayProxy<ComponentInitializationData * RESTRICT>(component_configurations));
}

/*
*	Saves the current level.
*/
void EditorLevelSystem::SaveLevel() NOEXCEPT
{
	if (_LevelFilePath)
	{
		nlohmann::json JSON;

		SaveLevelInternal(JSON);

		//Write the JSON to the file.
		std::ofstream file{ _LevelFilePath.Data() };
		file << std::setw(4) << JSON;
		file.close();
	}

	else
	{
		SaveLevelAs();
	}
}

/*
*	Saves the current level as...
*/
void EditorLevelSystem::SaveLevelAs() NOEXCEPT
{
	DynamicString chosen_file;

	if (File::BrowseForFile(true, &chosen_file, "*.Level"))
	{
		_LevelFilePath = chosen_file;
		
		nlohmann::json JSON;

		SaveLevelInternal(JSON);

		//Writ the JSON to the file.
		std::ofstream file{ _LevelFilePath.Data() };
		file << std::setw(4) << JSON;
		file.close();
	}
}

/*
*	Loads a level
*/
void EditorLevelSystem::LoadLevel() NOEXCEPT
{
	//Browse for a level.
	DynamicString chosen_file;

	if (File::BrowseForFile(false, &chosen_file, "*.Level"))
	{
		//Set the level file path.
		_LevelFilePath = chosen_file;

		//Figure out the level name.
		{
			const std::string _file_path{ _LevelFilePath.Data() };
			const size_t position{ _file_path.find_last_of('\\') };
			const std::string name{ _file_path.substr(position + 1, _file_path.length() - position - strlen(".Level") - 1) };

			_Level._Name = name.c_str();
		}

		//Load the JSON object.
		nlohmann::json JSON;

		{
			std::ifstream input_file{ _LevelFilePath.Data() };
			input_file >> JSON;
			input_file.close();
		}

		//Load the level.
		LoadLevelInternal(JSON);
	}
}

/*
*	Starts the game.
*/
void EditorLevelSystem::StartGame() NOEXCEPT
{
	//Save down the level, so we can restore if once the game is ended.
	SaveLevelInternal(_CurrentlyPlayedLevel);

	//Write the JSON to a file for debug purpises..
	std::ofstream file{ "CurrentlyPlayedLevel.Level" };
	file << std::setw(4) << _CurrentlyPlayedLevel;
	file.close();
}

/*
*	Ends the game.
*/
void EditorLevelSystem::EndGame() NOEXCEPT
{
	//Restore the level.
	LoadLevelInternal(_CurrentlyPlayedLevel);

	//Clear the JSON object so we don't keep around old data.
	_CurrentlyPlayedLevel.clear();
}

/*
*	Saves a level internally.
*/
void EditorLevelSystem::SaveLevelInternal(nlohmann::json &JSON) NOEXCEPT
{
	//Calculate the level statistics.
	LevelStatistics level_statistics;

	//Set some defaults.
	level_statistics._Radius = 0.0f;

	//Write all entities.
	nlohmann::json &entities{ JSON["Entities"] };

	for (const LevelEntry &level_entry : _Level._LevelEntries)
	{
		//Cache the entity entry.
		nlohmann::json &entity_entry{ entities[level_entry._EditorData._Name.Data()] };

		//Serialize the entity.
		EntitySerialization::SerializeToJSON(entity_entry, level_entry._Entity);

		//Serialize the editor data.
		nlohmann::json &editor_data_entry{ entity_entry["EditorData"] };

		//Serialize the rotation.
		{
			nlohmann::json &rotation_entry{ editor_data_entry["Rotation"] };

			rotation_entry["Roll"] = CatalystBaseMath::RadiansToDegrees(level_entry._EditorData._Rotation._Roll);
			rotation_entry["Yaw"] = CatalystBaseMath::RadiansToDegrees(level_entry._EditorData._Rotation._Yaw);
			rotation_entry["Pitch"] = CatalystBaseMath::RadiansToDegrees(level_entry._EditorData._Rotation._Pitch);
		}

		//Update the level statistics from the world transform component.
		{
			//Cache the world transform instance data.
			const WorldTransformInstanceData &world_transform_instance_data{ WorldTransformComponent::Instance->InstanceData(level_entry._Entity) };

			//Calculate the distance from the center.
			const float32 distance_from_center{ Vector3<float32>::LengthXZ(world_transform_instance_data._CurrentWorldTransform.GetAbsolutePosition()) };

			//Update the radius.
			level_statistics._Radius = CatalystBaseMath::Maximum<float32>(level_statistics._Radius, distance_from_center);
		}
	}

	//Write the level statistics.
	nlohmann::json &level_statistics_entry{ JSON["LevelStatistics"] };

	level_statistics_entry["Radius"] = level_statistics._Radius;
}

/*
*	Loads a level internally.
*/
void EditorLevelSystem::LoadLevelInternal(const nlohmann::json &JSON) NOEXCEPT
{
	//Destroy all entities.
	EntitySystem::Instance->DestroyAllEntities();

	//Clear the level entries.
	_Level._LevelEntries.Clear();

	//Reset the selected level entry.
	_SelectedLevelEntryIndex = UINT64_MAXIMUM;

	//Set up the stream archive.
	StreamArchive stream_archive;

	//Cache the entities JSON object.
	const nlohmann::json &entities{ JSON["Entities"] };

	//Allocate the necessary amount of memory.
	_Level._LevelEntries.Reserve(entities.size());

	//Iterate through all entities.
	for (auto entity_iterator{ entities.begin() }; entity_iterator != entities.end(); ++entity_iterator)
	{
		//Add the new level entry.
		_Level._LevelEntries.Emplace();
		LevelEntry &new_level_entry{ _Level._LevelEntries.Back() };

		//Set the name.
		new_level_entry._EditorData._Name = entity_iterator.key().c_str();

		//Cache the entity entry.
		const nlohmann::json &entity_entry{ *entity_iterator };

		//Remember the stream archive position.
		uint64 stream_archive_position{ stream_archive.Size() };

		//Serialize to the stream archive.
		EntitySerialization::SerializeToStreamArchive(entity_entry, &stream_archive);

		//Now deserialize!
		new_level_entry._Entity = EntitySerialization::DeserializeFromStreamArchive(stream_archive, &stream_archive_position, nullptr);

		//Deserialize the editor data.
		const nlohmann::json &editor_data_entry{ entity_entry["EditorData"] };

		//Deserialize the rotation.
		{
			const nlohmann::json &rotation_entry{ editor_data_entry["Rotation"] };

			new_level_entry._EditorData._Rotation._Roll = CatalystBaseMath::DegreesToRadians(rotation_entry["Roll"]);
			new_level_entry._EditorData._Rotation._Yaw = CatalystBaseMath::DegreesToRadians(rotation_entry["Yaw"]);
			new_level_entry._EditorData._Rotation._Pitch = CatalystBaseMath::DegreesToRadians(rotation_entry["Pitch"]);
		}
	}
}

/*
*	Generates an entity name.
*/
void EditorLevelSystem::GenerateEntityName(char* const RESTRICT buffer, const uint64 buffer_size) NOEXCEPT
{
	sprintf_s(buffer, buffer_size, "Entity_%u", _NameCounter++);
}

/*
*	The top right window update.
*/
NO_DISCARD bool EditorLevelSystem::TopRightWindowUpdate(const Vector2<float32> minimum, const Vector2<float32> maximum) NOEXCEPT
{
	//Begin the window.
	ImGuiSystem::Instance->BeginWindow("Editor Level Top Right", minimum, maximum, false, false, false, false);

	//List all level entries.
	for (uint64 i{ 0 }; i < _Level._LevelEntries.Size(); ++i)
	{
		const LevelEntry &level_entry{ _Level._LevelEntries[i] };

		const uint64 name_length{ StringUtilities::StringLength(level_entry._EditorData._Name.Data()) };

		if (ImGui::Selectable(name_length == 0 ? "EMPTY" : level_entry._EditorData._Name.Data(), _SelectedLevelEntryIndex == i))
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
	//Update the Gizmo mode.
	if (InputSystem::Instance->GetKeyboardState(InputLayer::USER_INTERFACE)->GetButtonState(KeyboardButton::ONE) == ButtonState::PRESSED)
	{
		_CurrentGizmoMode = GizmoMode::TRANSLATE;
	}

	else if (InputSystem::Instance->GetKeyboardState(InputLayer::USER_INTERFACE)->GetButtonState(KeyboardButton::TWO) == ButtonState::PRESSED)
	{
		_CurrentGizmoMode = GizmoMode::ROTATE;
	}

	else if (InputSystem::Instance->GetKeyboardState(InputLayer::USER_INTERFACE)->GetButtonState(KeyboardButton::THREE) == ButtonState::PRESSED)
	{
		_CurrentGizmoMode = GizmoMode::SCALE;
	}

	//Begin the window.
	ImGuiSystem::Instance->BeginWindow("Editor Level Bottom Right", minimum, maximum, false, false, false, true);

	//Set up stuff for the selected level entry.
	if (_SelectedLevelEntryIndex != UINT64_MAXIMUM)
	{
		//Cache the selected level entry.
		LevelEntry &selected_level_entry{ _Level._LevelEntries[_SelectedLevelEntryIndex] };

		//Add the menu bar.
		{
			ImGui::BeginMenuBar();

			if (ImGui::BeginMenu("Add Component"))
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
		TextInputWidget("Name", selected_level_entry._EditorData._Name.Data(), selected_level_entry._EditorData._Name.Size());

		//Add a duplicate button.
		bool should_duplicate{ false };

		if (ImGui::Button("Duplicate"))
		{
			should_duplicate = true;
		}

		//Add widgets for the world transform component.
		if (ImGui::CollapsingHeader(WorldTransformComponent::Instance->Name()))
		{
			//Retrieve the editable field.
			const ComponentEditableField &editable_field{ WorldTransformComponent::Instance->EditableFields()[0]};

			ASSERT(editable_field._Type == ComponentEditableField::Type::WORLD_TRANSFORM, "What happened here?");

			//Retrieve the world transform.
			WorldTransform *const RESTRICT world_transform{ WorldTransformComponent::Instance->EditableFieldData<WorldTransform>(selected_level_entry._Entity, editable_field) };

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
				EulerAngles rotation{ selected_level_entry._EditorData._Rotation };

				//Convert to degrees.
				for (uint8 i{ 0 }; i < 3; ++i)
				{
					rotation[i] = CatalystBaseMath::RadiansToDegrees(rotation[i]);
				}

				//Add the transform widget.
				const bool changed{ TransformWidget("Rotation", &rotation._Roll, 3, 0.0f, 0.1f) };

				//Update the rotation if anything changed.
				if (changed)
				{
					//Convert to radians.
					for (uint8 i{ 0 }; i < 3; ++i)
					{
						rotation[i] = CatalystBaseMath::DegreesToRadians(rotation[i]);
					}

					selected_level_entry._EditorData._Rotation = rotation;
					world_transform->SetRotation(rotation);
				}
			}

			//Add scale widget.
			{
				//Retrieve the scale.
				float32 scale{ world_transform->GetScale() };

				//Add the transform widget.
				const bool changed{ TransformWidget("Scale", &scale, 1, 1.0f, 0.01f) };

				//Update the scale if anything changed.
				if (changed)
				{
					world_transform->SetScale(scale);
				}
			}
		}

		//Add a Gizmo for the world transform!
		{
			WorldTransform *const RESTRICT world_transform{ WorldTransformComponent::Instance->EditableFieldData<WorldTransform>(selected_level_entry._Entity, WorldTransformComponent::Instance->EditableFields()[0]) };

			Matrix4x4 transformation_matrix;

			{
				Vector3<float32> position{ world_transform->GetAbsolutePosition() };
				EulerAngles rotation{ selected_level_entry._EditorData._Rotation };
				Vector3<float32> scale{ world_transform->GetScale() };

				//Rotation reported in degrees, so convert.
				for (uint8 i{ 0 }; i < 3; ++i)
				{
					rotation[i] = CatalystBaseMath::RadiansToDegrees(rotation[i]);
				}

				ImGuizmo::RecomposeMatrixFromComponents(&position._X, &rotation._Roll, &scale._X, &transformation_matrix._11);
			}

			const Matrix4x4 *const RESTRICT view_matrix{ RenderingSystem::Instance->GetCameraSystem()->GetCurrentCamera()->GetCameraMatrix() };
			Matrix4x4 projection_matrix{ *RenderingSystem::Instance->GetCameraSystem()->GetCurrentCamera()->GetProjectionMatrix() };

			//Unjitter.
			{
				const Vector2<float32>& projection_matrix_jitter{ RenderingSystem::Instance->GetCameraSystem()->GetCurrentCamera()->GetProjectionMatrixJitter() };

				projection_matrix._Matrix[2]._X += projection_matrix_jitter._X;
				projection_matrix._Matrix[2]._Y += projection_matrix_jitter._Y;
			}

			//Flip Y.
			projection_matrix._Matrix[1]._Y *= -1.0f;

			//Determine the operation.
			ImGuizmo::OPERATION operation;

			switch (_CurrentGizmoMode)
			{
				case GizmoMode::TRANSLATE:
				{
					operation = ImGuizmo::OPERATION::TRANSLATE;

					break;
				}

				case GizmoMode::ROTATE:
				{
					operation = ImGuizmo::OPERATION::ROTATE;

					break;
				}

				case GizmoMode::SCALE:
				{
					operation = ImGuizmo::OPERATION::SCALE_Y;

					break;
				}

				default:
				{
					ASSERT(false, "Invalid case!");

					break;
				}
			}

			const bool was_manipulated
			{
				ImGuizmo::Manipulate
				(
					&view_matrix->_11,
					&projection_matrix._11,
					operation,
					ImGuizmo::MODE::WORLD,
					&transformation_matrix._11
				)
			};

			if (was_manipulated)
			{
				Vector3<float32> position;
				EulerAngles rotation;
				Vector3<float32> scale;

				ImGuizmo::DecomposeMatrixToComponents(&transformation_matrix._11, &position._X, &rotation._Roll, &scale._X);

				//Rotation is reported in degrees, so convert to radians.
				for (uint8 i{ 0 }; i < 3; ++i)
				{
					rotation[i] = CatalystBaseMath::DegreesToRadians(rotation[i]);
				}

				world_transform->SetAbsolutePosition(position);
				selected_level_entry._EditorData._Rotation = rotation;
				world_transform->SetRotation(rotation);

				if (_CurrentGizmoMode == GizmoMode::SCALE)
				{	
					//As the scale is uniform, just use the X axis for now.
					world_transform->SetScale(scale._Y);
				}
			}
		}

		/*
		*	Alright, this is crude, but right now, just iterate over all components,
		*	check if this entity has that, and expose the editable fields.
		*/
		for (Component *const RESTRICT component : AllComponents())
		{
			//Ignore editor data and world transform components, as they are always implicitly added.
			if (component == WorldTransformComponent::Instance.Get())
			{
				continue;
			}

			if (component->Has(selected_level_entry._Entity))
			{
				if (ImGui::CollapsingHeader(component->Name()))
				{
					//Add widgets for all editable fields.
					for (const ComponentEditableField &editable_field : component->EditableFields())
					{
						switch (editable_field._Type)
						{
							case ComponentEditableField::Type::FLOAT:
							{
								float32 *const RESTRICT value{ component->EditableFieldData<float32>(selected_level_entry._Entity, editable_field) };

								FloatWidget
								(
									editable_field._Name,
									component,
									selected_level_entry._Entity,
									editable_field,
									value
								);

								break;
							}

							case ComponentEditableField::Type::ENUMERATION:
							{
								Enumeration *const RESTRICT enumeration{ component->EditableFieldData<Enumeration>(selected_level_entry._Entity, editable_field) };

								EnumerationWidget
								(
									editable_field._Name,
									component,
									selected_level_entry._Entity,
									editable_field,
									enumeration
								);

								break;
							}

							case ComponentEditableField::Type::MATERIAL_ASSET:
							{
								AssetPointer<MaterialAsset> *const RESTRICT asset{ component->EditableFieldData<AssetPointer<MaterialAsset>>(selected_level_entry._Entity, editable_field) };

								MaterialAssetWidget(editable_field._Name, asset);

								break;
							}

							case ComponentEditableField::Type::MODEL_ASSET:
							{
								AssetPointer<ModelAsset> *const RESTRICT asset{ component->EditableFieldData<AssetPointer<ModelAsset>>(selected_level_entry._Entity, editable_field) };

								ModelAssetWidget(editable_field._Name, asset);

								break;
							}

							case ComponentEditableField::Type::POSITION:
							{
								Vector3<float32> *const RESTRICT editable_field_data{ component->EditableFieldData<Vector3<float32>>(selected_level_entry._Entity, editable_field) };

								TransformWidget(editable_field._Name, &editable_field_data->_X, 3, 0.0f, 0.01f);

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

		//Duplicate the entity, if requested.
		if (should_duplicate)
		{
			DuplicateEntity(selected_level_entry._Entity);
		}
	}

	//End the window.
	ImGui::End();

	//This window should always be shown.
	return true;
}

/*
*	Duplicates an entry.
*/
void EditorLevelSystem::DuplicateEntity(Entity *const RESTRICT entity) NOEXCEPT
{
	//Add a new level entry.
	_Level._LevelEntries.Emplace();
	LevelEntry &new_level_entry{ _Level._LevelEntries.Back() };

	GenerateEntityName(new_level_entry._EditorData._Name.Data(), new_level_entry._EditorData._Name.Size());

	/*
	*	Maybe a bit overkill to do this whole round trip, but it keeps me from having to write more code here.
	*	Shouldn't be too bad. (:
	*/
	nlohmann::json JSON;
	StreamArchive stream_archive;
	uint64 stream_archive_position{ 0 };

	EntitySerialization::SerializeToJSON(JSON, entity);
	EntitySerialization::SerializeToStreamArchive(JSON, &stream_archive);
	new_level_entry._Entity = EntitySerialization::DeserializeFromStreamArchive(stream_archive, &stream_archive_position, nullptr);

	//Set the new selected level entry index.
	_SelectedLevelEntryIndex = _Level._LevelEntries.LastIndex();
}
#endif