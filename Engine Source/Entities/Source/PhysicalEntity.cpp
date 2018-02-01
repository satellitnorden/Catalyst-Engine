//Header file.
#include <PhysicalEntity.h>

//Components.
#include <ComponentManager.h>

//Entities.
#include <CameraEntity.h>

//Systems.
#include <GraphicsSystem.h>

//Static variable definitions.
DynamicArray<PhysicalEntity *CATALYST_RESTRICT> PhysicalEntity::instances;

/*
*	Default constructor.
*/
PhysicalEntity::PhysicalEntity() CATALYST_NOEXCEPT
{
	//Get a new components index.
	componentsIndex = ComponentManager::GetNewPhysicalEntityComponentsIndex();

	//Add this physical entity to the universal container.
	instances.Emplace(this);
}

/*
*	Default destructor.
*/
PhysicalEntity::~PhysicalEntity() CATALYST_NOEXCEPT
{
	//Remove this physical entity from the universal container.
	instances.Erase(this);
}

/*
*	Initializes this physical entity.
*/
void PhysicalEntity::Initialize(const PhysicalModel &newModel) CATALYST_NOEXCEPT
{
	//Initialize this physical entity.
	GraphicsSystem::Instance->InitializePhysicalEntity(*this, newModel);
}

/*
*	Moves this physical entity.
*/
void PhysicalEntity::Move(const Vector3 &moveVector) CATALYST_NOEXCEPT
{
	//Move this physical entity.
	ComponentManager::GetPhysicalEntityTransformComponents()[componentsIndex].position += moveVector;
}

/*
*	Rotates this physical entity.
*/
void PhysicalEntity::Rotate(const Vector3 &rotateVector) CATALYST_NOEXCEPT
{
	//Rotate this physical entity.
	ComponentManager::GetPhysicalEntityTransformComponents()[componentsIndex].rotation += rotateVector;
}

/*
*	Scales this physical entity.
*/
void PhysicalEntity::Scale(const Vector3 &scaleVector) CATALYST_NOEXCEPT
{
	//Scale this physical entity.
	ComponentManager::GetPhysicalEntityTransformComponents()[componentsIndex].scale *= scaleVector;
}

/*
*	Updates the model matrix.
*/
void PhysicalEntity::UpdateModelMatrix() CATALYST_NOEXCEPT
{
	//Calculate the new model matrix.
	const TransformComponent &transformComponent{ ComponentManager::GetPhysicalEntityTransformComponents()[componentsIndex] };
	modelMatrix = Matrix4(transformComponent.position, transformComponent.rotation, transformComponent.scale);

	//Upload the graphics matrix to the uniform buffer.
	uniformBuffer->UploadData(static_cast<void *CATALYST_RESTRICT>(&modelMatrix));
}