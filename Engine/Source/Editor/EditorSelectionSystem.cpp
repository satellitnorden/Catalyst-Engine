#if defined(CATALYST_EDITOR)
//Header file.
#include <Editor/EditorSelectionSystem.h>

//Core.
#include <Core/General/Perceiver.h>

//Componens.
#include <Components/Core/ComponentManager.h>

//Entities.
#include <Entities/Types/DynamicModelEntity.h>
#include <Entities/Types/StaticModelEntity.h>
#include <Entities/Types/LightEntity.h>

//Math.
#include <Math/Core/CatalystGeometryMath.h>

//Rendering.
#include <Rendering/Native/RenderingUtilities.h>

//Systems.
#include <Systems/CatalystEditorSystem.h>
#include <Systems/CatalystEngineSystem.h>
#include <Systems/EntitySystem.h>
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

	//Add the scene window.
	AddSceneWindow();

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

			configuration._PhysicsChannels = PhysicsChannel::ALL;
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
						SetCurrentlySelectedEntity(nullptr);

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
		//Display a screen with this entities properties.
		ImGui::Begin("Selected Entity", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);
		ImGui::SetWindowPos(ImVec2(1'920.0f - 256.0f, 512.0f));
		ImGui::SetWindowSize(ImVec2(256.0f, 1080.0f - 512.0f));

		//Add a button for destroying the entity.
		if (ImGui::Button("Destroy Entity")
			|| InputSystem::Instance->GetKeyboardState()->GetButtonState(KeyboardButton::Delete) == ButtonState::PRESSED)
		{
			EntitySystem::Instance->RequestTermination(_CurrentlySelectedEntity);
			EntitySystem::Instance->RequestDestruction(_CurrentlySelectedEntity);
		}

		else if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::Text("Hotkey: Delete");
			ImGui::EndTooltip();
		}

		//Add a button for duplication the entity.
		const bool wants_to_duplicate{	(InputSystem::Instance->GetKeyboardState()->GetButtonState(KeyboardButton::LeftControl) == ButtonState::PRESSED || InputSystem::Instance->GetKeyboardState()->GetButtonState(KeyboardButton::LeftControl) == ButtonState::PRESSED_HELD)
										&& (InputSystem::Instance->GetKeyboardState()->GetButtonState(KeyboardButton::D) == ButtonState::PRESSED || InputSystem::Instance->GetKeyboardState()->GetButtonState(KeyboardButton::D) == ButtonState::PRESSED_HELD) };
		
		Entity *RESTRICT duplicated_entity{ nullptr };

		if (ImGui::Button("Duplicate Entity")
			|| wants_to_duplicate)
		{
			if (!_DuplicatedEntity)
			{
				duplicated_entity = EntitySystem::Instance->DuplicateEntity(_CurrentlySelectedEntity);

				_DuplicatedEntity = true;
			}
		}

		else
		{
			_DuplicatedEntity = false;
		}

		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::Text("Hotkey: Control + D");
			ImGui::EndTooltip();

			_DuplicatedEntity = false;
		}

		switch (_CurrentlySelectedEntity->_Type)
		{
			case EntityType::DynamicModel:
			{
				//Cache the dynamic model entity.
				DynamicModelEntity *const RESTRICT dynamic_model_entity{ static_cast<DynamicModelEntity *const RESTRICT>(_CurrentlySelectedEntity) };

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

				//Add widgets for the model collision configuration.
				{
					ModelCollisionConfiguration current_model_collision_configuration{ dynamic_model_entity->GetModelCollisionConfiguration() };

					{
						char buffer[128];

						switch (current_model_collision_configuration._Type)
						{
							case ModelCollisionType::NONE:
							{
								sprintf_s(buffer, "Model Collision Type: None");

								break;
							}

							case ModelCollisionType::BOX:
							{
								sprintf_s(buffer, "Model Collision Type: Box");

								break;
							}

							case ModelCollisionType::COLLISION_MODEL:
							{
								sprintf_s(buffer, "Model Collision Type: Collision Model");

								break;
							}

							default:
							{
								ASSERT(false, "Invalid case!");

								break;
							}
						}

						if (ImGui::Button(buffer))
						{
							switch (current_model_collision_configuration._Type)
							{
								case ModelCollisionType::NONE:
								{
									current_model_collision_configuration._Type = ModelCollisionType::BOX;

									break;
								}

								case ModelCollisionType::BOX:
								{
									if (dynamic_model_entity->GetModelResource()->_CollisionModel)
									{
										current_model_collision_configuration._Type = ModelCollisionType::COLLISION_MODEL;
									}

									else
									{
										current_model_collision_configuration._Type = ModelCollisionType::NONE;
									}

									break;
								}

								case ModelCollisionType::COLLISION_MODEL:
								{
									current_model_collision_configuration._Type = ModelCollisionType::NONE;

									break;
								}

								default:
								{
									ASSERT(false, "Invalid case!");

									break;
								}
							}

							dynamic_model_entity->SetModelCollisionConfiguration(current_model_collision_configuration);
						}
					}
				}

				if (_DynamicModelSelectionData._IsSelectingModelResource)
				{
					ImGui::Begin("Choose New Model Resource:", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);
					ImGui::SetWindowPos(ImVec2(1'920.0f - 256.0f - 256.0f, 512.0f));
					ImGui::SetWindowSize(ImVec2(256.0f, 1080.0f - 512.0f));

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
					ImGui::SetWindowPos(ImVec2(1'920.0f - 256.0f - 256.0f, 512.0f));
					ImGui::SetWindowSize(ImVec2(256.0f, 1080.0f - 512.0f));

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

				//Get the world transform.
				WorldTransform world_transform{ *dynamic_model_entity->GetWorldTransform() };

				//Add the position editor.
				Vector3<float32> position{ world_transform.GetAbsolutePosition() };

				if (ImGui::DragFloat3("Position", &position[0], 0.01f))
				{
					world_transform.SetAbsolutePosition(position);
				}

				//Add the rotation editor.
				EulerAngles rotation{ world_transform.GetRotation() };

				rotation._Roll = CatalystBaseMath::RadiansToDegrees(rotation._Roll);
				rotation._Yaw = CatalystBaseMath::RadiansToDegrees(rotation._Yaw);
				rotation._Pitch = CatalystBaseMath::RadiansToDegrees(rotation._Pitch);

				if (ImGui::DragFloat3("Rotation", rotation.Data(), 0.1f))
				{
					rotation._Roll = CatalystBaseMath::DegreesToRadians(rotation._Roll);
					rotation._Yaw = CatalystBaseMath::DegreesToRadians(rotation._Yaw);
					rotation._Pitch = CatalystBaseMath::DegreesToRadians(rotation._Pitch);

					world_transform.SetRotation(rotation);
				}

				//Add the scale editor.
				float32 scale{ world_transform.GetScale() };

				if (ImGui::DragFloat("Scale", &scale, 0.01f))
				{
					world_transform.SetScale(scale);
				}

				//Set the world transform.
				dynamic_model_entity->SetWorldTransform(world_transform);

				break;
			}

			case EntityType::Light:
			{
				//Cache the light entity.
				LightEntity *const RESTRICT light_entity{ static_cast<LightEntity *const RESTRICT>(_CurrentlySelectedEntity) };

				//Add a widget for modifying the light type of this light.
				{
					LightType current_light_type{ light_entity->GetLightType() };

					char buffer[32];

					switch (light_entity->GetLightType())
					{
						case LightType::DIRECTIONAL:
						{
							sprintf_s(buffer, "Light Type: %s", "Directional");

							break;
						}

						case LightType::POINT:
						{
							sprintf_s(buffer, "Light Type: %s", "Point");

							break;
						}

						default:
						{
							ASSERT(false, "Invalid case!");

							break;
						}
					}
					
					if (ImGui::Button(buffer))
					{
						//Change the light type.
						switch (light_entity->GetLightType())
						{
							case LightType::DIRECTIONAL:
							{
								light_entity->SetLightType(LightType::POINT);

								break;
							}

							case LightType::POINT:
							{
								light_entity->SetLightType(LightType::DIRECTIONAL);

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

				//Add checkboxes for the different light properties.
				{
					{
						uint32 current_light_properties{ light_entity->GetLightProperties() };

						bool surface_shadow_casting{ TEST_BIT(current_light_properties, CatalystShaderConstants::LIGHT_PROPERTY_SURFACE_SHADOW_CASTING_BIT) };

						if (ImGui::Checkbox("Surface Shadow Casting", &surface_shadow_casting))
						{
							if (surface_shadow_casting)
							{
								SET_BIT(current_light_properties, CatalystShaderConstants::LIGHT_PROPERTY_SURFACE_SHADOW_CASTING_BIT);
							}

							else
							{
								CLEAR_BIT(current_light_properties, CatalystShaderConstants::LIGHT_PROPERTY_SURFACE_SHADOW_CASTING_BIT);
							}

							light_entity->SetLightProperties(current_light_properties);
						}
					}

					{
						uint32 current_light_properties{ light_entity->GetLightProperties() };

						bool volumetric{ TEST_BIT(current_light_properties, CatalystShaderConstants::LIGHT_PROPERTY_VOLUMETRIC_BIT) };

						if (ImGui::Checkbox("Volumetric", &volumetric))
						{
							if (volumetric)
							{
								SET_BIT(current_light_properties, CatalystShaderConstants::LIGHT_PROPERTY_VOLUMETRIC_BIT);
							}

							else
							{
								CLEAR_BIT(current_light_properties, CatalystShaderConstants::LIGHT_PROPERTY_VOLUMETRIC_BIT);
							}

							light_entity->SetLightProperties(current_light_properties);
						}
					}

					{
						uint32 current_light_properties{ light_entity->GetLightProperties() };

						bool volumetric_shadow_casting{ TEST_BIT(current_light_properties, CatalystShaderConstants::LIGHT_PROPERTY_VOLUMETRIC_SHADOW_CASTING_BIT) };

						if (ImGui::Checkbox("Volumetric Shadow Casting", &volumetric_shadow_casting))
						{
							if (volumetric_shadow_casting)
							{
								SET_BIT(current_light_properties, CatalystShaderConstants::LIGHT_PROPERTY_VOLUMETRIC_SHADOW_CASTING_BIT);
							}

							else
							{
								CLEAR_BIT(current_light_properties, CatalystShaderConstants::LIGHT_PROPERTY_VOLUMETRIC_SHADOW_CASTING_BIT);
							}

							light_entity->SetLightProperties(current_light_properties);
						}
					}
				}

				switch (light_entity->GetLightType())
				{
					case LightType::DIRECTIONAL:
					{
						//Add a widget for modifying the rotation of this directional light.
						{
							EulerAngles rotation{ light_entity->GetRotation() };

							rotation._Roll = CatalystBaseMath::RadiansToDegrees(rotation._Roll);
							rotation._Yaw = CatalystBaseMath::RadiansToDegrees(rotation._Yaw);
							rotation._Pitch = CatalystBaseMath::RadiansToDegrees(rotation._Pitch);

							if (ImGui::DragFloat3("Rotation", rotation.Data(), 0.1f))
							{
								rotation._Roll = CatalystBaseMath::DegreesToRadians(rotation._Roll);
								rotation._Yaw = CatalystBaseMath::DegreesToRadians(rotation._Yaw);
								rotation._Pitch = CatalystBaseMath::DegreesToRadians(rotation._Pitch);

								light_entity->SetRotation(rotation);
							}
						}

						break;
					}

					case LightType::POINT:
					{
						//Add a widget for modifying the position of this point light.
						{
							Vector3<float32> position{ light_entity->GetWorldPosition().GetAbsolutePosition() };

							if (ImGui::DragFloat3("Position", &position[0], 0.01f))
							{
								light_entity->SetWorldPosition(WorldPosition(position));
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

				//Add a widget for modifying the color of this light.
				{
					Vector3<float32> color{ light_entity->GetColor() };

					if (ImGui::ColorEdit3("Color", &color[0]))
					{
						light_entity->SetColor(color);
					}
				}

				//Add a widget for modifying the intensity of this light.
				{
					float32 intensity{ light_entity->GetIntensity() };

					if (ImGui::DragFloat("Intensity", &intensity))
					{
						light_entity->SetIntensity(intensity);
					}
				}

				switch (light_entity->GetLightType())
				{
					case LightType::DIRECTIONAL:
					{
						

						break;
					}

					case LightType::POINT:
					{
						//Add a widget for modifying the radius of this point light.
						{
							float32 radius{ light_entity->GetRadius() };

							if (ImGui::DragFloat("Radius", &radius))
							{
								light_entity->SetRadius(radius);
							}
						}

						//Add a widget for modifying the size of this point light.
						{
							float32 size{ light_entity->GetSize() };

							if (ImGui::DragFloat("Size", &size))
							{
								light_entity->SetSize(size);
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

				break;
			}

			case EntityType::StaticModel:
			{
				//Cache the static model entity.
				StaticModelEntity *const RESTRICT static_model_entity{ static_cast<StaticModelEntity *const RESTRICT>(_CurrentlySelectedEntity) };

				char buffer[128];

				sprintf_s(buffer, "Model Resource: %s", static_model_entity->GetModelResource()->_Header._ResourceName.Data());

				if (ImGui::Button(buffer))
				{
					_StaticModelSelectionData._IsSelectingModelResource = true;
					_StaticModelSelectionData._IsSelectingMaterialResource = false;
				}

				for (uint8 i{ 0 }, size{ static_cast<uint8>(static_model_entity->GetModelResource()->_Meshes.Size()) }; i < size; ++i)
				{
					if (static_model_entity->GetMaterialResources()[i])
					{
						char buffer[128];

						sprintf_s(buffer, "Material Resource #%u: %s", i, static_model_entity->GetMaterialResources()[i]->_Header._ResourceName.Data());

						if (ImGui::Button(buffer))
						{
							_StaticModelSelectionData._IsSelectingModelResource = false;
							_StaticModelSelectionData._IsSelectingMaterialResource = true;
							_StaticModelSelectionData._SelectedMaterialIndex = i;

							break;
						}
					}
				}

				//Add widgets for the model collision configuration.
				{
					ModelCollisionConfiguration current_model_collision_configuration{ static_model_entity->GetModelCollisionConfiguration() };

					{
						char buffer[128];

						switch (current_model_collision_configuration._Type)
						{
							case ModelCollisionType::NONE:
							{
								sprintf_s(buffer, "Model Collision Type: None");

								break;
							}

							case ModelCollisionType::BOX:
							{
								sprintf_s(buffer, "Model Collision Type: Box");

								break;
							}

							case ModelCollisionType::COLLISION_MODEL:
							{
								sprintf_s(buffer, "Model Collision Type: Collision Model");

								break;
							}

							default:
							{
								ASSERT(false, "Invalid case!");

								break;
							}
						}

						if (ImGui::Button(buffer))
						{
							switch (current_model_collision_configuration._Type)
							{
								case ModelCollisionType::NONE:
								{
									current_model_collision_configuration._Type = ModelCollisionType::BOX;

									break;
								}

								case ModelCollisionType::BOX:
								{
									if (static_model_entity->GetModelResource()->_CollisionModel)
									{
										current_model_collision_configuration._Type = ModelCollisionType::COLLISION_MODEL;
									}

									else
									{
										current_model_collision_configuration._Type = ModelCollisionType::NONE;
									}

									break;
								}

								case ModelCollisionType::COLLISION_MODEL:
								{
									current_model_collision_configuration._Type = ModelCollisionType::NONE;

									break;
								}

								default:
								{
									ASSERT(false, "Invalid case!");

									break;
								}
							}

							static_model_entity->SetModelCollisionConfiguration(current_model_collision_configuration);
						}
					}
				}

				if (_StaticModelSelectionData._IsSelectingModelResource)
				{
					ImGui::Begin("Choose New Model Resource:", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);
					ImGui::SetWindowPos(ImVec2(1'920.0f - 256.0f - 256.0f, 512.0f));
					ImGui::SetWindowSize(ImVec2(256.0f, 1080.0f - 512.0f));

					const HashTable<HashString, ModelResource* RESTRICT> &all_model_resources{ ResourceSystem::Instance->GetAllModelResources() };

					for (const ModelResource *const RESTRICT model_resource : all_model_resources.ValueIterator())
					{
						if (ImGui::Button(model_resource->_Header._ResourceName.Data()))
						{
							static_model_entity->SetModelResource(ResourceSystem::Instance->GetModelResource(model_resource->_Header._ResourceIdentifier));

							_StaticModelSelectionData._IsSelectingModelResource = false;

							break;
						}
					}

					ImGui::End();
				}

				if (_StaticModelSelectionData._IsSelectingMaterialResource)
				{
					ImGui::Begin("Choose New Material Resource:", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);
					ImGui::SetWindowPos(ImVec2(1'920.0f - 256.0f - 256.0f, 512.0f));
					ImGui::SetWindowSize(ImVec2(256.0f, 1080.0f - 512.0f));

					const HashTable<HashString, MaterialResource* RESTRICT> &all_material_resources{ ResourceSystem::Instance->GetAllMaterialResources() };

					for (const MaterialResource *const RESTRICT material_resource : all_material_resources.ValueIterator())
					{
						if (ImGui::Button(material_resource->_Header._ResourceName.Data()))
						{
							static_model_entity->SetMaterialResource(_StaticModelSelectionData._SelectedMaterialIndex, ResourceSystem::Instance->GetMaterialResource(material_resource->_Header._ResourceIdentifier));

							_StaticModelSelectionData._IsSelectingMaterialResource = false;

							break;
						}
					}

					ImGui::End();
				}

				//Cache the world transform.
				WorldTransform world_transform{ *static_model_entity->GetWorldTransform() };

				//Add the position editor.
				Vector3<float32> position{ world_transform.GetAbsolutePosition() };

				if (ImGui::DragFloat3("Position", &position[0], 0.01f))
				{
					world_transform.SetAbsolutePosition(position);
				}

				//Add the rotation editor.
				EulerAngles rotation{ world_transform.GetRotation() };

				rotation._Roll = CatalystBaseMath::RadiansToDegrees(rotation._Roll);
				rotation._Yaw = CatalystBaseMath::RadiansToDegrees(rotation._Yaw);
				rotation._Pitch = CatalystBaseMath::RadiansToDegrees(rotation._Pitch);

				if (ImGui::DragFloat3("Rotation", rotation.Data(), 0.1f))
				{
					rotation._Roll = CatalystBaseMath::DegreesToRadians(rotation._Roll);
					rotation._Yaw = CatalystBaseMath::DegreesToRadians(rotation._Yaw);
					rotation._Pitch = CatalystBaseMath::DegreesToRadians(rotation._Pitch);

					world_transform.SetRotation(rotation);
				}

				//Add the scale editor.
				float32 scale{ world_transform.GetScale() };

				if (ImGui::DragFloat("Scale", &scale, 0.01f))
				{
					world_transform.SetScale(scale);
				}

				//Set the world transform.
				static_model_entity->SetWorldTransform(world_transform);

				break;
			}
		}

		ImGui::End();

		//The then entity was duplicated, set it to the currently selected entity.
		if (duplicated_entity)
		{
			SetCurrentlySelectedEntity(duplicated_entity);
		}
	}

	//Transform the currently selected entity.
	TransformCurrentlySelectedEntity(ray);
}

/*
*	Adds the scene window.
*/
void EditorSelectionSystem::AddSceneWindow() NOEXCEPT
{
	//Add the level window.
	ImGui::Begin("Scene", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);
	ImGui::SetWindowPos(ImVec2(1'920.0f - 256.0f, 0.0f));
	ImGui::SetWindowSize(ImVec2(256.0f, 512.0f));

	//List all dynamic model entities.
	{
		const uint64 number_of_components{ ComponentManager::GetNumberOfDynamicModelComponents() };

		for (uint64 i{ 0 }; i < number_of_components; ++i)
		{
			char buffer[64];

			sprintf_s(buffer, "Dynamic Model Entity #%llu", i + 1);

			if (ImGui::Button(buffer))
			{
				SetCurrentlySelectedEntity(ComponentManager::GetDynamicModelEntities()->At(i));
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
				SetCurrentlySelectedEntity(ComponentManager::GetLightEntities()->At(i));
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
				SetCurrentlySelectedEntity(ComponentManager::GetStaticModelEntities()->At(i));
			}
		}
	}

	ImGui::End();
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

		case EntityType::Light:
		{
			const LightEntity *const RESTRICT light_entity{ static_cast<const LightEntity *const RESTRICT>(_CurrentlySelectedEntity) };

			switch (light_entity->GetLightType())
			{
				case LightType::DIRECTIONAL:
				{
					//Directional lights have no world transform, so just leave empty. (:

					break;
				}

				case LightType::POINT:
				{
					world_transform = WorldTransform(light_entity->GetWorldPosition().GetCell(), light_entity->GetWorldPosition().GetLocalPosition(), EulerAngles(), 1.0f);

					break;
				}

				default:
				{
					ASSERT(false, "Invalid case!");

					break;
				}
			}

			break;
		}

		case EntityType::StaticModel:
		{
			world_transform = *static_cast<StaticModelEntity* const RESTRICT>(_CurrentlySelectedEntity)->GetWorldTransform();

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
		Plane plane{ world_position, ray._Origin._Y >= world_position._Y ? CatalystWorldCoordinateSpace::UP : CatalystWorldCoordinateSpace::DOWN };

		float32 intersection_distance;

		if (CatalystGeometryMath::RayPlaneIntersection(ray, plane, &intersection_distance))
		{
			desired_position._X = (ray._Origin + ray._Direction * intersection_distance)._X;
		}
	}

	//Calculate the desired position on the Y axis.
	{
		//Cast the ray against the Y plane that the entity lies on.
		Plane plane{ world_position, Vector3<float32>::NormalizeXZ(ray._Origin - world_position) };

		float32 intersection_distance;

		if (CatalystGeometryMath::RayPlaneIntersection(ray, plane, &intersection_distance))
		{
			desired_position._Y = (ray._Origin + ray._Direction * intersection_distance)._Y;
		}
	}

	//Calculate the desired position on the Z axis.
	{
		//Cast the ray against the XZ plane that the entity lies on.
		Plane plane{ world_position, ray._Origin._Y >= world_position._Y ? CatalystWorldCoordinateSpace::UP : CatalystWorldCoordinateSpace::DOWN };

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
			static_cast<DynamicModelEntity* const RESTRICT>(_CurrentlySelectedEntity)->SetWorldTransform(world_transform);

			break;
		}

		case EntityType::Light:
		{
			LightEntity *const RESTRICT light_entity{ static_cast<LightEntity *const RESTRICT>(_CurrentlySelectedEntity) };

			switch (light_entity->GetLightType())
			{
				case LightType::DIRECTIONAL:
				{
					//Directional lights have no world transform, so just leave empty. (:

					break;
				}

				case LightType::POINT:
				{
					light_entity->SetWorldPosition(world_transform.GetWorldPosition());

					break;
				}

				default:
				{
					ASSERT(false, "Invalid case!");

					break;
				}
			}

			break;
		}

		case EntityType::StaticModel:
		{
			static_cast<StaticModelEntity* const RESTRICT>(_CurrentlySelectedEntity)->SetWorldTransform(world_transform);

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