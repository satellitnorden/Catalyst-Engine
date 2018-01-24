#pragma once

//Engine core.
#include <EngineCore.h>

//Enumeration covering all component types.
enum class ComponentType : uint8
{
	PhysicsComponent,
	NumberOfComponentTypes
};

//Forward declarations.
class Entity;

class Component
{

public:

	/*
	*	Default constructor.
	*/
	Component() CATALYST_NOEXCEPT;

	/*
	*	Constructor taking all values as arguments.
	*/
	Component(const ComponentType initialComponentType, Entity *CATALYST_RESTRICT initialOwner) CATALYST_NOEXCEPT;

	/*
	*	Default destructor.
	*/
	virtual ~Component() CATALYST_NOEXCEPT;

	/*
	*	Returns the component type of this component.
	*/
	ComponentType GetComponentType() const CATALYST_NOEXCEPT { return componentType; }

	/*
	*	Sets the component type of this component.
	*/
	void SetComponentType(const ComponentType newComponentType) CATALYST_NOEXCEPT { componentType = newComponentType; }

	/*
	*	Returns the owner of this component, const.
	*/
	CATALYST_RESTRICTED const Entity* GetOwner() const CATALYST_NOEXCEPT { return owner; }

	/*
	*	Returns the owner of this component, non-const.
	*/
	CATALYST_RESTRICTED Entity* GetOwner() CATALYST_NOEXCEPT { return owner; }

private:

	//The component type of this component.
	ComponentType componentType;

	//Pointer to the owner entity of this component.
	Entity *CATALYST_RESTRICT owner;

};