//Header file.
#include <Main/MaximObject.h>

//Components.
#include <Components/ComponentManager.h>

//Entities.
#include <Entities/InitializationData/DynamicPhysicalInitializationData.h>

//Math.
#include <Math/CatalystBaseMath.h>
#include <Math/CatalystVectorMath.h>

//Systems.
#if !defined(CATALYST_FINAL)
#include <Systems/DebugRenderingSystem.h>
#endif
#include <Systems/EntitySystem.h>
#include <Systems/InputSystem.h>
#include <Systems/RenderingSystem.h>

//Maxim.
#include <Main/MaximGameSystem.h>

//Static variable definitions.
float MaximObject::_Speed = 1.0f;

/*
*	Initializes this Maxim object.
*/
void MaximObject::Initialize(const MaximColor initialColor, const Vector3& initialOutlineColor, const float initialSpeed) NOEXCEPT
{
	//Set the color.
	_Color = initialColor;

	//Set the speed.
	_Speed = initialSpeed;

	//Create the entity.
	DynamicPhysicalInitializationData *const RESTRICT data{ EntitySystem::Instance->CreateInitializationData<DynamicPhysicalInitializationData>() };

	data->_PhysicalFlags = static_cast<uint8>(PhysicalFlag::Physical) | static_cast<uint8>(PhysicalFlag::Outline);
	data->_Model = RenderingSystem::Instance->GetCommonPhysicalModel(RenderingSystem::CommonPhysicalModel::Octahedron);
	data->_Material = RenderingSystem::Instance->GetCommonPhysicalMaterial(RenderingSystem::CommonPhysicalMaterial::Black);
	
	switch (CatalystBaseMath::RandomIntegerInRange(0, 4))
	{
		case 0:
		{
			data->_Position = Vector3(-2.5f, 7.5f, 0.0f);

			break;
		}

		case 1:
		{
			data->_Position = Vector3(-1.25f, 7.5f, 0.0f);

			break;
		}

		case 2:
		{
			data->_Position = Vector3(0.0f, 7.5f, 0.0f);

			break;
		}

		case 3:
		{
			data->_Position = Vector3(1.25f, 7.5f, 0.0f);

			break;
		}

		case 4:
		{
			data->_Position = Vector3(2.5f, 7.5f, 0.0f);

			break;
		}
	}

	data->_Rotation = Vector3(0.0f, 0.0f, 0.0f);
	data->_Scale = Vector3(0.75f, 1.0f, 0.75f);
	data->_OutlineColor = initialOutlineColor;

	_Entity = EntitySystem::Instance->CreateEntity<DynamicPhysicalEntity>();

	EntitySystem::Instance->RequestInitialization(_Entity, data, true);
}

/*
*	Logic updates this Maxim object asynchronously.
*/
bool MaximObject::LogicUpdateAsynchronous(const UpdateContext *const RESTRICT context) NOEXCEPT
{
	if (_Entity->_Initialized)
	{
		if (_IsSelected)
		{
			UpdateSelectedPosition();
		}

		if (!_IsSelected)
		{
			//Move the entity.
			_Entity->Move(Vector3(0.0f, -_Speed * context->_DeltaTime, 0.0f));

			if (_Entity->GetPosition()._Y <= -5.0f)
			{
				//Destroy this object.
				MaximGameSystem::Instance->DestroyMaximObject(this);

				_IsDestroyed = true;
			}
		}
	}

	return !_IsDestroyed;
}

/*
*	Returns the position for a selected Maxim object.
*/
void MaximObject::UpdateSelectedPosition() NOEXCEPT
{
#if defined(CATALYST_WINDOWS)
	const MouseState *const RESTRICT state{ InputSystem::Instance->GetMouseState() };
	const ButtonState buttonState{ state->_Left };
#elif defined(CATALYST_ANDROID)
	const TouchState *const RESTRICT state{ InputSystem::Instance->GetTouchState() };
	const ButtonState buttonState{ state->_ButtonState };
#endif
	if (buttonState == ButtonState::Pressed || buttonState == ButtonState::PressedHold)
	{
		const Vector3 newPosition{ CatalystVectorMath::LinePlaneIntersection(	Vector3(0.0f, 0.0f, 0.0f),
																				RenderingSystem::Instance->GetActiveCamera()->GetPosition(),
																				Vector3::BACKWARD,
																				RenderingSystem::Instance->GetWorldDirectionFromScreenCoordinate(Vector2(state->_CurrentX, state->_CurrentY))) };

		_Entity->SetPosition(newPosition);
	}

	else
	{
		_IsSelected = false;
	}
}