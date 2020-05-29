#if defined(CATALYST_EDITOR)
//Header file.
#include <Editor/EditorSelectionSystem.h>

//Core.
#include <Core/General/Perceiver.h>

//Entities.
#include <Entities/Types/DynamicModelEntity.h>

//Math.
#include <Math/Core/CatalystGeometryMath.h>

//Rendering.
#include <Rendering/Native/RenderingUtilities.h>

//Systems.
#include <Systems/CatalystEditorSystem.h>
#include <Systems/CatalystEngineSystem.h>
#include <Systems/InputSystem.h>
#include <Systems/PhysicsSystem.h>
#include <Systems/RenderingSystem.h>
#include <Systems/ResourceSystem.h>

//Third party.
#include <ThirdParty/imgui.h>

/*
*	Initializes the editor selection system.
*/
void EditorSelectionSystem::Initialize() NOEXCEPT
{
	
}

/*
*	Updates the editor selection system.
*/
void EditorSelectionSystem::Update() NOEXCEPT
{
	//Only update if the editor isn't in a game.
	if (CatalystEditorSystem::Instance->IsInGame())
	{
		return;
	}

	//Calculate the ray.
	Ray ray;

	ray.SetOrigin(Perceiver::Instance->GetWorldTransform().GetAbsolutePosition());
	ray.SetDirection(RenderingUtilities::CalculateRayDirectionFromScreenCoordinate(Vector2<float32>(InputSystem::Instance->GetMouseState()->_CurrentX, InputSystem::Instance->GetMouseState()->_CurrentY)));

	if (_CurrentlySelectedEntityOverride)
	{
		SetCurrentlySelectedEntity(_CurrentlySelectedEntityOverride);

		_CurrentlySelectedEntityOverride = nullptr;
	}

	//Cast a ray from the mouse position and forward to determine a new selected entity.
	else if (!ImGui::IsAnyWindowHovered() && !ImGui::IsAnyItemHovered() && !CatalystEditorSystem::Instance->GetEditorEntitySystem()->IsCurrentlyCreatingAnEntity())
	{
		//Cache if the left mouse button is pressed.
		const bool left_mouse_button_pressed{ InputSystem::Instance->GetMouseState()->_Left == ButtonState::PRESSED };

		if (left_mouse_button_pressed)
		{
			Ray ray;

			ray.SetOrigin(Perceiver::Instance->GetWorldTransform().GetAbsolutePosition());
			ray.SetDirection(RenderingUtilities::CalculateRayDirectionFromScreenCoordinate(Vector2<float32>(InputSystem::Instance->GetMouseState()->_CurrentX, InputSystem::Instance->GetMouseState()->_CurrentY)));

			RaycastConfiguration configuration;

			configuration._PhysicsChannels = PhysicsChannel::DYNAMIC_MODELS;
			configuration._MaximumHitDistance = CatalystEngineSystem::Instance->GetProjectConfiguration()->_RenderingConfiguration._ViewDistance;
			configuration._TerrainRayMarchStep = 1.0f;

			RaycastResult result;

			PhysicsSystem::Instance->CastRay(ray, configuration, &result);

			if (result._HasHit)
			{
				switch (result._Type)
				{
				case RaycastResult::Type::DYNAMIC_MODEL:
				{
					SetCurrentlySelectedEntity(result._DynamicModelRaycastResult._Entity);

					break;
				}

				case RaycastResult::Type::STATIC_MODEL:
				{
					SetCurrentlySelectedEntity(result._StaticModelRaycastResult._Entity);

					break;
				}

				case RaycastResult::Type::TERRAIN:
				{
					//HWEL

					break;
				}

				default:
				{
					ASSERT(false, "Invalid case!");

					break;
				}
				}
			}

			else
			{
				SetCurrentlySelectedEntity(nullptr);
			}
		}
	}

	if (_CurrentlySelectedEntity && _CurrentlySelectedEntity->_Initialized)
	{
		//Render a bounding box for the currently selected entity.
		switch (_CurrentlySelectedEntity->_Type)
		{
			case EntityType::DynamicModel:
			{
				AxisAlignedBoundingBox3 box{ *static_cast<DynamicModelEntity *const RESTRICT>(_CurrentlySelectedEntity)->GetWorldSpaceAxisAlignedBoundingBox() };

				box._Minimum -= 0.1f;
				box._Maximum += 0.1f;

				RenderingSystem::Instance->GetDebugRenderingSystem()->DebugRenderAxisAlignedBoundingBox3D(Vector4<float32>(0.0f, 1.0f, 1.0f, 1.0f), true, true, box, 0.0f);
				RenderingSystem::Instance->GetDebugRenderingSystem()->DebugRenderAxisAlignedBoundingBox3D(Vector4<float32>(0.0f, 1.0f, 1.0f, 0.25f * 0.125f), true, false, box, 0.0f);

				break;
			}
		}

		//Display a screen with this entities properties.
		ImGui::Begin("Selected Entity", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);
		ImGui::SetWindowPos(ImVec2(1'920.0f - 8.0f - 512.0f, 8.0f));
		ImGui::SetWindowSize(ImVec2(512.0f, 256.0f));

		switch (_CurrentlySelectedEntity->_Type)
		{
			case EntityType::DynamicModel:
			{
				//Cache the dynamic model entity.
				DynamicModelEntity *const RESTRICT dynamic_model_entity{ static_cast<DynamicModelEntity *const RESTRICT>(_CurrentlySelectedEntity) };

				//Add the header text.
				ImGui::Text("Dynamic Model");

				char buffer[128];

				sprintf_s(buffer, "Model Resource: %s", dynamic_model_entity->GetModelResource()->_Header._ResourceName.Data());

				if (ImGui::Button(buffer))
				{
					_DynamicModelSelectionData._IsSelectingModelResource = true;
					_DynamicModelSelectionData._IsSelectingMaterialResource = false;
				}

				for (uint8 i{ 0 }, size{ static_cast<uint8>(dynamic_model_entity->GetModelResource()->_Meshes.Size()) }; i < size; ++i)
				{
					if (dynamic_model_entity->GetMaterialResources()[i])
					{
						char buffer[128];

						sprintf_s(buffer, "Material Resource #%u: %s", i, dynamic_model_entity->GetMaterialResources()[i]->_Header._ResourceName.Data());

						if (ImGui::Button(buffer))
						{
							_DynamicModelSelectionData._IsSelectingModelResource = false;
							_DynamicModelSelectionData._IsSelectingMaterialResource = true;
							_DynamicModelSelectionData._SelectedMaterialIndex = i;

							break;
						}
					}
				}

				if (_DynamicModelSelectionData._IsSelectingModelResource)
				{
					ImGui::Begin("Choose New Model Resource:", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);
					ImGui::SetWindowPos(ImVec2(1'920.0f - 8.0f - 512.0f - 8.0f - 256.0f, 8.0f));
					ImGui::SetWindowSize(ImVec2(256.0f, 256.0f));

					const HashTable<HashString, ModelResource* RESTRICT> &all_model_resources{ ResourceSystem::Instance->GetAllModelResources() };

					for (const ModelResource *const RESTRICT model_resource : all_model_resources.ValueIterator())
					{
						if (ImGui::Button(model_resource->_Header._ResourceName.Data()))
						{
							dynamic_model_entity->SetModelResource(ResourceSystem::Instance->GetModelResource(model_resource->_Header._ResourceIdentifier));

							_DynamicModelSelectionData._IsSelectingModelResource = false;

							break;
						}
					}

					ImGui::End();
				}

				if (_DynamicModelSelectionData._IsSelectingMaterialResource)
				{
					ImGui::Begin("Choose New Material Resource:", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);
					ImGui::SetWindowPos(ImVec2(1'920.0f - 8.0f - 512.0f - 8.0f - 256.0f, 8.0f));
					ImGui::SetWindowSize(ImVec2(256.0f, 256.0f));

					const HashTable<HashString, MaterialResource* RESTRICT> &all_material_resources{ ResourceSystem::Instance->GetAllMaterialResources() };

					for (const MaterialResource *const RESTRICT material_resource : all_material_resources.ValueIterator())
					{
						if (ImGui::Button(material_resource->_Header._ResourceName.Data()))
						{
							dynamic_model_entity->SetMaterialResource(_DynamicModelSelectionData._SelectedMaterialIndex, ResourceSystem::Instance->GetMaterialResource(material_resource->_Header._ResourceIdentifier));

							_DynamicModelSelectionData._IsSelectingMaterialResource = false;

							break;
						}
					}

					ImGui::End();
				}

				//Cache the world transform.
				WorldTransform *const RESTRICT world_transform{ dynamic_model_entity->ModifyWorldTransform() };

				//Add the position editor.
				Vector3<float32> position{ world_transform->GetAbsolutePosition() };

				if (ImGui::DragFloat3("Position", reinterpret_cast<float32 *const RESTRICT>(&position), 0.01f))
				{
					world_transform->SetAbsolutePosition(position);
				}

				//Add the rotation editor.
				Vector3<float32> rotation{ world_transform->GetRotation() };

				rotation._X = CatalystBaseMath::RadiansToDegrees(rotation._X);
				rotation._Y = CatalystBaseMath::RadiansToDegrees(rotation._Y);
				rotation._Z = CatalystBaseMath::RadiansToDegrees(rotation._Z);

				if (ImGui::DragFloat3("Rotation", reinterpret_cast<float32 *const RESTRICT>(&rotation), 0.1f))
				{
					rotation._X = CatalystBaseMath::DegreesToRadians(rotation._X);
					rotation._Y = CatalystBaseMath::DegreesToRadians(rotation._Y);
					rotation._Z = CatalystBaseMath::DegreesToRadians(rotation._Z);

					world_transform->SetRotation(rotation);
				}

				break;
			}
		}

		ImGui::End();
	}

	//Transform the currently selected entity.
	TransformCurrentlySelectedEntity(ray);
}

/*
*	Sets the currently selected entity.
*/
void EditorSelectionSystem::SetCurrentlySelectedEntity(Entity *const RESTRICT entity) NOEXCEPT
{
	//Set the currently selected entity.
	_CurrentlySelectedEntity = entity;

	//Reset the state of the selected entity.
	Memory::Set(reinterpret_cast<byte* const RESTRICT>(this) + offsetof(EditorSelectionSystem, _DynamicModelSelectionData), 0, sizeof(EditorSelectionSystem) - sizeof(void* const RESTRICT));
}

/*
*	Transforms the currently selected entity.
*/
void EditorSelectionSystem::TransformCurrentlySelectedEntity(const Ray& ray)
{
	if (!_CurrentlySelectedEntity || !_CurrentlySelectedEntity->_Initialized)
	{
		return;
	}

	//Cache the world transform of the selected entity.
	WorldTransform world_transform;

	switch (_CurrentlySelectedEntity->_Type)
	{
		case EntityType::DynamicModel:
		{
			world_transform = *static_cast<DynamicModelEntity* const RESTRICT>(_CurrentlySelectedEntity)->GetWorldTransform();

			break;
		}

		default:
		{
			ASSERT(false, "Invalid case!");

			break;
		}
	}

	//Cache the world position.
	Vector3<float32> world_position{ world_transform.GetAbsolutePosition() };

	//Calculate the desired position.
	Vector3<float32> desired_position{ world_position };

	//Calculate the desired position on the X axis.
	{
		//Cast the ray against the XZ plane that the entity lies on.
		Plane plane{ world_position, CatalystWorldCoordinateSpace::UP };

		float32 intersection_distance;

		if (CatalystGeometryMath::RayPlaneIntersection(ray, plane, &intersection_distance))
		{
			desired_position._X = (ray._Origin + ray._Direction * intersection_distance)._X;
		}
	}

	/*
	//Calculate the desired position on the Y axis.
	{
		//Cast the ray against the Y plane that the entity lies on.
		Plane plane{ world_position, CatalystWorldCoordinateSpace::UP };

		float32 intersection_distance;

		if (CatalystGeometryMath::RayPlaneIntersection(ray, plane, &intersection_distance))
		{
			desired_position._X = (ray._Origin + ray._Direction * intersection_distance)._X;
		}
	}
	*/

	//Calculate the desired position on the Z axis.
	{
		//Cast the ray against the XZ plane that the entity lies on.
		Plane plane{ world_position, CatalystWorldCoordinateSpace::UP };

		float32 intersection_distance;

		if (CatalystGeometryMath::RayPlaneIntersection(ray, plane, &intersection_distance))
		{
			desired_position._Z = (ray._Origin + ray._Direction * intersection_distance)._Z;
		}
	}

	//Calculate which axes should be applied.
	const KeyboardState *const RESTRICT keyboard_state{ InputSystem::Instance->GetKeyboardState() };

	desired_position._X = keyboard_state->GetButtonState(KeyboardButton::X) == ButtonState::PRESSED || keyboard_state->GetButtonState(KeyboardButton::X) == ButtonState::PRESSED_HELD ? desired_position._X : world_position._X;
	desired_position._Y = keyboard_state->GetButtonState(KeyboardButton::Y) == ButtonState::PRESSED || keyboard_state->GetButtonState(KeyboardButton::Y) == ButtonState::PRESSED_HELD ? desired_position._Y : world_position._Y;
	desired_position._Z = keyboard_state->GetButtonState(KeyboardButton::Z) == ButtonState::PRESSED || keyboard_state->GetButtonState(KeyboardButton::Z) == ButtonState::PRESSED_HELD ? desired_position._Z : world_position._Z;

	//Set the position on the selected entity.
	world_transform.SetAbsolutePosition(desired_position);

	switch (_CurrentlySelectedEntity->_Type)
	{
		case EntityType::DynamicModel:
		{
			*static_cast<DynamicModelEntity* const RESTRICT>(_CurrentlySelectedEntity)->ModifyWorldTransform() = world_transform;

			break;
		}

		default:
		{
			ASSERT(false, "Invalid case!");

			break;
		}
	}
}
#endif