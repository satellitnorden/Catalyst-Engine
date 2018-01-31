#pragma once

//Engine core.
#include <EngineCore.h>

//Components.
#include <PhysicsComponent.h>

//Math.
#include <Vector3.h>

//Multithreading.
#include <MultithreadedResource.h>

class Entity
{

public:

	//Universal container of all entities.
	static DynamicArray<Entity *CATALYST_RESTRICT> instances;

	/*
	*	Default constructor.
	*/
	Entity() CATALYST_NOEXCEPT;

	/*
	*	Default destructor.
	*/
	virtual ~Entity() CATALYST_NOEXCEPT;

	/*
	*	Returns a pointer to the parent of this entity.
	*/
	CATALYST_RESTRICTED const Entity* GetParent() const CATALYST_NOEXCEPT { return parent; }

	/*
	*	Sets the parent of this entity.
	*/
	void SetParent(Entity *CATALYST_RESTRICT newParent) CATALYST_NOEXCEPT { parent = newParent; }

	/*
	*	Returns the children of this entity.
	*/
	const DynamicArray<Entity *CATALYST_RESTRICT>& GetChildren() const CATALYST_NOEXCEPT { return children; }

	/*
	*	Adds a child to this entity.
	*/
	void AddChild(Entity *CATALYST_RESTRICT newChildEntity) CATALYST_NOEXCEPT { children.Emplace(newChildEntity); }

	/*
	*	Marks this entitiy for destruction.
	*/
	void MarkForDestruction() CATALYST_NOEXCEPT;

	/*
	*	Returns the world position of this entity.
	*/
	Vector3 GetWorldPosition() const CATALYST_NOEXCEPT;

	/*
	*	Returns the world rotation of this entity.
	*/
	Vector3 GetWorldRotation() const CATALYST_NOEXCEPT;

	/*
	*	Returns the world position of this entity.
	*/
	Vector3 GetWorldScale() const CATALYST_NOEXCEPT;

	/*
	*	Returns the local rotation of this entity, const.
	*/
	const Vector3& GetLocalRotation() const CATALYST_NOEXCEPT { return localRotation; }

	/*
	*	Returns the local scale of this entity, const.
	*/
	const Vector3& GetLocalScale() const CATALYST_NOEXCEPT { return localScale; }

	/*
	*	Returns the local position of this entity, non-const.
	*/
	Vector3 GetLocalPosition() const CATALYST_NOEXCEPT { return localPosition.GetSafe(); }

	/*
	*	Returns the local rotation of this entity, non-const.
	*/
	Vector3 GetLocalRotation() CATALYST_NOEXCEPT { return localRotation; }

	/*
	*	Returns the local scale of this entity, non-const.
	*/
	Vector3 GetLocalScale() CATALYST_NOEXCEPT { return localScale; }

	/*
	*	Sets the local position of this entity.
	*/
	void SetLocalPosition(const Vector3 &newLocalPosition) CATALYST_NOEXCEPT { localPosition.Set(newLocalPosition); }

	/*
	*	Sets the local rotation of this entity.
	*/
	void SetLocalRotation(const Vector3 &newLocalRotation) CATALYST_NOEXCEPT { localRotation = newLocalRotation; }

	/*
	*	Sets the local scale of this entity.
	*/
	void SetLocalScale(const Vector3 &newLocalScale) CATALYST_NOEXCEPT { localScale = newLocalScale; }

	/*
	*	Move the local position by a certain amount.
	*/
	void Move(const Vector3 &moveVector) CATALYST_NOEXCEPT { localPosition.Set(localPosition.GetSafe() + moveVector); }

	/*
	*	Rotate the local rotation by a certain amount.
	*/
	void Rotate(const Vector3 &rotateVector) CATALYST_NOEXCEPT { localRotation += rotateVector; }

	/*
	*	Scale the local scale by a certain amount.
	*/
	void Scale(const Vector3 &scaleVector) CATALYST_NOEXCEPT { localScale *= scaleVector; }

	/*
	*	Returns the forward vector.
	*/
	Vector3 GetForwardVector() const CATALYST_NOEXCEPT { return Vector3(0.0f, 0.0f, -1.0f).Rotated(GetWorldRotation()); }

	/*
	*	Returns the right vector.
	*/
	Vector3 GetRightVector() const CATALYST_NOEXCEPT { return Vector3(1.0f, 0.0f, 0.0f).Rotated(GetWorldRotation()); }

	/*
	*	Returns the up vector.
	*/
	Vector3 GetUpVector() const CATALYST_NOEXCEPT { return Vector3(0.0f, 1.0f, 0.0f).Rotated(GetWorldRotation()); }

	/*
	*	Initializes physics for this entity.
	*/
	void InitializePhysics(const float initialMass, const Vector3 &initialVelocity) CATALYST_NOEXCEPT;

	/*
	*	Returns the physics component, const, safe.
	*/
	CATALYST_RESTRICTED const PhysicsComponent* GetPhysicsComponentSafe() const { const auto iterator = components.Find(ComponentType::PhysicsComponent); if (iterator) return *reinterpret_cast<const PhysicsComponent *const CATALYST_RESTRICT * CATALYST_RESTRICT>(iterator); else return nullptr; }

	/*
	*	Returns the physics component, non-const, safe.
	*/
	CATALYST_RESTRICTED PhysicsComponent* GetPhysicsComponentSafe() { const auto iterator = components.Find(ComponentType::PhysicsComponent); if (iterator) return *reinterpret_cast<PhysicsComponent *CATALYST_RESTRICT * CATALYST_RESTRICT>(iterator); else return nullptr; }

	/*
	*	Returns the physics component, const, unsafe.
	*/
	CATALYST_RESTRICTED const PhysicsComponent* GetPhysicsComponentUnsafe() const { return *reinterpret_cast<const PhysicsComponent *const CATALYST_RESTRICT * CATALYST_RESTRICT>(components.Find(ComponentType::PhysicsComponent)); }
	
	/*
	*	Returns the physics component, non-const, unsafe.
	*/
	CATALYST_RESTRICTED PhysicsComponent* GetPhysicsComponentUnsafe() { return *reinterpret_cast<PhysicsComponent * CATALYST_RESTRICT * CATALYST_RESTRICT>(components.Find(ComponentType::PhysicsComponent)); }

protected:

	//Pointer to this entity's parent.
	Entity *CATALYST_RESTRICT parent{ nullptr };

	//Pointers to this entity's children.
	DynamicArray<Entity *CATALYST_RESTRICT> children;

	//The local position of this world entity.
	MultithreadedResource<Vector3> localPosition{ 0.0f, 0.0f, 0.0f };

	//The local rotation of this world entity.
	Vector3 localRotation{ 0.0f, 0.0f, 0.0f };

	//The local scale of this world entity.
	Vector3 localScale{ 1.0f, 1.0f, 1.0f };

private:

	//Friend declarations.
	friend class EntitySystem;

	//Container of all components.
	Map<ComponentType, void *CATALYST_RESTRICT> components;

	/*
	*	Destroys this entity.
	*/
	void Destroy() CATALYST_NOEXCEPT;

};