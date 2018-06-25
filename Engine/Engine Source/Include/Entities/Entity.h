#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Math.
#include <Math/Vector3.h>

//Declares an entity class.
#define DECLARE_ENTITY_CLASS(EntityClass) static DynamicArray<EntityClass *RESTRICT> Instances;

//Defines an entity class.
#define DEFINE_ENTITY_CLASS(EntityClass) DynamicArray<EntityClass *RESTRICT> EntityClass::Instances;

class Entity
{

public:

	//Declare the entity class.
	DECLARE_ENTITY_CLASS(Entity);

	//Enumeration covering all entity types.
	enum class EntityType : uint8
	{
		Terrain,

		NumberOfEntityTypes
	};

	/*
	*	Default constructor.
	*/
	Entity() NOEXCEPT;

	/*
	*	Default destructor.
	*/
	virtual ~Entity() NOEXCEPT;

	/*
	*	Returns the component index.
	*/
	uint64 GetComponentsIndex() const NOEXCEPT { return componentsIndex; }

	/*
	*	Returns the entity type.
	*/
	EntityType GetEntityType() const NOEXCEPT { return entityType; }

	/*
	*	Returns a pointer to the parent of this entity.
	*/
	RESTRICTED const Entity* GetParent() const NOEXCEPT { return parent; }

	/*
	*	Sets the parent of this entity.
	*/
	void SetParent(Entity *RESTRICT newParent) NOEXCEPT { parent = newParent; }

	/*
	*	Returns the children of this entity.
	*/
	const DynamicArray<Entity *RESTRICT>& GetChildren() const NOEXCEPT { return children; }

	/*
	*	Adds a child to this entity.
	*/
	void AddChild(Entity *RESTRICT newChildEntity) NOEXCEPT { children.EmplaceSlow(newChildEntity); }

	/*
	*	Marks this entitiy for destruction.
	*/
	void MarkForDestruction() NOEXCEPT;

	/*
	*	Returns the position of this entity.
	*/
	virtual const Vector3& GetPosition() const NOEXCEPT = 0;

	/*
	*	Returns the rotation of this entity.
	*/
	virtual const Vector3& GetRotation() const NOEXCEPT = 0;

	/*
	*	Returns the scale of this entity.
	*/
	virtual const Vector3& GetScale() const NOEXCEPT = 0;

	/*
	*	Moves this entity.
	*/
	virtual void Move(const Vector3 &moveVector) NOEXCEPT = 0;

	/*
	*	Rotates this entity.
	*/
	virtual void Rotate(const Vector3 &rotateVector) NOEXCEPT = 0;

	/*
	*	Scales this entity.
	*/
	virtual void Scale(const Vector3 &scaleVector) NOEXCEPT = 0;

protected:

	//The default position.
	static constexpr Vector3 defaultPosition{ 0.0f, 0.0f, 0.0f };

	//The default rotation.
	static constexpr Vector3 defaultRotation{ 0.0f, 0.0f, 0.0f };

	//The default scale.
	static constexpr Vector3 defaultScale{ 1.0f, 1.0f, 1.0f };

	//The entity type.
	EntityType entityType{ EntityType::NumberOfEntityTypes };

	//The components index for this entity.
	uint64 componentsIndex;

	//Pointer to this entity's parent.
	Entity *RESTRICT parent{ nullptr };

	//Pointers to this entity's children.
	DynamicArray<Entity *RESTRICT> children;

private:

	//Friend declarations.
	friend class EntitySystem;

};