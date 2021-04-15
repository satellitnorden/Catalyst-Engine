#if defined(CATALYST_EDITOR)
//Header file.
#include <Editor/EditorEntitySystem.h>

//Core.
#include <Core/General/Perceiver.h>

//Entities.
#include <Entities/Creation/DynamicModelInitializationData.h>
#include <Entities/Creation/LightInitializationData.h>
#include <Entities/Creation/StaticModelInitializationData.h>
#include <Entities/Types/DynamicModelEntity.h>
#include <Entities/Types/LightEntity.h>
#include <Entities/Types/StaticModelEntity.h>

//Rendering.
#include <Rendering/Native/RenderingUtilities.h>

//Systems.
#include <Systems/CatalystEditorSystem.h>
#include <Systems/CatalystEngineSystem.h>
#include <Systems/EntitySystem.h>
#include <Systems/InputSystem.h>
#include <Systems/PhysicsSystem.h>
#include <Systems/ResourceSystem.h>

//Third party.
#include <ThirdParty/imgui.h>

/*
*	Updates the editor entity system.
*/
void EditorEntitySystem::Update() NOEXCEPT
{
	//Is the current contextual window ENTITIES?
	if (CatalystEditorSystem::Instance->GetCurrentContextualWindow() != CatalystEditorSystem::ContextualWindow::ENTITIES)
	{
		_IsCurrentlyCreatingEntity = false;

		return;
	}

	//Add the entities window.
	ImGui::Begin("Entities", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);
	ImGui::SetWindowPos(ImVec2(0.0f, 256.0f));
	ImGui::SetWindowSize(ImVec2(256.0f, 512.0f));

	//Add the button for creating a dynamic model entity.
	if (ImGui::Button("Create Dynamic Model Entity"))
	{
		if (_IsCurrentlyCreatingEntity)
		{
			_IsCurrentlyCreatingEntity = false;
		}

		else
		{
			_IsCurrentlyCreatingEntity = true;
			_EntityTypeBeingCreated = EntityType::DynamicModel;
		}
	}

	//Add the button for creating a light entity.
	if (ImGui::Button("Create Light Entity"))
	{
		if (_IsCurrentlyCreatingEntity)
		{
			_IsCurrentlyCreatingEntity = false;
		}

		else
		{
			_IsCurrentlyCreatingEntity = true;
			_EntityTypeBeingCreated = EntityType::Light;
		}
	}

	//Add the button for creating a static model entity.
	if (ImGui::Button("Create Static Model Entity"))
	{
		if (_IsCurrentlyCreatingEntity)
		{
			_IsCurrentlyCreatingEntity = false;
		}

		else
		{
			_IsCurrentlyCreatingEntity = true;
			_EntityTypeBeingCreated = EntityType::StaticModel;
		}
	}

	if (!ImGui::IsAnyWindowHovered() && !ImGui::IsAnyItemHovered() && _IsCurrentlyCreatingEntity)
	{
		//Cache if the left mouse button is pressed.
		const bool left_mouse_button_pressed{ InputSystem::Instance->GetMouseState()->_Left == ButtonState::PRESSED };

		if (left_mouse_button_pressed)
		{
			/*
			*	Cast a ray to get the position to place the new entity.
			*	If there was no hit, just spawn the new entity N meters in front of the perceiver.
			*/
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
				const Vector3<float32> hit_position{ ray._Origin + ray._Direction * result._HitDistance };

				CreateEntity(hit_position);
			}

			else
			{
				const Vector3<float32> hit_position{ ray._Origin + ray._Direction * 10.0f };

				CreateEntity(hit_position);
			}

			_IsCurrentlyCreatingEntity = false;
		}
	}

	ImGui::End();
}

/*
*	Creates a new entity at the given position.
*/
void EditorEntitySystem::CreateEntity(const Vector3<float32> &position)
{
	switch (_EntityTypeBeingCreated)
	{
		case EntityType::DynamicModel:
		{
			DynamicModelEntity *const RESTRICT entity{ EntitySystem::Instance->CreateEntity<DynamicModelEntity>() };
			DynamicModelInitializationData* const RESTRICT data{ EntitySystem::Instance->CreateInitializationData<DynamicModelInitializationData>() };

			data->_Properties = EntityInitializationData::Property::NONE;
			data->_InitialWorldTransform = WorldTransform(position, VectorConstants::ZERO, 1.0f);
			data->_ModelResource = ResourceSystem::Instance->GetModelResource(HashString("Catalyst_Engine_Default_Model"));

			for (uint8 i{ 0 }; i < RenderingConstants::MAXIMUM_NUMBER_OF_MESHES_PER_MODEL; ++i)
			{
				data->_MaterialResources[i] = ResourceSystem::Instance->GetMaterialResource(HashString("Catalyst_Engine_Default_Material"));
			}

			data->_ModelCollisionConfiguration._Type = ModelCollisionType::AXIS_ALIGNED_BOUNDING_BOX;
			data->_ModelSimulationConfiguration._SimulatePhysics = false;

			EntitySystem::Instance->RequestInitialization(entity, data, false);

			CatalystEditorSystem::Instance->GetEditorSelectionSystem()->SetCurrentlySelectedEntityOverride(entity);

			break;
		}

		case EntityType::Light:
		{
			LightEntity *const RESTRICT entity{ EntitySystem::Instance->CreateEntity<LightEntity>() };
			LightInitializationData* const RESTRICT data{ EntitySystem::Instance->CreateInitializationData<LightInitializationData>() };

			data->_Properties = EntityInitializationData::Property::NONE;
			data->_Color = Vector3<float32>(1.0f, 1.0f, 1.0f);
			data->_LightType = LightType::POINT;
			data->_LightProperties = 0;
			data->_Intensity = 1.0f;
			data->_Radius = 8.0f;
			data->_Size = 1.0f;

			EntitySystem::Instance->RequestInitialization(entity, data, false);

			CatalystEditorSystem::Instance->GetEditorSelectionSystem()->SetCurrentlySelectedEntityOverride(entity);

			break;
		}

		case EntityType::StaticModel:
		{
			StaticModelEntity *const RESTRICT entity{ EntitySystem::Instance->CreateEntity<StaticModelEntity>() };
			StaticModelInitializationData* const RESTRICT data{ EntitySystem::Instance->CreateInitializationData<StaticModelInitializationData>() };

			data->_Properties = EntityInitializationData::Property::NONE;
			data->_WorldTransform = WorldTransform(position, VectorConstants::ZERO, 1.0f);
			data->_ModelResource = ResourceSystem::Instance->GetModelResource(HashString("Catalyst_Engine_Default_Model"));

			for (uint8 i{ 0 }; i < RenderingConstants::MAXIMUM_NUMBER_OF_MESHES_PER_MODEL; ++i)
			{
				data->_MaterialResources[i] = ResourceSystem::Instance->GetMaterialResource(HashString("Catalyst_Engine_Default_Material"));
			}

			data->_ModelCollisionConfiguration._Type = ModelCollisionType::AXIS_ALIGNED_BOUNDING_BOX;

			EntitySystem::Instance->RequestInitialization(entity, data, false);

			CatalystEditorSystem::Instance->GetEditorSelectionSystem()->SetCurrentlySelectedEntityOverride(entity);

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