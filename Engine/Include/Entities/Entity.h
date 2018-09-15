#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/Containers/DynamicArray.h>

//Math.
#include <Math/Vector3.h>

//Rendering.
#include <Rendering/Engine/AxisAlignedBoundingBox.h>

class Entity
{

public:

	//Enumeration covering all entity types.
	enum class EntityType : uint8
	{
		DynamicPhysical,
		Terrain,

		NumberOfEntityTypes
	};

	//The type.
	EntityType _Type{ EntityType::NumberOfEntityTypes };

	//Denotes whether or not this entity is initialized.
	bool _Initialized{ false };

	//The components index for this entity.
	uint64 _ComponentsIndex;

	//Pointer to this entity's parent.
	Entity *RESTRICT _Parent{ nullptr };

	//Pointers to this entity's children.
	DynamicArray<Entity *RESTRICT> _Children;

	/*
	*	Default destructor.
	*/
	virtual ~Entity() NOEXCEPT { }

	/*
	*	Returns the position of this entity.
	*/
	const Vector3& GetPosition() const NOEXCEPT;

	/*
	*	Returns the rotation of this entity.
	*/
	const Vector3& GetRotation() const NOEXCEPT;

	/*
	*	Returns the scale of this entity.
	*/
	const Vector3& GetScale() const NOEXCEPT;

	/*
	*	Sets the position of this entity.
	*/
	void SetPosition(const Vector3 &newPosition) NOEXCEPT;

	/*
	*	Sets the rotation of this entity.
	*/
	void SetRotation(const Vector3 &newRotation) NOEXCEPT;

	/*
	*	Sets the scale of this entity.
	*/
	void SetScale(const Vector3 &newScale) NOEXCEPT;

	/*
	*	Moves this entity.
	*/
	void Move(const Vector3 &amount) NOEXCEPT;

	/*
	*	Rotates this entity.
	*/
	void Rotate(const Vector3 &amount) NOEXCEPT;

	/*
	*	Scales this entity.
	*/
	void Scale(const Vector3 &amount) NOEXCEPT;

	/*
	*	Returns the forward vector of this entity.
	*/
	NO_DISCARD Vector3 GetForwardVector() const NOEXCEPT;

	/*
*	Returns the up vector of this entity.
*/
	NO_DISCARD Vector3 GetUpVector() const NOEXCEPT;

	/*
	*	Returns the right vector of this entity.
	*/
	NO_DISCARD Vector3 GetRightVector() const NOEXCEPT;

protected:

	/*
	*	Returns the position of this entity.
	*/
	RESTRICTED virtual NO_DISCARD Vector3 *const RESTRICT GetPositionInternal() NOEXCEPT
	{
		return nullptr;
	}

	/*
	*	Returns the rotation of this entity.
	*/
	RESTRICTED virtual NO_DISCARD Vector3 *const RESTRICT GetRotationInternal() NOEXCEPT
	{
		return nullptr;
	}

	/*
	*	Returns the scale of this entity.
	*/
	RESTRICTED virtual NO_DISCARD Vector3 *const RESTRICT GetScaleInternal() NOEXCEPT
	{
		return nullptr;
	}

	/*
	*	Callback for when this entity is moved.
	*/
	virtual void OnMove() NOEXCEPT
	{

	}

	/*
	*	Callback for when this entity is rotated.
	*/
	virtual void OnRotate() NOEXCEPT
	{

	}

	/*
	*	Callback for when this entity is scaled.
	*/
	virtual void OnScale() NOEXCEPT
	{

	}

private:

	//The default position.
	static constexpr Vector3 _DefaultPosition{ 0.0f, 0.0f, 0.0f };

	//The default rotation.
	static constexpr Vector3 _DefaultRotation{ 0.0f, 0.0f, 0.0f };

	//The default scale.
	static constexpr Vector3 _DefaultScale{ 1.0f, 1.0f, 1.0f };

};