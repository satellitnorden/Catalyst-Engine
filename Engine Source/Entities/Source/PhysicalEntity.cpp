//Header file.
#include <PhysicalEntity.h>

//Entities.
#include <CameraEntity.h>

//Systems.
#include <GraphicsSystem.h>

//Static variable definitions.
DynamicArray<PhysicalEntity *CATALYST_RESTRICT> PhysicalEntity::physicalEntities;

/*
*	Default constructor.
*/
PhysicalEntity::PhysicalEntity() CATALYST_NOEXCEPT
{
	//Add this physical entity to the universal container.
	physicalEntities.Emplace(this);
}

/*
*	Default destructor.
*/
PhysicalEntity::~PhysicalEntity() CATALYST_NOEXCEPT
{
	//Remove this physical entity from the universal container.
	physicalEntities.Erase(this);
}

/*
*	Initializes this physical entity.
*/
void PhysicalEntity::Initialize(const PhysicalModel &newModel) CATALYST_NOEXCEPT
{
	//Set the model extent.
	modelExtent = newModel.GetExtent();

	//Initialize this physical entity.
	GraphicsSystem::Instance->InitializePhysicalEntity(*this, newModel);
}

/*
*	Updates the model matrix.
*/
void PhysicalEntity::UpdateModelMatrix() CATALYST_NOEXCEPT
{
	//Calculate the new model matrix.
	modelMatrix = Matrix4(GetWorldPosition(), GetWorldRotation(), GetWorldScale());

	//Upload the graphics matrix to the uniform buffer.
	uniformBuffer->UploadData(static_cast<void *CATALYST_RESTRICT>(&modelMatrix));
}