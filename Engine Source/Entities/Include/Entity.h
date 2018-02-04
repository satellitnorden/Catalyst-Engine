#pragma once

//Engine core.
#include <EngineCore.h>

//Math.
#include <Vector3.h>

//Declares an entity class.
#define DECLARE_ENTITY_CLASS(EntityClass) static DynamicArray<EntityClass *CATALYST_RESTRICT> Instances;

//Defines an entity class.
#define DEFINE_ENTITY_CLASS(EntityClass) DynamicArray<EntityClass *CATALYST_RESTRICT> EntityClass::Instances;

class Entity
{

public:

	//Declare the entity class.
	DECLARE_ENTITY_CLASS(Entity);

	/*
	*	Default constructor.
	*/
	Entity() CATALYST_NOEXCEPT;

	/*
	*	Default destructor.
	*/
	virtual ~Entity() CATALYST_NOEXCEPT;

	/*
	*	Returns the component index.
	*/
	size_t GetComponentsIndex() const CATALYST_NOEXCEPT { return componentsIndex; }

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
	*	Returns the position of this entity.
	*/
	virtual const Vector3& GetPosition() CATALYST_NOEXCEPT = 0;

	/*
	*	Returns the rotation of this entity.
	*/
	virtual const Vector3& GetRotation() CATALYST_NOEXCEPT = 0;

	/*
	*	Returns the scale of this entity.
	*/
	virtual const Vector3& GetScale() CATALYST_NOEXCEPT = 0;

	/*
	*	Moves this entity.
	*/
	virtual void Move(const Vector3 &moveVector) CATALYST_NOEXCEPT = 0;

	/*
	*	Rotates this entity.
	*/
	virtual void Rotate(const Vector3 &rotateVector) CATALYST_NOEXCEPT = 0;

	/*
	*	Scales this entity.
	*/
	virtual void Scale(const Vector3 &scaleVector) CATALYST_NOEXCEPT = 0;

protected:

	//The default position.
	static constexpr Vector3 defaultPosition{ 0.0f, 0.0f, 0.0f };

	//The default rotation.
	static constexpr Vector3 defaultRotation{ 0.0f, 0.0f, 0.0f };

	//The default scale.
	static constexpr Vector3 defaultScale{ 1.0f, 1.0f, 1.0f };

	//Pointer to this entity's parent.
	Entity *CATALYST_RESTRICT parent{ nullptr };

	//Pointers to this entity's children.
	DynamicArray<Entity *CATALYST_RESTRICT> children;

	//The components index for this entity.
	size_t componentsIndex;

private:

	//Friend declarations.
	friend class EntitySystem;

};