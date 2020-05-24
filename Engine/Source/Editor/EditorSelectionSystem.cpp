#if defined(CATALYST_EDITOR)
//Header file.
#include <Editor/EditorSelectionSystem.h>

//Core.
#include <Core/General/Perceiver.h>

//Entities.
#include <Entities/Types/DynamicModelEntity.h>

//Rendering.
#include <Rendering/Native/RenderingUtilities.h>

//Systems.
#include <Systems/CatalystEditorSystem.h>
#include <Systems/CatalystEngineSystem.h>
#include <Systems/InputSystem.h>
#include <Systems/PhysicsSystem.h>
#include <Systems/RenderingSystem.h>
#include <Systems/ResourceSystem.h>

/*
*	Initializes the editor selection system.
*/
void EditorSelectionSystem::Initialize() NOEXCEPT
{
	//Register the update.
	CatalystEngineSystem::Instance->RegisterUpdate([](void* const RESTRICT arguments)
	{
		static_cast<EditorSelectionSystem *const RESTRICT>(arguments)->PhysicsUpdate();
	},
	this,
	UpdatePhase::PHYSICS,
	UpdatePhase::RENDER,
	false);
}

/*
*	Updates the editor selection system during the PHYSICS update phase.
*/
void EditorSelectionSystem::PhysicsUpdate() NOEXCEPT
{
	//Only update if the editor isn't in a game.
	if (CatalystEditorSystem::Instance->IsInGame())
	{
		return;
	}

	//Cache if the left mouse button is pressed.
	const bool left_mouse_button_pressed{ InputSystem::Instance->GetMouseState()->_Left == ButtonState::PRESSED };

	//Cast a ray from the mouse position and forward.
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
				if (left_mouse_button_pressed)
				{
					_CurrentlySelectedEntity = result._DynamicModelRaycastResult._Entity;
				}

				else
				{
					_CurrentlyHoveredEntity = result._DynamicModelRaycastResult._Entity;
				}

				break;
			}

			case RaycastResult::Type::STATIC_MODEL:
			{
				if (left_mouse_button_pressed)
				{
					_CurrentlySelectedEntity = result._StaticModelRaycastResult._Entity;
				}

				else
				{
					_CurrentlyHoveredEntity = result._StaticModelRaycastResult._Entity;
				}

				break;
			}

			case RaycastResult::Type::TERRAIN:
			{


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
		//Reset the currently hovered entity.
		_CurrentlyHoveredEntity = nullptr;

		//If the left mouse button was pressed without any hit, reset the currently selected entity.
		if (left_mouse_button_pressed)
		{
			_CurrentlySelectedEntity = nullptr;
		}
	}

	//Render a bounding box for the currently hovered entity.
	if (_CurrentlyHoveredEntity && _CurrentlyHoveredEntity->_Initialized && _CurrentlyHoveredEntity != _CurrentlySelectedEntity)
	{
		switch (_CurrentlyHoveredEntity->_Type)
		{
			case EntityType::DynamicModel:
			{
				AxisAlignedBoundingBox3 box{ *static_cast<DynamicModelEntity *const RESTRICT>(_CurrentlyHoveredEntity)->GetWorldSpaceAxisAlignedBoundingBox() };

				box._Minimum -= 0.1f;
				box._Maximum += 0.1f;

				RenderingSystem::Instance->GetDebugRenderingSystem()->DebugRenderAxisAlignedBoundingBox3D(Vector4<float32>(0.0f, 0.0f, 1.0f, 0.25f * 0.5f), true, true, box, 0.0f);
				RenderingSystem::Instance->GetDebugRenderingSystem()->DebugRenderAxisAlignedBoundingBox3D(Vector4<float32>(0.0f, 0.0f, 1.0f, 0.25f * 0.5f * 0.125f), true, false, box, 0.0f);

				break;
			}
		}
	}

	//Render a bounding box for the currently selected entity.
	if (_CurrentlySelectedEntity && _CurrentlySelectedEntity->_Initialized)
	{
		switch (_CurrentlySelectedEntity->_Type)
		{
			case EntityType::DynamicModel:
			{
				AxisAlignedBoundingBox3 box{ *static_cast<DynamicModelEntity *const RESTRICT>(_CurrentlySelectedEntity)->GetWorldSpaceAxisAlignedBoundingBox() };

				box._Minimum -= 0.1f;
				box._Maximum += 0.1f;

				RenderingSystem::Instance->GetDebugRenderingSystem()->DebugRenderAxisAlignedBoundingBox3D(Vector4<float32>(0.0f, 1.0f, 1.0f, 0.25f), true, true, box, 0.0f);
				RenderingSystem::Instance->GetDebugRenderingSystem()->DebugRenderAxisAlignedBoundingBox3D(Vector4<float32>(0.0f, 1.0f, 1.0f, 0.25f * 0.125f), true, false, box, 0.0f);

				break;
			}
		}
	}
}
#endif