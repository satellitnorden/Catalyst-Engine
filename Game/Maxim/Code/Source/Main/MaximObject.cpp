//Header file.
#include <Main/MaximObject.h>

//Entities.
#include <Entities/InitializationData/DynamicPhysicalInitializationData.h>

//Math.
#include <Math/CatalystBaseMath.h>

//Systems.
#include <Systems/EntitySystem.h>
#include <Systems/RenderingSystem.h>

/*
*	Initializes this Maxim object.
*/
void MaximObject::Initialize(const float initialSpeed) NOEXCEPT
{
	//Set the speed.
	_Speed = initialSpeed;

	//Create the entity.
	PhysicalModel model{ RenderingSystem::Instance->GetCommonPhysicalModel(RenderingSystem::CommonPhysicalModel::Octahedron) };
	model._Material = RenderingSystem::Instance->GetCommonPhysicalMaterial(RenderingSystem::CommonPhysicalMaterial::Teal);

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
	data->_Scale = Vector3(1.0f, 1.0f, 1.0f);

	_Entity = EntitySystem::Instance->CreateEntity<DynamicPhysicalEntity>();

	EntitySystem::Instance->RequestInitialization(_Entity, data, true);
}

/*
*	Pre-updates this Maxim object.
*/
void MaximObject::PreUpdate(const UpdateContext *const RESTRICT context) NOEXCEPT
{
	//Move the entity.
	_Entity->Move(Vector3(0.0f, -_Speed * context->_DeltaTime, 0.0f));
}