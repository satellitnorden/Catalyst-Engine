//Header file.
#include <Entities/Types/Entity.h>

//Math.
#include <Math/CatalystVectorMath.h>

//Systems.
#include <Systems/EntityCreationSystem.h>
#include <Systems/PhysicsSystem.h>

//Static variable definitions.
constexpr Vector3<float> Entity::_DefaultPosition;
constexpr Vector3<float> Entity::_DefaultRotation;
constexpr Vector3<float> Entity::_DefaultScale;

/*
*	Moves this entity.
*/
void Entity::Move(const Vector3<float> &amount) NOEXCEPT
{
	//Move this entity.
	if (Vector3<float> *const RESTRICT position{ GetPositionInternal() })
	{
		*position += amount;
	}

	//Move all children.
	for (Entity *const RESTRICT child : _Children)
	{
		child->Move(amount);
	}

	//Call the callback.
	OnMove();
}