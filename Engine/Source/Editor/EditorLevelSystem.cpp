#if defined(CATALYST_EDITOR)
//Header file.
#include <Editor/EditorLevelSystem.h>

//Core.
#include <Core/General/Enumeration.h>

//Components.
#include <Components/Components/StaticModelComponent.h>
#include <Components/Components/WorldTransformComponent.h>

//Content.
#include <Content/Core/AssetPointer.h>
#include <Content/Assets/AnimatedModelAsset.h>
#include <Content/Assets/AnimationAsset.h>
#include <Content/Assets/LevelAsset.h>
#include <Content/Assets/MaterialAsset.h>
#include <Content/Assets/ModelAsset.h>
#include <Content/Assets/SoundAsset.h>

//Entities.
#include <Entities/Core/EntitySerialization.h>
#include <Entities/Core/EntityStatistics.h>

//File.
#include <File/Core/File.h>

//Rendering.
#include <Rendering/Native/RenderingUtilities.h>

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

//STL.
#include <fstream>

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
*	Creates a custom bool widget.
*/
FORCE_INLINE void BoolWidget
(
	const char *const RESTRICT label,
	Component *const RESTRICT component,
	Entity *const RESTRICT entity,
	const ComponentEditableField &editable_field,
	bool *const RESTRICT value
) NOEXCEPT
{
	bool _value{ *value };

	if (ImGui::Checkbox(label, &_value))
	{
		component->PreEditableFieldChange(entity, editable_field);
		*value = _value;
		component->PostEditableFieldChange(entity, editable_field);
	}
}

/*
*	Creates a custom color widget.
*/
FORCE_INLINE void ColorWidget
(
	const char *const RESTRICT label,
	Component *const RESTRICT component,
	Entity *const RESTRICT entity,
	const ComponentEditableField &editable_field,
	Vector3<float32> *const RESTRICT value
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

	float32 _value[3]{ value->_R, value->_G, value->_B };

	if (ImGui::ColorEdit3("##COLOR_VALUE", _value))
	{
		component->PreEditableFieldChange(entity, editable_field);
		value->_R = _value[0];
		value->_G = _value[1];
		value->_B = _value[2];
		component->PostEditableFieldChange(entity, editable_field);
	}

	ImGui::PopItemWidth();
	ImGui::SameLine();

	ImGui::PopStyleVar();
	ImGui::Columns(1);
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
	ImGui::Text(label);

	ImGui::PushMultiItemsWidths(1, ImGui::CalcItemWidth());

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
}

/*
*	Creates a custom hash string widget. Returns if there was a change.
*/
FORCE_INLINE void HashStringWidget
(
	const char *const RESTRICT label,
	Component *const RESTRICT component,
	Entity *const RESTRICT entity,
	const ComponentEditableField &editable_field,
	EditorEntityData::HashStringData *const RESTRICT hash_string_data,
	HashString *const RESTRICT hash_string
) NOEXCEPT
{
	ImGui::Text(label);

	ImGui::PushMultiItemsWidths(1, ImGui::CalcItemWidth());

	if (ImGui::InputText("##TEXT_INPUT", hash_string_data->_String.Data(), hash_string_data->_String.Size()))
	{
		component->PreEditableFieldChange(entity, editable_field);
		*hash_string = HashString(hash_string_data->_String.Data());
		component->PostEditableFieldChange(entity, editable_field);
	}

	ImGui::PopItemWidth();
}

/*
*	Creates a custom animated model asset widget. Returns if there was a change.
*/
FORCE_INLINE NO_DISCARD bool AnimatedModelAssetWidget
(
	const char *const RESTRICT label,
	Component *const RESTRICT component,
	Entity *const RESTRICT entity,
	const ComponentEditableField &editable_field,
	AssetPointer<AnimatedModelAsset> *const RESTRICT asset) NOEXCEPT
{
	char buffer[256];
	sprintf_s(buffer, "%s: %s", label, (*asset) ? (*asset)->_Header._AssetName.Data() : "None");

	if (ImGui::Selectable(buffer))
	{
		CatalystEditorSystem::Instance->GetEditorContentBrowser()->Request
		(
			"Select Animated Model",
			AnimatedModelAsset::TYPE_IDENTIFIER,
			component,
			entity,
			&editable_field,
			asset,
			[](Component *const RESTRICT component, Entity *const RESTRICT entity, const ComponentEditableField *const RESTRICT editable_field, Asset *const RESTRICT asset, void *const RESTRICT user_data)
			{
				AssetPointer<AnimatedModelAsset> *const RESTRICT animated_model_asset{ static_cast<AssetPointer<AnimatedModelAsset> *const RESTRICT>(user_data) };

				component->PreEditableFieldChange(entity, *editable_field);
				(*animated_model_asset) = AssetPointer<AnimatedModelAsset>((AnimatedModelAsset *const RESTRICT)asset);
				component->PostEditableFieldChange(entity, *editable_field);
			}
		);
	}

	//Eh.
	return false;
}

/*
*	Creates a custom animation asset widget. Returns if there was a change.
*/
FORCE_INLINE NO_DISCARD bool AnimationAssetWidget
(
	const char *const RESTRICT label,
	Component *const RESTRICT component,
	Entity *const RESTRICT entity,
	const ComponentEditableField &editable_field,
	AssetPointer<AnimationAsset> *const RESTRICT asset) NOEXCEPT
{
	char buffer[256];
	sprintf_s(buffer, "%s: %s", label, (*asset) ? (*asset)->_Header._AssetName.Data() : "None");

	if (ImGui::Selectable(buffer))
	{
		CatalystEditorSystem::Instance->GetEditorContentBrowser()->Request
		(
			"Select Animation",
			AnimationAsset::TYPE_IDENTIFIER,
			component,
			entity,
			&editable_field,
			asset,
			[](Component *const RESTRICT component, Entity *const RESTRICT entity, const ComponentEditableField *const RESTRICT editable_field, Asset *const RESTRICT asset, void *const RESTRICT user_data)
			{
				AssetPointer<AnimationAsset> *const RESTRICT animated_model_asset{ static_cast<AssetPointer<AnimationAsset> *const RESTRICT>(user_data) };

				component->PreEditableFieldChange(entity, *editable_field);
				(*animated_model_asset) = AssetPointer<AnimationAsset>((AnimationAsset *const RESTRICT)asset);
				component->PostEditableFieldChange(entity, *editable_field);
			}
		);
	}

	//Eh.
	return false;
}

/*
*	Creates a custom material asset widget. Returns if there was a change.
*/
FORCE_INLINE NO_DISCARD bool MaterialAssetWidget
(
	const char *const RESTRICT label,
	Component *const RESTRICT component,
	Entity *const RESTRICT entity,
	const ComponentEditableField &editable_field,
	AssetPointer<MaterialAsset> *const RESTRICT asset) NOEXCEPT
{
	char buffer[256];
	sprintf_s(buffer, "%s: %s", label, (*asset)->_Header._AssetName.Data());

	if (ImGui::Selectable(buffer))
	{
		CatalystEditorSystem::Instance->GetEditorContentBrowser()->Request
		(
			"Select Material",
			MaterialAsset::TYPE_IDENTIFIER,
			component,
			entity,
			&editable_field,
			asset,
			[](Component *const RESTRICT component, Entity *const RESTRICT entity, const ComponentEditableField *const RESTRICT editable_field, Asset *const RESTRICT asset, void *const RESTRICT user_data)
			{
				AssetPointer<MaterialAsset> *const RESTRICT material_asset{ static_cast<AssetPointer<MaterialAsset> *const RESTRICT>(user_data) };

				component->PreEditableFieldChange(entity, *editable_field);
				(*material_asset) = AssetPointer<MaterialAsset>((MaterialAsset *const RESTRICT)asset);
				component->PostEditableFieldChange(entity, *editable_field);
			}
		);
	}

	//Eh.
	return false;
}

/*
*	Creates a custom model asset widget. Returns if there was a change.
*/
FORCE_INLINE NO_DISCARD bool ModelAssetWidget
(
	const char *const RESTRICT label,
	Component *const RESTRICT component,
	Entity *const RESTRICT entity,
	const ComponentEditableField &editable_field,
	AssetPointer<ModelAsset> *const RESTRICT model_asset
) NOEXCEPT
{
	char buffer[256];
	sprintf_s(buffer, "%s: %s", label, (*model_asset)->_Header._AssetName.Data());

	if (ImGui::Selectable(buffer))
	{
		CatalystEditorSystem::Instance->GetEditorContentBrowser()->Request
		(
			"Select Model",
			ModelAsset::TYPE_IDENTIFIER,
			component,
			entity,
			&editable_field,
			model_asset,
			[](Component *const RESTRICT component, Entity *const RESTRICT entity, const ComponentEditableField *const RESTRICT editable_field, Asset *const RESTRICT asset, void* const RESTRICT user_data)
			{
				AssetPointer<ModelAsset> *const RESTRICT model_asset{ static_cast<AssetPointer<ModelAsset> *const RESTRICT>(user_data) };

				component->PreEditableFieldChange(entity, *editable_field);
				(*model_asset) = AssetPointer<ModelAsset>((ModelAsset *const RESTRICT)asset);
				component->PostEditableFieldChange(entity, *editable_field);
			}
		);
	}

	//Eh.
	return false;
}

/*
*	Creates a custom sound asset widget. Returns if there was a change.
*/
FORCE_INLINE NO_DISCARD bool SoundAssetWidget
(
	const char *const RESTRICT label,
	Component *const RESTRICT component,
	Entity *const RESTRICT entity,
	const ComponentEditableField &editable_field,
	AssetPointer<SoundAsset> *const RESTRICT sound_asset
) NOEXCEPT
{
	char buffer[256];
	sprintf_s(buffer, "%s: %s", label, (*sound_asset) ? (*sound_asset)->_Header._AssetName.Data() : "None");

	if (ImGui::Selectable(buffer))
	{
		CatalystEditorSystem::Instance->GetEditorContentBrowser()->Request
		(
			"Select Sound",
			SoundAsset::TYPE_IDENTIFIER,
			component,
			entity,
			&editable_field,
			sound_asset,
			[](Component *const RESTRICT component, Entity *const RESTRICT entity, const ComponentEditableField *const RESTRICT editable_field, Asset *const RESTRICT asset, void *const RESTRICT user_data)
			{
				AssetPointer<SoundAsset> *const RESTRICT model_asset{ static_cast<AssetPointer<SoundAsset> *const RESTRICT>(user_data) };

				component->PreEditableFieldChange(entity, *editable_field);
				(*model_asset) = AssetPointer<SoundAsset>((SoundAsset *const RESTRICT)asset);
				component->PostEditableFieldChange(entity, *editable_field);
			}
		);
	}

	//Eh.
	return false;
}

/*
*	Creates a custom widget for stuff like position/rotation/scale.
*	Returns if there was a change.
*/
FORCE_INLINE bool TransformWidgetFloat
(
	const char *const RESTRICT label,
	Component *const RESTRICT component,
	Entity *const RESTRICT entity,
	const ComponentEditableField &editable_field,
	float32 *const RESTRICT data,
	const uint64 data_size,
	const float32 reset_value,
	const float32 drag_speed,
	const char *const RESTRICT format = nullptr
) NOEXCEPT
{
	//Remember if there was a change.
	bool changed{ false };

	//Push the ID.
	ImGui::PushID(label);

	//Set the text on it's own line.
	ImGui::Text(label);

	//Set up the widget.
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
			component->PreEditableFieldChange(entity, editable_field);
			data[0] = reset_value;
			component->PostEditableFieldChange(entity, editable_field);

			changed = true;
		}
		ImGui::PopStyleColor(3);

		ImGui::SameLine();

		float32 value{ data[0] };

		if (ImGui::DragFloat("##X", &value, drag_speed, 0.0f, 0.0f, format ? format : "%.3f"))
		{
			component->PreEditableFieldChange(entity, editable_field);
			data[0] = value;
			component->PostEditableFieldChange(entity, editable_field);

			changed = true;
		}

		ImGui::PopItemWidth();

		if (data_size >= 2)
		{
			ImGui::SameLine();
		}
	}
	
	if (data_size >= 2)
	{
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.7f, 0.3f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.8f, 0.3f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.7f, 0.2f, 1.0f));
		if (ImGui::Button("Y", button_size))
		{
			component->PreEditableFieldChange(entity, editable_field);
			data[1] = reset_value;
			component->PostEditableFieldChange(entity, editable_field);

			changed = true;
		}

		ImGui::PopStyleColor(3);

		ImGui::SameLine();

		float32 value{ data[1] };

		if (ImGui::DragFloat("##Y", &value, drag_speed, 0.0f, 0.0f, format ? format : "%.3f"))
		{
			component->PreEditableFieldChange(entity, editable_field);
			data[1] = value;
			component->PostEditableFieldChange(entity, editable_field);

			changed = true;
		}

		ImGui::PopItemWidth();

		if (data_size >= 3)
		{
			ImGui::SameLine();
		}
	}

	if (data_size >= 3)
	{
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.25f, 0.8f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.1f, 0.35f, 0.9f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.25f, 0.8f, 1.0f));
		if (ImGui::Button("Z", button_size))
		{
			component->PreEditableFieldChange(entity, editable_field);
			data[2] = reset_value;
			component->PostEditableFieldChange(entity, editable_field);

			changed = true;
		}

		ImGui::PopStyleColor(3);

		ImGui::SameLine();

		float32 value{ data[2] };

		if (ImGui::DragFloat("##Z", &value, drag_speed, 0.0f, 0.0f, format ? format : "%.3f"))
		{
			component->PreEditableFieldChange(entity, editable_field);
			data[2] = value;
			component->PostEditableFieldChange(entity, editable_field);

			changed = true;
		}

		ImGui::PopItemWidth();
	}

	ImGui::PopStyleVar();

	ImGui::PopID();

	return changed;
}

/*
*	Creates a custom widget for stuff like position/rotation/scale.
*	Returns if there was a change.
*/
FORCE_INLINE bool TransformWidgetUint32
(
	const char *const RESTRICT label,
	Component *const RESTRICT component,
	Entity *const RESTRICT entity,
	const ComponentEditableField &editable_field,
	uint32 *const RESTRICT data,
	const uint64 data_size,
	const uint32 reset_value
) NOEXCEPT
{
	//Remember if there was a change.
	bool changed{ false };

	//Push the ID.
	ImGui::PushID(label);

	//Set the text on it's own line.
	ImGui::Text(label);

	//Set up the widget.
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
			component->PreEditableFieldChange(entity, editable_field);
			data[0] = reset_value;
			component->PostEditableFieldChange(entity, editable_field);

			changed = true;
		}
		ImGui::PopStyleColor(3);

		ImGui::SameLine();

		int32 value{ static_cast<int32>(data[0]) };

		if (ImGui::DragInt("##X", &value))
		{
			component->PreEditableFieldChange(entity, editable_field);
			data[0] = value;
			component->PostEditableFieldChange(entity, editable_field);

			changed = true;
		}

		ImGui::PopItemWidth();

		if (data_size >= 2)
		{
			ImGui::SameLine();
		}
	}

	if (data_size >= 2)
	{
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.7f, 0.3f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.8f, 0.3f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.7f, 0.2f, 1.0f));
		if (ImGui::Button("Y", button_size))
		{
			component->PreEditableFieldChange(entity, editable_field);
			data[1] = reset_value;
			component->PostEditableFieldChange(entity, editable_field);

			changed = true;
		}

		ImGui::PopStyleColor(3);

		ImGui::SameLine();

		int32 value{ static_cast<int32>(data[1]) };

		if (ImGui::DragInt("##Y", &value))
		{
			component->PreEditableFieldChange(entity, editable_field);
			data[1] = value;
			component->PostEditableFieldChange(entity, editable_field);

			changed = true;
		}

		ImGui::PopItemWidth();

		if (data_size >= 3)
		{
			ImGui::SameLine();
		}
	}

	if (data_size >= 3)
	{
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.25f, 0.8f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.1f, 0.35f, 0.9f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.25f, 0.8f, 1.0f));
		if (ImGui::Button("Z", button_size))
		{
			component->PreEditableFieldChange(entity, editable_field);
			data[2] = reset_value;
			component->PostEditableFieldChange(entity, editable_field);

			changed = true;
		}

		ImGui::PopStyleColor(3);

		ImGui::SameLine();

		int32 value{ static_cast<int32>(data[2]) };

		if (ImGui::DragInt("##Z", &value))
		{
			component->PreEditableFieldChange(entity, editable_field);
			data[2] = value;
			component->PostEditableFieldChange(entity, editable_field);

			changed = true;
		}

		ImGui::PopItemWidth();
	}

	ImGui::PopStyleVar();

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
*	Updates the editor level system.
*/
void EditorLevelSystem::Update() NOEXCEPT
{
	//Only update if no window is hovered.
	if (ImGui::GetIO().WantCaptureMouse)
	{
		return;
	}
	 
	//Cache the input state.
	const MouseState *const RESTRICT mouse_state{ InputSystem::Instance->GetMouseState(InputLayer::GAME) };

	//If the left mouse button is clicked, try to select an entity. (:
	if (mouse_state->_Left == ButtonState::PRESSED)
	{
		Ray ray;

		ray.SetOrigin(RenderingSystem::Instance->GetCameraSystem()->GetCurrentCamera()->GetWorldTransform().GetAbsolutePosition());
		ray.SetDirection(RenderingUtilities::CalculateRayDirectionFromScreenCoordinate(Vector2<float32>(mouse_state->_CurrentX, mouse_state->_CurrentY)));

		//Perform the selection!
		Entity *RESTRICT selected_entity;

		{
			const ArrayProxy<Entity *RESTRICT> entities_proxy{ _Entities };
			selected_entity = Components::Select(ray, entities_proxy);
		}
		
		if (selected_entity)
		{
			//Find the index.
			for (uint64 entity_index{ 0 }; entity_index < _Entities.Size(); ++entity_index)
			{
				if (_Entities[entity_index] == selected_entity)
				{
					SetSelectedEntityIndex(entity_index);

					break;
				}
			}
		}

		else
		{
			SetSelectedEntityIndex(UINT64_MAXIMUM);
		}
	}
}

/*
*	Creates an entity.
*/
void EditorLevelSystem::CreateEntity() NOEXCEPT
{
	//Add a new entity.
	_Entities.Emplace();
	Entity *RESTRICT &new_entity{ _Entities.Back() };

	_EditorEntityData.Emplace();
	EditorEntityData &new_editor_entity_data{ _EditorEntityData.Back() };

	//Generate a name.
	GenerateEntityName(new_editor_entity_data._Name.Data(), new_editor_entity_data._Name.Size());

	//Generate the entity identifier.
	GenerateEntityIdentifier(&new_editor_entity_data._Identifier);

	//Add components (entities created in the editor always have a world transform component).
	StaticArray<ComponentInitializationData *RESTRICT, 1> component_configurations;

	{
		WorldTransformInitializationData *const RESTRICT data{ WorldTransformComponent::Instance->AllocateInitializationData() };

		data->_WorldTransform = WorldTransform();

		component_configurations[0] = data;
	}

	//Create the entity!
	new_entity = EntitySystem::Instance->CreateEntity(ArrayProxy<ComponentInitializationData *RESTRICT>(component_configurations));

	//The entity is now the selected entity. (:
	SetSelectedEntityIndex(_Entities.LastIndex());
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

	//Write the JSON to a file for debug purposes..
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
*	Returns the selected entity.
*/
NO_DISCARD Entity *const RESTRICT EditorLevelSystem::GetSelectedEntity() NOEXCEPT
{
	if (_SelectedEntityIndex != UINT64_MAXIMUM && _SelectedEntityIndex < _Entities.Size())
	{
		return _Entities[_SelectedEntityIndex];
	}

	else
	{
		return nullptr;
	}
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

	for (uint64 entity_index{ 0 }; entity_index < _Entities.Size(); ++entity_index)
	{
		//Cache the entity and entity editor data.
		Entity *const RESTRICT entity{ _Entities[entity_index] };
		EditorEntityData &editor_entity_data{ _EditorEntityData[entity_index] };

		//Cache the entity entry.
		nlohmann::json &entity_entry{ entities[editor_entity_data._Name.Data()] };

		//Serialize the entity.
		EntitySerialization::SerializeToJSON(editor_entity_data, entity_entry, entity);

		//Serialize the editor data.
		nlohmann::json &editor_data_entry{ entity_entry["EditorData"] };

		//Serialize the rotation.
		{
			nlohmann::json &rotation_entry{ editor_data_entry["Rotation"] };

			rotation_entry["Roll"] = BaseMath::RadiansToDegrees(editor_entity_data._Rotation._Roll);
			rotation_entry["Yaw"] = BaseMath::RadiansToDegrees(editor_entity_data._Rotation._Yaw);
			rotation_entry["Pitch"] = BaseMath::RadiansToDegrees(editor_entity_data._Rotation._Pitch);
		}

		//Serialize the hash string data.
		if (!editor_entity_data._HashStringData.Empty())
		{
			nlohmann::json &hash_string_data_entry{ editor_data_entry["HashStringData"] };

			for (const EditorEntityData::HashStringData &hash_string_data : editor_entity_data._HashStringData)
			{
				nlohmann::json &_hash_string_data_entry{ hash_string_data_entry.emplace_back() };

				_hash_string_data_entry["String"] = hash_string_data._String.Data();
				_hash_string_data_entry["ComponentIdentifier"] = hash_string_data._ComponentIdentifier.operator size_t();
				_hash_string_data_entry["EditableFieldIdentifier"] = hash_string_data._EditableFieldIdentifier.operator size_t();
			}
		}

		//Serialize whether or not this entity should be serialized.
		editor_data_entry["Serialize"] = editor_entity_data._Serialize;

		//Serialize the identifier.
		entity_entry["Identifier"] = editor_entity_data._Identifier;

		//Update the level statistics from the world transform component.
		{
			//Cache the world transform instance data.
			const WorldTransformInstanceData &world_transform_instance_data{ WorldTransformComponent::Instance->InstanceData(entity) };

			//Calculate the distance from the center.
			const float32 distance_from_center{ Vector3<float32>::LengthXZ(world_transform_instance_data._CurrentWorldTransform.GetAbsolutePosition()) };

			//Update the radius.
			level_statistics._Radius = BaseMath::Maximum<float32>(level_statistics._Radius, distance_from_center);
		}

		//Update level statistics from the static model component.
		if (StaticModelComponent::Instance->Has(entity))
		{
			//Cache the static model instance data.
			const StaticModelInstanceData &static_model_instance_data{ StaticModelComponent::Instance->InstanceData(entity) };

			//Cache the absolute axis aligned bounding box.
			const AxisAlignedBoundingBox3D absolute_axis_aligned_bounding_box{ static_model_instance_data._WorldSpaceAxisAlignedBoundingBox.GetAbsoluteAxisAlignedBoundingBox() };

			//Expand the axis aligned bounding box.
			level_statistics._AxisAlignedBoundingBox.Expand(absolute_axis_aligned_bounding_box);
		
			//Calculate the distance from the center.
			float32 distance_from_center{ Vector3<float32>::LengthXZ(AxisAlignedBoundingBox3D::CalculateCenter(absolute_axis_aligned_bounding_box)) };

			//Cache the half extents.
			const Vector3<float32> half_extents{ absolute_axis_aligned_bounding_box.Dimensions() * 0.5f };

			//Update the radius.
			level_statistics._Radius = BaseMath::Maximum<float32>(level_statistics._Radius, distance_from_center + BaseMath::Maximum<float32>(half_extents._X, half_extents._Z));
		}
	}

	//Write the level statistics.
	nlohmann::json &level_statistics_entry{ JSON["LevelStatistics"] };

	{
		nlohmann::json &axis_aligned_bounding_box_entry{ level_statistics_entry["AxisAlignedBoundingBox"] };

		{
			nlohmann::json &minimum_entry{ axis_aligned_bounding_box_entry["Minimum"] };

			minimum_entry["X"] = level_statistics._AxisAlignedBoundingBox._Minimum._X;
			minimum_entry["Y"] = level_statistics._AxisAlignedBoundingBox._Minimum._Y;
			minimum_entry["Z"] = level_statistics._AxisAlignedBoundingBox._Minimum._Z;
		}

		{
			nlohmann::json &maximum_entry{ axis_aligned_bounding_box_entry["Maximum"] };

			maximum_entry["X"] = level_statistics._AxisAlignedBoundingBox._Maximum._X;
			maximum_entry["Y"] = level_statistics._AxisAlignedBoundingBox._Maximum._Y;
			maximum_entry["Z"] = level_statistics._AxisAlignedBoundingBox._Maximum._Z;
		}
	}

	level_statistics_entry["Radius"] = level_statistics._Radius;

	//Write the entity links.
	nlohmann::json &entity_links_entry{ JSON["EntityLinks"] };

	for (uint64 entity_index{ 0 }; entity_index < _Entities.Size(); ++entity_index)
	{
		//Cache the entity identifier.
		const uint64 entity_identifier{ _EditorEntityData[entity_index]._Identifier };

		//Go through the links.
		for (const uint64 link : _EditorEntityData[entity_index]._Links)
		{
			//Add the new entry.
			nlohmann::json &link_entry{ entity_links_entry.emplace_back() };

			//Set the from/to links.
			link_entry["From"] = entity_identifier;
			link_entry["To"] = link;
		}
	}
}

/*
*	Loads a level internally.
*/
void EditorLevelSystem::LoadLevelInternal(const nlohmann::json &JSON) NOEXCEPT
{
	//Destroy all entities.
	EntitySystem::Instance->DestroyAllEntities();

	//Clear the entities.
	_Entities.Clear();

	//Clear the editor entity data.
	_EditorEntityData.Clear();

	//Reset the selected entity.
	SetSelectedEntityIndex(UINT64_MAXIMUM);

	//Set up the stream archive.
	StreamArchive stream_archive;

	//Cache the entities JSON object.
	const nlohmann::json &entities{ JSON["Entities"] };

	//Allocate the necessary amount of memory.
	_Entities.Reserve(entities.size());
	_EditorEntityData.Reserve(entities.size());

	//Iterate through all entities.
	for (auto entity_iterator{ entities.begin() }; entity_iterator != entities.end(); ++entity_iterator)
	{
		//Add the new entity.
		_Entities.Emplace();
		Entity *RESTRICT &new_entity{ _Entities.Back() };

		//Add a new editor entity data.
		_EditorEntityData.Emplace();
		EditorEntityData &new_editor_entity_data{ _EditorEntityData.Back() };

		//Set the name.
		new_editor_entity_data._Name = entity_iterator.key().c_str();

		//Cache the entity entry.
		const nlohmann::json &entity_entry{ *entity_iterator };

		//Remember the stream archive position.
		uint64 stream_archive_position{ stream_archive.Size() };

		//Serialize to the stream archive.
		EntitySerialization::SerializeToStreamArchive(entity_entry, &stream_archive);

		//Now deserialize!
		new_entity = EntitySerialization::DeserializeFromStreamArchive(stream_archive, &stream_archive_position, nullptr);

		//Deserialize the editor data.
		const nlohmann::json &editor_data_entry{ entity_entry["EditorData"] };

		//Deserialize the rotation.
		{
			const nlohmann::json &rotation_entry{ editor_data_entry["Rotation"] };

			new_editor_entity_data._Rotation._Roll = BaseMath::DegreesToRadians(rotation_entry["Roll"]);
			new_editor_entity_data._Rotation._Yaw = BaseMath::DegreesToRadians(rotation_entry["Yaw"]);
			new_editor_entity_data._Rotation._Pitch = BaseMath::DegreesToRadians(rotation_entry["Pitch"]);
		}

		//Deserialize the identifier.
		new_editor_entity_data._Identifier = entity_entry["Identifier"];

		//Deserialize the hash string data.
		if (editor_data_entry.contains("HashStringData"))
		{
			const nlohmann::json &hash_string_data_entry{ editor_data_entry["HashStringData"] };
			const uint64 hash_string_data_size{ hash_string_data_entry.size() };
			new_editor_entity_data._HashStringData.Reserve(hash_string_data_size);

			for (const nlohmann::json &_hash_string_data_entry : hash_string_data_entry)
			{
				new_editor_entity_data._HashStringData.Emplace();
				EditorEntityData::HashStringData &new_hash_string_data{ new_editor_entity_data._HashStringData.Back() };

				new_hash_string_data._String = _hash_string_data_entry["String"].get<std::string>().c_str();
				new_hash_string_data._ComponentIdentifier = _hash_string_data_entry["ComponentIdentifier"].get<uint64>();
				new_hash_string_data._EditableFieldIdentifier = _hash_string_data_entry["EditableFieldIdentifier"].get<uint64>();
			}
		}

		//Deserialize whether or not this entity should be serialized.
		if (editor_data_entry.contains("Serialize"))
		{
			new_editor_entity_data._Serialize = editor_data_entry["Serialize"];
		}

		else
		{
			new_editor_entity_data._Serialize = true;
		}

		//Update the current entity identifier, since it's supposed to be a long chain of identifiers.
		_CurrentEntityIdentifier = new_editor_entity_data._Identifier;
	}

	//Read the entity links.
	const nlohmann::json &entity_links_entry{ JSON["EntityLinks"] };

	for (const nlohmann::json &entity_link : entity_links_entry)
	{
		const uint64 from{ entity_link["From"] };
		const uint64 to{ entity_link["To"] };

		const uint64 from_index{ EntityIdentifierToIndex(from) };

		_EditorEntityData[from_index]._Links.Emplace(to);
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
*	Generates an entity identifier.
*/
void EditorLevelSystem::GenerateEntityIdentifier(uint64 *const RESTRICT identifier) NOEXCEPT
{
	_CurrentEntityIdentifier = HashAlgorithms::MurmurHash64(&_CurrentEntityIdentifier, sizeof(uint64));
	*identifier = _CurrentEntityIdentifier;
}

/*
*	Sets the selected entity index.
*/
void EditorLevelSystem::SetSelectedEntityIndex(const uint64 selected_entity_index) NOEXCEPT
{
	//If we had a previous selected entity, clear the flag.
	if (_SelectedEntityIndex != UINT64_MAXIMUM && _SelectedEntityIndex < _Entities.Size())
	{
		CLEAR_BIT(_Entities[_SelectedEntityIndex]->_Flags, Entity::Flags::EDITOR_SELECTED);
	}

	//Set the new selected entity index.
	_SelectedEntityIndex = selected_entity_index;

	//Set the flag on the new selected entity.
	if (_SelectedEntityIndex != UINT64_MAXIMUM)
	{
		SET_BIT(_Entities[_SelectedEntityIndex]->_Flags, Entity::Flags::EDITOR_SELECTED);
	}
}

/*
*	The top right window update.
*/
NO_DISCARD bool EditorLevelSystem::TopRightWindowUpdate(const Vector2<float32> minimum, const Vector2<float32> maximum) NOEXCEPT
{
	//Begin the window.
	ImGuiSystem::BeginWindowParameters begin_window_parameters;

	begin_window_parameters._Name = "Editor Level Top Right";
	begin_window_parameters._Minimum = minimum;
	begin_window_parameters._Maximum = maximum;
	begin_window_parameters._ShowTitleBar = false;

	ImGuiSystem::Instance->BeginWindow(begin_window_parameters);

	//List all entities.
	for (uint64 entity_index{ 0 }; entity_index < _Entities.Size(); ++entity_index)
	{
		//Cache the entity and entity editor data.
		Entity *const RESTRICT entity{ _Entities[entity_index] };
		EditorEntityData &editor_entity_data{ _EditorEntityData[entity_index] };

		//Calculate the name length.
		const uint64 name_length{ StringUtilities::StringLength(editor_entity_data._Name.Data()) };

		if (ImGui::Selectable(name_length == 0 ? "EMPTY" : editor_entity_data._Name.Data(), _SelectedEntityIndex == entity_index))
		{
			SetSelectedEntityIndex(entity_index);
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
	//Cache the keyboard state.
	const KeyboardState *const RESTRICT keyboard_state{ InputSystem::Instance->GetKeyboardState(InputLayer::USER_INTERFACE) };

	//Update the Gizmo mode.
	if (!ImGui::GetIO().WantTextInput)
	{
		if (keyboard_state->GetButtonState(KeyboardButton::ONE) == ButtonState::PRESSED)
		{
			_CurrentGizmoMode = GizmoMode::TRANSLATE;
		}

		else if (keyboard_state->GetButtonState(KeyboardButton::TWO) == ButtonState::PRESSED)
		{
			_CurrentGizmoMode = GizmoMode::ROTATE;
		}

		else if (keyboard_state->GetButtonState(KeyboardButton::THREE) == ButtonState::PRESSED)
		{
			_CurrentGizmoMode = GizmoMode::SCALE;
		}
	}

	//Begin the window.
	ImGuiSystem::BeginWindowParameters begin_window_parameters;

	begin_window_parameters._Name = "Editor Level Bottom Right";
	begin_window_parameters._Minimum = minimum;
	begin_window_parameters._Maximum = maximum;
	begin_window_parameters._ShowTitleBar = false;
	begin_window_parameters._EnableMenuBar = true;

	ImGuiSystem::Instance->BeginWindow(begin_window_parameters);

	//Set up stuff for the selected entity.
	if (_SelectedEntityIndex != UINT64_MAXIMUM && TEST_BIT(_Entities[_SelectedEntityIndex]->_Flags, Entity::Flags::INITIALIZED))
	{
		//Cache the selected entity and entity editor data.
		Entity *const RESTRICT selected_entity{ _Entities[_SelectedEntityIndex] };
		EditorEntityData &selected_editor_entity_data{ _EditorEntityData[_SelectedEntityIndex] };

		//Add the menu bar.
		{
			ImGui::BeginMenuBar();

			if (ImGui::BeginMenu("Add Component"))
			{
				for (uint64 component_index{ 0 }; component_index < Components::Size(); ++component_index)
				{
					//Cache the component.
					Component *const RESTRICT component{ Components::At(component_index) };

					if (component->Has(selected_entity))
					{
						continue;
					}

					if (ImGui::MenuItem(component->_Name))
					{
						ComponentInitializationData *const RESTRICT initialization_data{ Components::AllocateInitializationData(component) };
						EntitySystem::Instance->AddComponentToEntity(selected_entity, initialization_data);

						//For any hash string editable fields this component has, add a hash string data to the editor entity data as well.
						for (const ComponentEditableField &editable_field : component->EditableFields())
						{
							if (editable_field._Type == ComponentEditableField::Type::HASH_STRING)
							{
								selected_editor_entity_data._HashStringData.Emplace();
								EditorEntityData::HashStringData &new_hash_string_data{ selected_editor_entity_data._HashStringData.Back() };

								new_hash_string_data._ComponentIdentifier = component->_Identifier;
								new_hash_string_data._EditableFieldIdentifier = editable_field._Identifier;
							}
						}
					}
				}
				
				ImGui::EndMenu();
			}

			//End the menu.
			ImGui::EndMenuBar();
		}

		//Add a text input for the name.
		TextInputWidget("Name", selected_editor_entity_data._Name.Data(), selected_editor_entity_data._Name.Size());

		//Add a load button.
		bool wants_to_load{ false };
		DynamicString load_file_path;

		if (ImGui::Button("Load"))
		{
			if (File::BrowseForFile(false, &load_file_path, "*.Entity"))
			{
				wants_to_load = true;
			}
		}

		//Add a save button.
		ImGui::SameLine();

		if (ImGui::Button("Save"))
		{
			DynamicString chosen_file;

			if (File::BrowseForFile(true, &chosen_file, "*.Entity"))
			{
				SaveEntity(chosen_file.Data(), selected_entity, selected_editor_entity_data);
			}
		}

		//Add a delete button.
		bool should_delete{ false };

		ImGui::SameLine();

		if (ImGui::Button("Delete"))
		{
			should_delete = true;
		}

		if (keyboard_state->GetButtonState(KeyboardButton::Delete) == ButtonState::PRESSED)
		{
			should_delete = true;
		}

		//Add a duplicate button.
		bool should_duplicate{ false };

		ImGui::SameLine();

		if (ImGui::Button("Duplicate"))
		{
			should_duplicate = true;
		}

		if ((keyboard_state->GetButtonState(KeyboardButton::LeftControl) == ButtonState::PRESSED || keyboard_state->GetButtonState(KeyboardButton::LeftControl) == ButtonState::PRESSED_HELD) && keyboard_state->GetButtonState(KeyboardButton::D) == ButtonState::PRESSED)
		{
			should_duplicate = true;
		}

		//Add a checkbox for whether or not to serialize this entity.
		ImGui::Checkbox("Serialize", &selected_editor_entity_data._Serialize);

		//Add widgets for entity links.
		if (ImGui::CollapsingHeader("Entity Links"))
		{
			//Add the "Add" button.
			if (ImGui::Button("Add"))
			{
				selected_editor_entity_data._Links.Emplace(0);
			}

			//List all the current links.
			for (uint64 link_index{ 0 }; link_index < selected_editor_entity_data._Links.Size();)
			{
				//Cache the link.
				uint64 &link{ selected_editor_entity_data._Links[link_index] };

				//Retrieve the linked entity index.
				const uint64 linked_entity_index{ EntityIdentifierToIndex(link) };

				//Figure out the preview value.
				const char *RESTRICT preview_value{ nullptr };

				if (linked_entity_index != UINT64_MAXIMUM)
				{
					preview_value = _EditorEntityData[linked_entity_index]._Name.Data();
				}

				else
				{
					preview_value = "NONE";
				}

				//Figure out the label.
				char label_buffer[64];
				sprintf_s(label_buffer, "##%llu", link);

				if (ImGui::BeginCombo(label_buffer, preview_value))
				{
					//Cache if an entry is selected.
					bool selected{ false };

					//Add the remove selected.
					if (ImGui::Selectable("REMOVE", &selected))
					{
						selected_editor_entity_data._Links.EraseAt<true>(link_index);

						ImGui::EndCombo();

						continue;
					}

					//List all the entities.
					for (uint64 entity_index{ 0 }; entity_index < _Entities.Size(); ++entity_index)
					{
						//Disregard the current entity.
						if (entity_index == _SelectedEntityIndex)
						{
							continue;
						}

						//Disregard entities that are already linked.
						if (AreEntitiesLinked(_SelectedEntityIndex, entity_index))
						{
							continue;
						}

						//Link the entities if requested.
						if (ImGui::Selectable(_EditorEntityData[entity_index]._Name.Data(), &selected))
						{
							link = _EditorEntityData[entity_index]._Identifier;
						}
					}

					ImGui::EndCombo();
				}

				//Update the link index.
				++link_index;
			}
		}

		//Add widgets for the world transform component.
		if (ImGui::CollapsingHeader(WorldTransformComponent::Instance->_Name))
		{
			//Retrieve the editable field.
			const ComponentEditableField &editable_field{ WorldTransformComponent::Instance->EditableFields()[0]};

			ASSERT(editable_field._Type == ComponentEditableField::Type::WORLD_TRANSFORM, "What happened here?");

			//Retrieve the world transform.
			WorldTransform *const RESTRICT world_transform{ WorldTransformComponent::Instance->EditableFieldData<WorldTransform>(selected_entity, editable_field) };

			//Add a header.
			ImGui::Text("World Transform");

			//Add position widget.
			{
				//Retrieve the position.
				Vector3<float32> position{ world_transform->GetAbsolutePosition() };

				//Add the transform widget.
				const bool changed
				{
					TransformWidgetFloat
					(
						"Position",
						WorldTransformComponent::Instance,
						selected_entity,
						editable_field,
						&position._X,
						3,
						0.0f,
						0.01f,
						"%.2f"
					)
				};

				//Update the position if anything changed.
				if (changed)
				{
					world_transform->SetAbsolutePosition(position);
				}
			}

			//Add rotation widget.
			{
				//Retrieve the rotation.
				EulerAngles rotation{ selected_editor_entity_data._Rotation };

				//Convert to degrees.
				for (uint8 i{ 0 }; i < 3; ++i)
				{
					rotation[i] = BaseMath::RadiansToDegrees(rotation[i]);
				}

				//Add the transform widget.
				const bool changed
				{
					TransformWidgetFloat
					(
						"Rotation",
						WorldTransformComponent::Instance,
						selected_entity,
						editable_field,
						&rotation._Roll,
						3,
						0.0f,
						0.1f,
						"%.1f"
					)
				};

				//Update the rotation if anything changed.
				if (changed)
				{
					//Convert to radians.
					for (uint8 i{ 0 }; i < 3; ++i)
					{
						rotation[i] = BaseMath::DegreesToRadians(rotation[i]);
					}

					selected_editor_entity_data._Rotation = rotation;
					world_transform->SetRotation(rotation);
				}
			}

			//Add scale widget.
			{
				//Retrieve the scale.
				Vector3<float32> scale{ world_transform->GetScale() };

				//Add the transform widget.
				const bool changed
				{
					TransformWidgetFloat
					(
						"Scale",
						WorldTransformComponent::Instance,
						selected_entity,
						editable_field,
						&scale._X,
						3,
						1.0f,
						0.01f,
						"%.2f"
					)
				};

				//Update the scale if anything changed.
				if (changed)
				{
					world_transform->SetScale(scale);
				}
			}
		}

		//Add a Gizmo for the world transform!
		{
			ASSERT(WorldTransformComponent::Instance->Has(selected_entity), "Entity doesn't have world transform component!");
			WorldTransform *const RESTRICT world_transform{ WorldTransformComponent::Instance->EditableFieldData<WorldTransform>(selected_entity, WorldTransformComponent::Instance->EditableFields()[0]) };

			Matrix4x4 transformation_matrix;

			{
				Vector3<float32> position{ world_transform->GetAbsolutePosition() };
				EulerAngles rotation{ selected_editor_entity_data._Rotation };
				Vector3<float32> scale{ world_transform->GetScale() };

				//Rotation reported in degrees, so convert.
				for (uint8 i{ 0 }; i < 3; ++i)
				{
					rotation[i] = BaseMath::RadiansToDegrees(rotation[i]);
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
					operation = ImGuizmo::OPERATION::SCALE;

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
					rotation[i] = BaseMath::DegreesToRadians(rotation[i]);
				}

				//If the user is holding in the left control key, then snap to... Hrrm, 1 meter. TODO: Expose this as a setting maybe?
				if (keyboard_state->GetButtonState(KeyboardButton::LeftControl) == ButtonState::PRESSED
					|| keyboard_state->GetButtonState(KeyboardButton::LeftControl) == ButtonState::PRESSED_HELD)
				{
					for (uint8 i{ 0 }; i < 3; ++i)
					{
						position[i] = static_cast<float32>(static_cast<int32>(position[i]));
					}
				}

				world_transform->SetAbsolutePosition(position);
				selected_editor_entity_data._Rotation = rotation;
				world_transform->SetRotation(rotation);
				world_transform->SetScale(scale);
			}
		}

		/*
		*	Alright, this is crude, but right now, just iterate over all components,
		*	check if this entity has that, and expose the editable fields.
		*/
		for (uint64 component_index{ 0 }; component_index < Components::Size(); ++component_index)
		{
			//Cache the component.
			Component *const RESTRICT component{ Components::At(component_index) };

			//Ignore editor data and world transform components, as they are always implicitly added.
			if (component == WorldTransformComponent::Instance)
			{
				continue;
			}

			if (component->Has(selected_entity))
			{
				if (ImGui::CollapsingHeader(component->_Name))
				{
					//Add widgets for all editable fields.
					for (const ComponentEditableField &editable_field : component->EditableFields())
					{
						switch (editable_field._Type)
						{
							case ComponentEditableField::Type::BOOL:
							{
								bool *const RESTRICT value{ component->EditableFieldData<bool>(selected_entity, editable_field) };

								BoolWidget
								(
									editable_field._Name,
									component,
									selected_entity,
									editable_field,
									value
								);

								break;
							}

							case ComponentEditableField::Type::COLOR:
							{
								Vector3<float32> *const RESTRICT value{ component->EditableFieldData<Vector3<float32>>(selected_entity, editable_field) };

								ColorWidget
								(
									editable_field._Name,
									component,
									selected_entity,
									editable_field,
									value
								);

								break;
							}

							case ComponentEditableField::Type::FLOAT:
							{
								float32 *const RESTRICT value{ component->EditableFieldData<float32>(selected_entity, editable_field) };

								TransformWidgetFloat
								(
									editable_field._Name,
									component,
									selected_entity,
									editable_field,
									value,
									1,
									0.0f,
									0.01f
								);

								break;
							}

							case ComponentEditableField::Type::ENUMERATION:
							{
								Enumeration *const RESTRICT enumeration{ component->EditableFieldData<Enumeration>(selected_entity, editable_field) };

								EnumerationWidget
								(
									editable_field._Name,
									component,
									selected_entity,
									editable_field,
									enumeration
								);

								break;
							}

							case ComponentEditableField::Type::HASH_STRING:
							{
								//Find the hash string data.
								EditorEntityData::HashStringData *RESTRICT hash_string_data{ nullptr };

								for (EditorEntityData::HashStringData &_hash_string_data : selected_editor_entity_data._HashStringData)
								{
									if (_hash_string_data._ComponentIdentifier == component->_Identifier
										&& _hash_string_data._EditableFieldIdentifier == editable_field._Identifier)
									{
										hash_string_data = &_hash_string_data;

										break;
									}
								}

								ASSERT(hash_string_data, "Couldn't find hash string data!");

								HashString *const RESTRICT hash_string{ component->EditableFieldData<HashString>(selected_entity, editable_field) };

								HashStringWidget
								(
									editable_field._Name,
									component,
									selected_entity,
									editable_field,
									hash_string_data,
									hash_string
								);

								break;
							}

							case ComponentEditableField::Type::ANIMATED_MODEL_ASSET:
							{
								AssetPointer<AnimatedModelAsset> *const RESTRICT asset{ component->EditableFieldData<AssetPointer<AnimatedModelAsset>>(selected_entity, editable_field) };

								AnimatedModelAssetWidget
								(
									editable_field._Name,
									component,
									selected_entity,
									editable_field,
									asset
								);

								break;
							}

							case ComponentEditableField::Type::ANIMATION_ASSET:
							{
								AssetPointer<AnimationAsset> *const RESTRICT asset{ component->EditableFieldData<AssetPointer<AnimationAsset>>(selected_entity, editable_field) };

								AnimationAssetWidget
								(
									editable_field._Name,
									component,
									selected_entity,
									editable_field,
									asset
								);

								break;
							}

							case ComponentEditableField::Type::MATERIAL_ASSET:
							{
								AssetPointer<MaterialAsset> *const RESTRICT asset{ component->EditableFieldData<AssetPointer<MaterialAsset>>(selected_entity, editable_field) };

								MaterialAssetWidget
								(
									editable_field._Name,
									component,
									selected_entity,
									editable_field,
									asset
								);

								break;
							}

							case ComponentEditableField::Type::MODEL_ASSET:
							{
								AssetPointer<ModelAsset> *const RESTRICT asset{ component->EditableFieldData<AssetPointer<ModelAsset>>(selected_entity, editable_field) };

								ModelAssetWidget
								(
									editable_field._Name,
									component,
									selected_entity,
									editable_field,
									asset
								);

								break;
							}

							case ComponentEditableField::Type::SOUND_ASSET:
							{
								AssetPointer<SoundAsset> *const RESTRICT asset{ component->EditableFieldData<AssetPointer<SoundAsset>>(selected_entity, editable_field) };

								SoundAssetWidget
								(
									editable_field._Name,
									component,
									selected_entity,
									editable_field,
									asset
								);

								break;
							}

							case ComponentEditableField::Type::UINT32:
							case ComponentEditableField::Type::UINT64:
							{
								uint32 *const RESTRICT value{ component->EditableFieldData<uint32>(selected_entity, editable_field) };

								TransformWidgetUint32
								(
									editable_field._Name,
									component,
									selected_entity,
									editable_field,
									value,
									1,
									0
								);

								break;
							}

							case ComponentEditableField::Type::VECTOR2:
							{
								Vector2<float32> *const RESTRICT editable_field_data{ component->EditableFieldData<Vector2<float32>>(selected_entity, editable_field) };

								TransformWidgetFloat
								(
									editable_field._Name,
									component,
									selected_entity,
									editable_field,
									&editable_field_data->_X,
									2,
									0.0f,
									0.01f
								);

								break;
							}

							case ComponentEditableField::Type::VECTOR3:
							{
								Vector3<float32> *const RESTRICT editable_field_data{ component->EditableFieldData<Vector3<float32>>(selected_entity, editable_field) };

								TransformWidgetFloat
								(
									editable_field._Name,
									component,
									selected_entity,
									editable_field,
									&editable_field_data->_X,
									3,
									0.0f,
									0.01f
								);

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

		//Load an entity, if requrested.
		if (wants_to_load)
		{
			//Load the entity!
			LoadEntity(load_file_path.Data(), &_Entities[_SelectedEntityIndex], &_EditorEntityData[_SelectedEntityIndex]);
		}

		//Delete the entity, if requested.
		if (should_delete)
		{
			EntitySystem::Instance->DestroyEntity(selected_entity);
			_Entities.EraseAt<true>(_SelectedEntityIndex);
			_EditorEntityData.EraseAt<true>(_SelectedEntityIndex);
			SetSelectedEntityIndex(UINT64_MAXIMUM);
		}

		//Duplicate the entity, if requested.
		if (should_duplicate)
		{
			DuplicateSelectedEntity();
		}
	}

	//End the window.
	ImGui::End();

	//This window should always be shown.
	return true;
}

/*
*	Loads an entity.
*/
void EditorLevelSystem::LoadEntity(const char *const RESTRICT file_path, Entity *RESTRICT *const RESTRICT entity, EditorEntityData *const RESTRICT editor_data) NOEXCEPT
{
	//Destroy the entity.
	EntitySystem::Instance->DestroyEntity(*entity);

	//Clear the editor data.
	editor_data->_Links.Clear();

	//Load the JSON object.
	nlohmann::json JSON;

	{
		std::ifstream input_file{ file_path };
		input_file >> JSON;
		input_file.close();
	}

	//Serialize the entity to the stream archive.
	StreamArchive stream_archive;
	EntitySerialization::SerializeToStreamArchive(JSON, &stream_archive);

	//Now deserialize!
	uint64 stream_archive_position{ 0 };
	*entity = EntitySerialization::DeserializeFromStreamArchive(stream_archive, &stream_archive_position, nullptr);

	//Figure out the name.
	{
		const std::string _file_path{ file_path };
		const size_t last_slash_position{ _file_path.find_last_of("\\") };

		editor_data->_Name = _file_path.substr(last_slash_position + 1, _file_path.length() - last_slash_position - strlen(".Entity") - 1).c_str();
	}

	//Generate the entity identifier.
	GenerateEntityIdentifier(&editor_data->_Identifier);

	//Deserialize the editor data.
	const nlohmann::json &editor_data_entry{ JSON["EditorData"] };

	//Deserialize the rotation.
	{
		const nlohmann::json &rotation_entry{ editor_data_entry["Rotation"] };

		editor_data->_Rotation._Roll = BaseMath::DegreesToRadians(rotation_entry["Roll"]);
		editor_data->_Rotation._Yaw = BaseMath::DegreesToRadians(rotation_entry["Yaw"]);
		editor_data->_Rotation._Pitch = BaseMath::DegreesToRadians(rotation_entry["Pitch"]);
	}

	//Deserialize the hash string data.
	if (editor_data_entry.contains("HashStringData"))
	{
		const nlohmann::json &hash_string_data_entry{ editor_data_entry["HashStringData"] };
		const uint64 hash_string_data_size{ hash_string_data_entry.size() };
		editor_data->_HashStringData.Reserve(hash_string_data_size);

		for (const nlohmann::json &_hash_string_data_entry : hash_string_data_entry)
		{
			editor_data->_HashStringData.Emplace();
			EditorEntityData::HashStringData &new_hash_string_data{ editor_data->_HashStringData.Back() };

			new_hash_string_data._String = _hash_string_data_entry["String"].get<std::string>().c_str();
			new_hash_string_data._ComponentIdentifier = _hash_string_data_entry["ComponentIdentifier"].get<uint64>();
			new_hash_string_data._EditableFieldIdentifier = _hash_string_data_entry["EditableFieldIdentifier"].get<uint64>();
		}
	}
}

/*
*	Saves an entity.
*/
void EditorLevelSystem::SaveEntity(const char *const RESTRICT file_path, Entity *const RESTRICT entity, const EditorEntityData &editor_entity_data) NOEXCEPT
{
	//Set up the JSON.
	nlohmann::json JSON;

	//Calculate the entity statistics.
	EntityStatistics entity_statistics;

	//Update entity statistics from the static model component.
	if (StaticModelComponent::Instance->Has(entity))
	{
		//Cache the static model instance data.
		const StaticModelInstanceData &static_model_instance_data{ StaticModelComponent::Instance->InstanceData(entity) };

		//Cache the absolute axis aligned bounding box.
		const AxisAlignedBoundingBox3D absolute_axis_aligned_bounding_box{ static_model_instance_data._WorldSpaceAxisAlignedBoundingBox.GetAbsoluteAxisAlignedBoundingBox() };

		//Expand the axis aligned bounding box.
		entity_statistics._AxisAlignedBoundingBox.Expand(absolute_axis_aligned_bounding_box);
	}

	//Write the entity statistics.
	nlohmann::json &entity_statistics_entry{ JSON["EntityStatistics"] };

	{
		nlohmann::json &axis_aligned_bounding_box_entry{ entity_statistics_entry["AxisAlignedBoundingBox"] };

		{
			nlohmann::json &minimum_entry{ axis_aligned_bounding_box_entry["Minimum"] };

			minimum_entry["X"] = entity_statistics._AxisAlignedBoundingBox._Minimum._X;
			minimum_entry["Y"] = entity_statistics._AxisAlignedBoundingBox._Minimum._Y;
			minimum_entry["Z"] = entity_statistics._AxisAlignedBoundingBox._Minimum._Z;
		}

		{
			nlohmann::json &maximum_entry{ axis_aligned_bounding_box_entry["Maximum"] };

			maximum_entry["X"] = entity_statistics._AxisAlignedBoundingBox._Maximum._X;
			maximum_entry["Y"] = entity_statistics._AxisAlignedBoundingBox._Maximum._Y;
			maximum_entry["Z"] = entity_statistics._AxisAlignedBoundingBox._Maximum._Z;
		}
	}

	//Serialize the entity.
	EntitySerialization::SerializeToJSON(editor_entity_data, JSON, entity);

	//Serialize the editor data.
	nlohmann::json &editor_data_entry{ JSON["EditorData"] };

	//Serialize the rotation.
	{
		nlohmann::json &rotation_entry{ editor_data_entry["Rotation"] };

		rotation_entry["Roll"] = BaseMath::RadiansToDegrees(editor_entity_data._Rotation._Roll);
		rotation_entry["Yaw"] = BaseMath::RadiansToDegrees(editor_entity_data._Rotation._Yaw);
		rotation_entry["Pitch"] = BaseMath::RadiansToDegrees(editor_entity_data._Rotation._Pitch);
	}
	
	//Serialize the hash string data.
	if (!editor_entity_data._HashStringData.Empty())
	{
		nlohmann::json &hash_string_data_entry{ editor_data_entry["HashStringData"] };

		for (const EditorEntityData::HashStringData &hash_string_data : editor_entity_data._HashStringData)
		{
			nlohmann::json &_hash_string_data_entry{ hash_string_data_entry.emplace_back() };

			_hash_string_data_entry["String"] = hash_string_data._String.Data();
			_hash_string_data_entry["ComponentIdentifier"] = hash_string_data._ComponentIdentifier.operator size_t();
			_hash_string_data_entry["EditableFieldIdentifier"] = hash_string_data._EditableFieldIdentifier.operator size_t();
		}
	}

	//Write the JSON to the file.
	std::ofstream file{ file_path };
	file << std::setw(4) << JSON;
	file.close();
}

/*
*	Duplicates the selected entry.
*/
void EditorLevelSystem::DuplicateSelectedEntity() NOEXCEPT
{
	//The entity editor data might be invalid after we add a new one, 

	//Add a new entity.
	_Entities.Emplace();
	Entity *RESTRICT &new_entity{ _Entities.Back() };

	_EditorEntityData.Emplace();
	EditorEntityData &new_editor_entity_data{ _EditorEntityData.Back() };

	//Generate the entity name.
	{
		//If the name contains numbers at the end, maybe named something like "Object N", try to just add one to that number. Otherwise, just generate a new entity name.
		bool could_extend_entity_name{ false };
		
		{
			const char *const RESTRICT selected_entity_name{ _EditorEntityData[_SelectedEntityIndex]._Name.Data() };
			const uint64 selected_entity_name_length{ _EditorEntityData[_SelectedEntityIndex]._Name.Length() };

			uint64 numbers_start{ UINT64_MAXIMUM };

			for (int64 i{ static_cast<int64>(selected_entity_name_length - 1) }; i >= 0; --i)
			{
				if (StringUtilities::IsNumber(selected_entity_name[i]))
				{
					numbers_start = static_cast<uint64>(i);
				}

				else
				{
					break;
				}
			}

			if (numbers_start != UINT64_MAXIMUM)
			{
				uint64 numbers{ std::stoull(&selected_entity_name[numbers_start]) };

				char name_buffer[EditorEntityData::NAME_SIZE];

				for (uint64 i{ 0 }; i < numbers_start; ++i)
				{
					name_buffer[i] = selected_entity_name[i];
				}

				name_buffer[numbers_start] = '\0';

				char final_buffer[EditorEntityData::NAME_SIZE];

				/*
				*	This is a bit crude, but unless someone has created over ~2 quintillion entities with the same naming structure,
				*	we're bound to find a free name after a while.
				*/
				for (;;)
				{
					sprintf_s(final_buffer, "%s%llu", name_buffer, ++numbers);

					if (IsEntityNameAvailable(final_buffer))
					{
						new_editor_entity_data._Name = final_buffer;

						break;
					}
				}
				
				could_extend_entity_name = true;
			}
		}

		if (!could_extend_entity_name)
		{
			GenerateEntityName(new_editor_entity_data._Name.Data(), new_editor_entity_data._Name.Size());
		}
	}

	//Generate the entity identifier.
	GenerateEntityIdentifier(&new_editor_entity_data._Identifier);

	//Copy over some stuff to the newentity editor data.
	new_editor_entity_data._Rotation = _EditorEntityData[_SelectedEntityIndex]._Rotation;

	/*
	*	Maybe a bit overkill to do this whole round trip, but it keeps me from having to write more code here.
	*	Shouldn't be too bad. (:
	*/
	nlohmann::json JSON;
	StreamArchive stream_archive;
	uint64 stream_archive_position{ 0 };

	EntitySerialization::SerializeToJSON(_EditorEntityData[_SelectedEntityIndex], JSON, _Entities[_SelectedEntityIndex]);
	EntitySerialization::SerializeToStreamArchive(JSON, &stream_archive);
	new_entity = EntitySerialization::DeserializeFromStreamArchive(stream_archive, &stream_archive_position, nullptr);

	//Set the new selected entity index.
	SetSelectedEntityIndex(_Entities.LastIndex());
}

/*
*	Returns the index for the given entity identifier.
*/
NO_DISCARD uint64 EditorLevelSystem::EntityIdentifierToIndex(const uint64 identifier) NOEXCEPT
{
	for (uint64 entity_index{ 0 }; entity_index < _Entities.Size(); ++entity_index)
	{
		if (_EditorEntityData[entity_index]._Identifier == identifier)
		{
			return entity_index;
		}
	}

	return UINT64_MAXIMUM;
}

/*
*	Returns if the first entity is linked to the second entity.
*/
NO_DISCARD bool EditorLevelSystem::AreEntitiesLinked(const uint64 entity_index_1, const uint64 entity_index_2) NOEXCEPT
{
	for (const uint64 link : _EditorEntityData[entity_index_1]._Links)
	{
		if (EntityIdentifierToIndex(link) == entity_index_2)
		{
			return true;
		}
	}

	return false;
}

/*
*	Returns if the given entity name is available.
*/
NO_DISCARD bool EditorLevelSystem::IsEntityNameAvailable(const char *const RESTRICT name) NOEXCEPT
{
	for (const EditorEntityData &editor_entity_data : _EditorEntityData)
	{
		if (StringUtilities::IsEqual(editor_entity_data._Name.Data(), name))
		{
			return false;
		}
	}

	return true;
}
#endif