//Header file.
#include <Main/MaximObject.h>

//Components.
#include <Components/ComponentManager.h>

//Entities.
#include <Entities/InitializationData/DynamicPhysicalInitializationData.h>

//Math.
#include <Math/CatalystBaseMath.h>

//Systems.
#if !defined(CATALYST_FINAL)
#include <Systems/DebugRenderingSystem.h>
#endif
#include <Systems/EntitySystem.h>
#include <Systems/RenderingSystem.h>

//Maxim.
#include <Main/MaximGameSystem.h>

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
	if (_Entity->IsInitialized())
	{
#if !defined(CATALYST_FINAL)
		//Debug render the axis-aligned bounding box.
		//DebugRenderingSystem::Instance->DebugRenderAxisAlignedBoundingBox(DebugRenderingSystem::AxisAlignedBoundingBoxDebugRenderData(*_Entity->GetWorldSpaceAxisAlignedBoundingBox(), Vector4(1.0f, 0.0f, 0.0f, 0.5f)));
#endif
		if (RenderingSystem::Instance->IsClockedOrTouched(*_Entity->GetWorldSpaceAxisAlignedBoundingBox()))
		{
			_Entity->Move(Vector3(0.0f, 2.5f, 0.0f));
		}

		else
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