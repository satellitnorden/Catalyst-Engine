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
*	Default constructor.
*/
MaximObject::MaximObject() NOEXCEPT
{
	//Create the entity.
	PhysicalModel model{ RenderingSystem::Instance->GetCommonPhysicalModel(RenderingSystem::CommonPhysicalModel::Octahedron) };
	model._Material = RenderingSystem::Instance->GetCommonPhysicalMaterial(RenderingSystem::CommonPhysicalMaterial::Teal);

	DynamicPhysicalInitializationData *const RESTRICT data{ EntitySystem::Instance->CreateInitializationData<DynamicPhysicalInitializationData>() };

	data->_PhysicalFlags = static_cast<uint8>(PhysicalFlag::Physical) | static_cast<uint8>(PhysicalFlag::Outline);
	data->_Model = model;
	data->_Position = Vector3(CatalystBaseMath::RandomFloatInRange(-2.5f, 2.5f), 5.0f, 0.0f);
	data->_Rotation = Vector3(0.0f, 0.0f, 0.0f);
	data->_Scale = Vector3(1.0f, 1.0f, 1.0f);

	_Entity = EntitySystem::Instance->CreateEntity<DynamicPhysicalEntity>();

	EntitySystem::Instance->RequestInitialization(_Entity, data, false);
}

/*
*	Pre-updates this Maxim object.
*/
void MaximObject::PreUpdate(const UpdateContext *const RESTRICT context) NOEXCEPT
{
	//Move the entity.
	_Entity->Move(Vector3(0.0f, -1.0f * context->_DeltaTime, 0.0f));
}