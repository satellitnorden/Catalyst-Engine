#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/Containers/DynamicArray.h>

//Math.
#include <Math/Vector3.h>

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

	/*
	*	Default destructor.
	*/
	virtual ~Entity() NOEXCEPT { }

	/*
	*	Returns the component index.
	*/
	uint64 GetComponentsIndex() const NOEXCEPT { return _ComponentsIndex; }

	/*
	*	Sets the component index.
	*/
	void SetComponentsIndex(const uint64 newComponentsIndex) NOEXCEPT { _ComponentsIndex = newComponentsIndex; }

	/*
	*	Returns the entity type.
	*/
	EntityType GetEntityType() const NOEXCEPT { return entityType; }

	/*
	*	Returns whether or not this entity is initialized.
	*/
	bool IsInitialized() const NOEXCEPT { return _IsInitialized; }

	/*
	*	Sets whether or not this entity is initialized.
	*/
	void SetIsInitialized(const bool newIsInitialized) NOEXCEPT { _IsInitialized = newIsInitialized; }

	/*
	*	Returns a pointer to the parent of this entity.
	*/
	RESTRICTED const Entity* GetParent() const NOEXCEPT { return _Parent; }

	/*
	*	Sets the parent of this entity.
	*/
	void SetParent(Entity *RESTRICT newParent) NOEXCEPT { _Parent = newParent; }

	/*
	*	Returns the children of this entity.
	*/
	const DynamicArray<Entity *RESTRICT>& GetChildren() const NOEXCEPT { return _Children; }

	/*
	*	Adds a child to this entity.
	*/
	void AddChild(Entity *RESTRICT newChildEntity) NOEXCEPT { _Children.EmplaceSlow(newChildEntity); }

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

	//The default position.
	static constexpr Vector3 _DefaultPosition{ 0.0f, 0.0f, 0.0f };

	//The default rotation.
	static constexpr Vector3 _DefaultRotation{ 0.0f, 0.0f, 0.0f };

	//The default scale.
	static constexpr Vector3 _DefaultScale{ 1.0f, 1.0f, 1.0f };

	//The entity type.
	EntityType entityType{ EntityType::NumberOfEntityTypes };

	//Denotes whether or not this entity is initialized.
	bool _IsInitialized{ false };

	//The components index for this entity.
	uint64 _ComponentsIndex;

	//Pointer to this entity's parent.
	Entity *RESTRICT _Parent{ nullptr };

	//Pointers to this entity's children.
	DynamicArray<Entity *RESTRICT> _Children;

	/*
	*	Returns the position of this entity.
	*/
	NO_DISCARD RESTRICTED virtual Vector3 *const RESTRICT GetPositionInternal() NOEXCEPT
	{
		return nullptr;
	}

	/*
	*	Returns the rotation of this entity.
	*/
	NO_DISCARD RESTRICTED virtual Vector3 *const RESTRICT GetRotationInternal() NOEXCEPT
	{
		return nullptr;
	}

	/*
	*	Returns the scale of this entity.
	*/
	NO_DISCARD RESTRICTED virtual Vector3 *const RESTRICT GetScaleInternal() NOEXCEPT
	{
		return nullptr;
	}

};