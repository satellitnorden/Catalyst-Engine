//Header file.
#include <Main/MaximObject.h>

//Components.
#include <Components/ComponentManager.h>

//Entities.
#include <Entities/InitializationData/DynamicPhysicalInitializationData.h>

//Math.
#include <Math/CatalystBaseMath.h>

//Systems.
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
	PhysicalModel model{ RenderingSystem::Instance->GetCommonPhysicalModel(RenderingSystem::CommonPhysicalModel::Octahedron) };
	model._Material = RenderingSystem::Instance->GetCommonPhysicalMaterial(RenderingSystem::CommonPhysicalMaterial::Black);

	DynamicPhysicalInitializationData *const RESTRICT data{ EntitySystem::Instance->CreateInitializationData<DynamicPhysicalInitializationData>() };

	data->_PhysicalFlags = static_cast<uint8>(PhysicalFlag::Physical) | static_cast<uint8>(PhysicalFlag::Outline);
	data->_Model = model;

	switch (CatalystBaseMath::RandomIntegerInRange(0, 4))
	{
		case 0:
		{
			data->_Position = Vector3(-2.5f, 10.0f, 0.0f);

			break;
		}

		case 1:
		{
			data->_Position = Vector3(-1.25f, 10.0f, 0.0f);

			break;
		}

		case 2:
		{
			data->_Position = Vector3(0.0f, 10.0f, 0.0f);

			break;
		}

		case 3:
		{
			data->_Position = Vector3(1.25f, 10.0f, 0.0f);

			break;
		}

		case 4:
		{
			data->_Position = Vector3(2.5f, 10.0f, 0.0f);

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
*	Pre-updates this Maxim object asynchronously.
*/
bool MaximObject::PreUpdateAsynchronous(const UpdateContext *const RESTRICT context) NOEXCEPT
{
	if (_Entity->IsInitialized())
	{
		//Move the entity.
		_Entity->Move(Vector3(0.0f, -_Speed * context->_DeltaTime, 0.0f));

		if (_Entity->GetPosition()._Y <= -2.5f)
		{
			//Destroy this object.
			MaximGameSystem::Instance->DestroyMaximObject(this);

			_IsDestroyed = true;
		}
	}

	return !_IsDestroyed;
}

/*
*	Updates this Maxim object asynchronously.
*/
bool MaximObject::UpdateAsynchronous(const UpdateContext *const RESTRICT context) NOEXCEPT
{
	if (_Entity->IsInitialized())
	{
		const FrustumCullingComponent &cullingComponent{ ComponentManager::GetDynamicPhysicalFrustumCullingComponents()[_Entity->GetComponentsIndex()] };
		const TransformComponent &transformComponent{ ComponentManager::GetDynamicPhysicalTransformComponents()[_Entity->GetComponentsIndex()] };

		AxisAlignedBoundingBox box{ cullingComponent._AxisAlignedBoundingBox };
		box._Minimum += transformComponent._Position;
		box._Maximum += transformComponent._Position;

		if (RenderingSystem::Instance->IsClockedOrTouched(box))
		{
			_Entity->SetPosition(Vector3(0.0f, 0.0f, 0.0f));
		}
	}

	return !_IsDestroyed;
}