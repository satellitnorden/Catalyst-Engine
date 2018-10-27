//Header file.
#include <Entities/Entity.h>

//Systems.
#include <Systems/EntityCreationSystem.h>
#include <Systems/PhysicsSystem.h>

//Static variable definitions.
constexpr Vector3 Entity::_DefaultPosition;
constexpr Vector3 Entity::_DefaultRotation;
constexpr Vector3 Entity::_DefaultScale;

/*
*	Returns the position of this entity.
*/
const Vector3& Entity::GetPosition() const NOEXCEPT
{
	//Return the position of this entity.
	if (const Vector3 *const RESTRICT position{ const_cast<Entity *const RESTRICT>(this)->GetPositionInternal() })
	{
		return *position;
	}

	else
	{
		return _DefaultPosition;
	}
}

/*
*	Returns the rotation of this entity.
*/
const Vector3& Entity::GetRotation() const NOEXCEPT
{
	//Return the rotation of this entity.
	if (const Vector3 *const RESTRICT rotation{ const_cast<Entity *const RESTRICT>(this)->GetRotationInternal() })
	{
		return *rotation;
	}

	else
	{
		return _DefaultRotation;
	}
}

/*
*	Returns the scale of this entity.
*/
const Vector3& Entity::GetScale() const NOEXCEPT
{
	//Return the scale of this entity.
	if (const Vector3 *const RESTRICT scale{ const_cast<Entity *const RESTRICT>(this)->GetScaleInternal() })
	{
		return *scale;
	}

	else
	{
		return _DefaultScale;
	}
}

/*
*	Sets the position of this entity.
*/
void Entity::SetPosition(const Vector3 &newPosition) NOEXCEPT
{
	//Set the position of this entity.
	if (Vector3 *const RESTRICT position{ GetPositionInternal() })
	{
		//Calculate the amount to move and move all children.
		const Vector3 moveVector{ newPosition - *position };

		for (Entity *const RESTRICT child : _Children)
		{
			child->Move(moveVector);
		}

		//Set the position of this entity.
		*position = newPosition;

		//Call the callback.
		OnMove();
	}
}

/*
*	Sets the rotation of this entity.
*/
void Entity::SetRotation(const Vector3 &newRotation) NOEXCEPT
{
	//Set the rotation of this entity.
	if (Vector3 *const RESTRICT rotation{ GetRotationInternal() })
	{
		//Calculate the amount to rotate and rotate all children.
		const Vector3 rotateVector{ newRotation - *rotation };

		for (Entity *const RESTRICT child : _Children)
		{
			child->Rotate(rotateVector);
		}

		//Set the rotation of this entity.
		*rotation = newRotation;

		//Call the callback.
		OnRotate();
	}
}

/*
*	Sets the scale of this entity.
*/
void Entity::SetScale(const Vector3 &newScale) NOEXCEPT
{
	//Set the scale of this entity.
	if (Vector3 *const RESTRICT scale{ GetScaleInternal() })
	{
		//Calculate the amount to scale and scale all children.
		const Vector3 scaleVector{ newScale - *scale };

		for (Entity *const RESTRICT child : _Children)
		{
			child->Scale(scaleVector);
		}

		//Set the scale of this entity.
		*scale = newScale;

		//Call the callback.
		OnScale();
	}
}

/*
*	Moves this entity.
*/
void Entity::Move(const Vector3 &amount) NOEXCEPT
{
	//Move this entity.
	if (Vector3 *const RESTRICT position{ GetPositionInternal() })
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

/*
*	Rotates this entity.
*/
void Entity::Rotate(const Vector3 &amount) NOEXCEPT
{
	//Rotate this entity.
	if (Vector3 *const RESTRICT rotation{ GetRotationInternal() })
	{
		*rotation += amount;
	}

	//Rotate all children.
	for (Entity *const RESTRICT child : _Children)
	{
		child->Rotate(amount);
	}

	//Call the callback.
	OnRotate();
}

/*
*	Scales this entity.
*/
void Entity::Scale(const Vector3 &amount) NOEXCEPT
{
	//Scale this entity.
	if (Vector3 *const RESTRICT scale{ GetScaleInternal() })
	{
		*scale *= amount;
	}

	//Scale all children.
	for (Entity *const RESTRICT child : _Children)
	{
		child->Scale(amount);
	}

	//Call the callback.
	OnScale();
}

/*
*	Returns the forward vector of this entity.
*/
NO_DISCARD Vector3 Entity::GetForwardVector() const NOEXCEPT
{
	//Return the forward vector of this entity.
	if (const Vector3 *const RESTRICT rotation{ const_cast<Entity *const RESTRICT>(this)->GetRotationInternal() })
	{
		Vector3 forward{ Vector3::FORWARD };
		forward.Rotate(*rotation);

		return forward;
	}

	else
	{
		return Vector3::FORWARD;
	}
}

/*
*	Returns the up vector of this entity.
*/
NO_DISCARD Vector3 Entity::GetUpVector() const NOEXCEPT
{
	//Return the up vector of this entity.
	if (const Vector3 *const RESTRICT rotation{ const_cast<Entity *const RESTRICT>(this)->GetRotationInternal() })
	{
		Vector3 up{ Vector3::UP };
		up.Rotate(*rotation);

		return up;
	}

	else
	{
		return Vector3::UP;
	}
}

/*
*	Returns the right vector of this entity.
*/
NO_DISCARD Vector3 Entity::GetRightVector() const NOEXCEPT
{
	//Return the right vector of this entity.
	if (const Vector3 *const RESTRICT rotation{ const_cast<Entity *const RESTRICT>(this)->GetRotationInternal() })
	{
		Vector3 right{ Vector3::RIGHT };
		right.Rotate(*rotation);

		return right;
	}

	else
	{
		return Vector3::RIGHT;
	}
}